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
    // Load trail
    if (argc < 2)
        usage(argv[0]);
    Trail trail = load_trail_or_exit(argv[1]);

    // Load config
    auto config = (argc < 3)
        ? make_default_config()
        : load_config(argv[2]);

    // Fallback to random value if PRNG seed is not provided
    if (config.get<unsigned>(conf::PrngSeed) == (unsigned) -1) {
        std::srand(std::time(nullptr));
        config.set<unsigned>(conf::PrngSeed, std::rand());
    }

    std::cout << "PRNG seed              = "
              << config.get<unsigned>(conf::PrngSeed)
              << std::endl;
    std::cout << "# of crossovers        = "
              << config.get<unsigned>(conf::CrossoverNum)
              << std::endl;
    std::cout << "# of mutations         = "
              << config.get<unsigned>(conf::MutationNum)
              << std::endl;
    std::cout << "# of reproductions     = "
              << config.get<unsigned>(conf::ReproductionNum)
              << std::endl;
    std::cout << "# of subtree mutations = "
              << config.get<unsigned>(conf::MutationSubtreeNum)
              << std::endl;
    std::cout << "# of point mutations   = "
              << config.get<unsigned>(conf::MutationPointNum)
              << std::endl;
    std::cout << "# of hoist mutations   = "
              << config.get<unsigned>(conf::MutationHoistNum)
              << std::endl;

    // Random engine
    auto PrngSeed = config.get<unsigned>(conf::PrngSeed);
    std::mt19937 prng(PrngSeed);

    // Evaluator
    Evaluator evaluator(trail, config.get<unsigned>(conf::StepLimit));

    // Initialize environment
    stree::Environment env;
    init_environment(env);

    // initialize GP context
    auto context = stree::gp::make_context<Individual>(
        config, env, evaluator, prng);

    // Initialize population
    unsigned generation = 0;
    std::cout << "Generation 0" << std::endl;
    Population pop_current;
    stree::gp::ramped_half_and_half(context, pop_current);

    stree::NodeManagerStats node_stats;
    bool done = false;
    do {
        // Output stree stats
        if (config.get<unsigned>(conf::ShowNodeStats)) {
            node_stats.update(env.node_manager());
            std::cout << node_stats << std::endl;
        }

        // Reap results
        Group best = stree::gp::reap<Individual>(
            pop_current, config.get<unsigned>(conf::ResultNum), evaluator);
        done = (generation == config.get<unsigned>(conf::GenerationMax))
            || stree::gp::is_goal_achieved<Individual>(
                best, config.get<float>(conf::FitnessGoal), evaluator);
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
            max_index += config.get<unsigned>(conf::CrossoverNum);
            for (; index < max_index; ++index) {
                // Select parents
                const auto& parent1 = stree::gp::selection_tournament(context, pop_current);
                const auto& parent2 = stree::gp::selection_tournament(context, pop_current);
                Individual child = stree::gp::crossover_random(context, parent1, parent2);

                // Add child
                pop_next.emplace_back(std::move(child));
            }

            /// Mutation
            // Random subtree mutation
            max_index += config.get<unsigned>(conf::MutationSubtreeNum);
            for (; index < max_index; ++index) {
                const auto& individual = stree::gp::selection_tournament(context, pop_current);
                pop_next.emplace_back(stree::gp::mutate_subtree(context, individual));
            }
            // Point mutation
            max_index += config.get<unsigned>(conf::MutationPointNum);
            for (; index < max_index; ++index) {
                Individual& individual = stree::gp::selection_tournament(context, pop_current);
                pop_next.emplace_back(stree::gp::mutate_point(context, individual));
            }
            // Hoist mutation
            max_index += config.get<unsigned>(conf::MutationHoistNum);
            for (; index < max_index; ++index) {
                const auto& individual = stree::gp::selection_tournament(context, pop_current);
                pop_next.emplace_back(stree::gp::mutate_hoist(context, individual));
            }

            /// Reproduction
            while (pop_next.size() < pop_current.size()) {
                const auto& individual = stree::gp::selection_tournament(context, pop_current);
                pop_next.emplace_back(individual.copy());
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
         << name << " <trail-filename> [<config-filename>]" << endl;
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
    using namespace stree;

    // Make ant
    Ant ant(trail);
    // Run program
    Exec exec(
        individual.tree(),
        Exec::FlagRunLoop | Exec::FlagStopIfCostNotZero);
    Params params;
    exec.init(&params, static_cast<DataPtr>(&ant));
    exec.set_cost_limit(600);
    try {
        exec.run();
    } catch (ExecCostLimitExceeded& e) {
        // do nothing
    }
    return static_cast<float>(ant.food_left()) / trail.size();
}

Fitness Evaluator::operator()(Individual& individual) {
    return evaluate(individual, trail, step_limit);
}
