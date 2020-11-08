#ifndef ANTVIEW_DATA_HPP_
#define ANTVIEW_DATA_HPP_

#include <stdexcept>
#include <string>
#include <stree/stree.hpp>
#include <streegp/streegp.hpp>
#include "ant.hpp"

namespace conf {

const char ShowNodeStats[]      = "show_node_stats";
const char PrngSeed[]           = "prng_seed";
const char GenerationMax[]      = "generation_max";
const char FitnessGoal[]        = "fitness_goal";
const char ResultNum[]          = "result_num";
const char StepLimit[]          = "step_limit";

const char MutationNum[]        = "mutation_num";
const char CrossoverNum[]       = "crossover_num";
const char MutationSubtreeNum[] = "mutation_subtree_num";
const char MutationPointNum[]   = "mutation_point_num";
const char MutationHoistNum[]   = "mutation_hoist_num";
const char ReproductionNum[]    = "reproduction_num";

const char MutationPercent[]        = "mutation_percent";
const char CrossoverPercent[]       = "crossover_percent";
const char MutationSubtreePercent[] = "mutation_subtree_percent";
const char MutationPointPercent[]   = "mutation_point_percent";
const char MutationHoistPercent[]   = "mutation_hoist_percent";
const char ReproductionPercent[]    = "reproduction_percent";

}

class ConfigError : public std::invalid_argument {
public:
    explicit ConfigError(const std::string& what);
};

stree::gp::Config make_default_config();
stree::gp::Config load_config(const std::string& filename);

Trail load_trail(const std::string& filename);

stree::Tree load_tree(stree::Environment& env, const std::string& filename);

#endif
