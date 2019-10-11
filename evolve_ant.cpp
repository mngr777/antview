#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <stree/stree.hpp>
#include <streegp/streegp.hpp>
#include "ant.hpp"
#include "data.hpp"
#include "primitives.hpp"

using Individual = stree::gp::Individual;
using Population = stree::gp::Population<Individual>;
using Fitness = stree::gp::Fitness;

static void usage(const std::string& name);
static Trail load_trail_or_exit(const std::string& filename);

static void evaluate(Individual& individual, const Trail& trail, unsigned step_limit);

struct GetFitness {
    GetFitness(Trail trail, unsigned step_limit)
        : trail(std::move(trail)),
          step_limit(step_limit) {}

    Fitness operator()(Individual& individual);

    Trail trail;
    unsigned step_limit;
};

int main(int argc, char** argv) {
    /// Evaluation
    const unsigned StepLimit = 600;

    /// Initialization
    const unsigned PopulationSize = 100;
    const unsigned InitMaxDepth = 5;
    const float InitPTermGrow = 0.1;

    /// Crossover
    const unsigned CrossoverNum = 60;
    const unsigned CrossoverOnePointPTerm = 0.1;

    /// Mutation
    const unsigned MutationNum = 15;

    // Subtree mutation
    const unsigned MutationSubtreeNum = MutationNum / 3;
    const unsigned MutationSubtreeDepth = 3;
    const float MutationSubtreePTerm = 0.1;
    const float MutationSubtreePTermGrow = 0.1;

    // Point mutation
    const unsigned MutationPointNum = MutationNum / 3;
    const float MutationPointPTerm = 0.1;

    // Hoist mutation
    assert(MutationNum >= MutationSubtreeNum + MutationPointNum);
    const unsigned MutationHoistNum = MutationNum
        - (MutationSubtreeNum + MutationPointNum);
    const unsigned MutationHoistPTerm = 0.1;

    assert(PopulationSize >= CrossoverNum + MutationNum);


    // Load trail
    if (argc < 2)
        usage(argv[0]);
    Trail trail = load_trail_or_exit(argv[1]);

    // Fitness getter
    GetFitness get_fitness(trail, StepLimit);

    // Random engine
    const unsigned PrngSeed = 1;
    std::mt19937 prng(PrngSeed);

    // Initialize environment
    stree::Environment env;
    env.add_function("forward", 0, &ant::forward);
    env.add_function("left", 0, &ant::left);
    env.add_function("right", 0, &ant::right);
    env.add_function("progn2", 2, &ant::progn);
    env.add_function("progn3", 3, &ant::progn);
    env.add_select_function("is-food-ahead", 2, 0, &ant::forward);

    // Initialize population
    Population pop_current = stree::gp::ramped_half_and_half<Individual>(
        env, PopulationSize, InitMaxDepth, InitPTermGrow, prng);
    // TEST
    for (Individual& individual : pop_current) {
        std::cout << individual.tree() << std::endl
                  << get_fitness(individual) << std::endl
                  << std::endl;
    }

    return 0;
}


void usage(const std::string& name) {
    using namespace std;
    cout << "Usage:" << endl
         << name << " <trail-filename>" << endl;
    exit(-1);
}

Trail load_trail_or_exit(const std::string& filename) {
    try {
        Trail trail = load_trail(filename);
        if (trail.size() == 0)
            throw std::invalid_argument("Trail is empty");
        return trail;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(-1);
    }
    assert(false);
}

void evaluate(Individual& individual, const Trail& trail, unsigned step_limit) {
    // Make ant
    Ant ant(trail);
    // Run program
    stree::Exec exec(individual.tree());
    stree::Params params;
    exec.init(&params, static_cast<stree::DataPtr>(&ant));
    unsigned step = 1;
    while (ant.food_left() > 0 && step++ <= step_limit)
        exec.step();
    // Set individual fitness
    Fitness fitness = static_cast<float>(ant.food_left()) / trail.size();
    individual.set_fitness(fitness);
}

Fitness GetFitness::operator()(Individual& individual) {
    if (!individual.fitness())
        evaluate(individual, trail, step_limit);
    return individual.fitness();
}
