#include <cassert>
#include <cstdlib>
#include <iostream>
#include <functional>
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
using Group = stree::gp::Group<Individual>;
using Fitness = stree::gp::Fitness;

static void usage(const std::string& name);
static Trail load_trail_or_exit(const std::string& filename);

static Fitness evaluate(Individual& individual, const Trail& trail, unsigned step_limit);

struct Evaluator {
    Evaluator(Trail trail, unsigned step_limit)
        : trail(std::move(trail)),
          step_limit(step_limit) {}

    Fitness operator()(Individual& individual);

    Trail trail;
    unsigned step_limit;
};

int main(int argc, char** argv) {
    const unsigned GenerationMax = 20;
    const Fitness FitnessGoal = 0.0;
    const unsigned TournamentSize = 10;
    const unsigned ResultNum = 10;

    /// Evaluation
    const unsigned StepLimit = 600;

    /// Initialization
    const unsigned PopulationSize = 100;
    const unsigned InitMaxDepth = 5;
    const float InitPTermGrow = 0.1;

    /// Crossover
    const unsigned CrossoverNum = 60;
    const unsigned CrossoverPTerm = 0.1;

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

    // Evaluator
    Evaluator evaluator(trail, StepLimit);

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
    unsigned generation = 0;
    std::cout << "Generation 0" << std::endl;
    Population pop_current = stree::gp::ramped_half_and_half<Individual>(
        env, PopulationSize, InitMaxDepth, InitPTermGrow, prng);

    bool done = false;
    do {
        // Reap results
        Group best = stree::gp::reap<Individual>(pop_current, ResultNum, evaluator);
        done = stree::gp::is_goal_achieved<Individual>(best, FitnessGoal, evaluator)
            || generation == GenerationMax;
        if (done) {
            std::cout << "Best results" << std::endl;
            for (auto item : best) {
                Individual& individual = item.get();
                std::cout << "[" << individual.fitness() << "] "
                          << individual.tree()
                          << std::endl;
            }
        } else {
            assert(best.size() > 0);
            std::cout << "Best fitness: ";
            for (auto item : best) {
                Individual& individual = item.get();
                std::cout << individual.fitness() << " ";
            }
            std::cout << std::endl;
        }

        if (!done) {
            ++generation;
            std::cout << std::endl;
            std::cout << "Generation " << generation << std::endl;
            Population pop_next;
            unsigned index = 0, max_index = 0;

            /// Crossover
            max_index += CrossoverNum;
            for (; index < max_index; ++index) {
                // Select parents
                Individual& parent1 = stree::gp::selection_tournament<Individual>(
                    pop_current, TournamentSize, prng, evaluator);
                Individual& parent2 = stree::gp::selection_tournament<Individual>(
                    pop_current, TournamentSize, prng, evaluator);
                // Apply crossover
                // stree::Tree child = stree::gp::crossover_one_point(
                //     parent1.tree(),
                //     parent2.tree(),
                //     CrossoverPTerm,
                //     prng);
                stree::Tree child = stree::gp::crossover_random(
                    parent1.tree(),
                    parent2.tree(),
                    CrossoverPTerm,
                    prng);
                // Add child
                pop_next.emplace_back(std::move(child));
            }

            /// Mutation
            // Random subtree mutation
            max_index += MutationSubtreeNum;
            for (; index < max_index; ++index) {
                Individual& individual = stree::gp::selection_tournament<Individual>(
                    pop_current, TournamentSize, prng, evaluator);
                pop_next.emplace_back(
                    stree::gp::mutate_subtree(
                        individual.tree(),
                        MutationSubtreeDepth,
                        MutationSubtreePTerm,
                        MutationSubtreePTermGrow,
                        prng));
            }
            // Point mutation
            max_index += MutationPointNum;
            for (; index < max_index; ++index) {
                Individual& individual = stree::gp::selection_tournament<Individual>(
                    pop_current, TournamentSize, prng, evaluator);
                pop_next.emplace_back(
                    stree::gp::mutate_point(
                        individual.tree(),
                        MutationPointPTerm,
                        prng));
            }
            // Hoist mutation
            max_index += MutationHoistNum;
            for (; index < max_index; ++index) {
                Individual& individual = stree::gp::selection_tournament<Individual>(
                    pop_current, TournamentSize, prng, evaluator);
                pop_next.emplace_back(
                    stree::gp::mutate_hoist(
                        individual.tree(),
                        MutationHoistPTerm,
                        prng));
            }

            /// Reproduction
            while (pop_next.size() < pop_current.size()) {
                Individual& individual = stree::gp::selection_tournament<Individual>(
                    pop_current, TournamentSize, prng, evaluator);
                pop_next.emplace_back(individual.tree().sub(0).copy());
            }

            /// Swap populations
            pop_current.swap(pop_next);
        }
    } while(!done);

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

Fitness evaluate(Individual& individual, const Trail& trail, unsigned step_limit) {
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
    return static_cast<float>(ant.food_left()) / trail.size();
}

Fitness Evaluator::operator()(Individual& individual) {
    return evaluate(individual, trail, step_limit);
}
