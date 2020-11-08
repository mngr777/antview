#include "data.hpp"
#include <cmath>
#include <fstream>
#include <sstream>
#include "trail_parser.hpp"

static std::ifstream open_file(const std::string& filepath);

static stree::gp::Config _make_default_config();
static void prepare_config(stree::gp::Config& config);
static void config_percent_to_num(
    stree::gp::Config& config,
    const char* percent_name,
    const char* num_name,
    const char* whole_num_name);

ConfigError::ConfigError(const std::string& what)
    : std::invalid_argument(
        std::string("Config error: ") + what) {}

stree::gp::Config make_default_config() {
    auto config = _make_default_config();
    prepare_config(config);
    return config;
}

stree::gp::Config load_config(const std::string& filename) {
    auto file = open_file(filename);
    auto config = make_default_config();
    config.read(file);
    prepare_config(config);
    return config;
}

Trail load_trail(const std::string& filename) {
    auto file = open_file(filename);
    TrailParser parser;
    parser.parse(file);
    if (!parser.is_done())
        throw TrailParserException(parser);
    return parser.result();
}

stree::Tree load_tree(stree::Environment& env, const std::string& filename) {
    auto file = open_file(filename);
    stree::Parser parser(&env);
    parser.parse(file);
    if (!parser.is_done())
        throw stree::ParserError(parser);
    return stree::Tree(&env, parser.move_result());
}

std::ifstream open_file(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file)
        throw std::invalid_argument("File not found");
    return file;
}

static stree::gp::Config _make_default_config() {
    auto config = stree::gp::make_default_config();
    config.set_order_step(1);

    config.set_order(50);
    config.set<unsigned>(conf::ShowNodeStats, 0);
    config.set<unsigned>(conf::PrngSeed, 1);
    config.set<unsigned>(conf::GenerationMax, 20);
    config.set<float>(conf::FitnessGoal, 0.0);
    config.set<unsigned>(conf::ResultNum, 10);
    config.set<unsigned>(conf::StepLimit, 600);

    config.set_order(250);
    config.set<unsigned>(conf::MutationNum, 0);
    config.set<unsigned>(conf::MutationSubtreeNum, 0);
    config.set<unsigned>(conf::MutationPointNum, 0);
    config.set<unsigned>(conf::MutationHoistNum, 0);
    config.set<float>(conf::MutationPercent, 10.0);
    config.set<float>(conf::MutationSubtreePercent, 100.0);
    config.set<float>(conf::MutationPointPercent, 0.0);
    config.set<float>(conf::MutationHoistPercent, 0.0);

    config.set_order(350);
    config.set<unsigned>(conf::CrossoverNum, 0);
    config.set<float>(conf::CrossoverPercent, 80.0);

    config.set_order(400);
    config.set<float>(conf::ReproductionPercent, 0.0);
    config.set<unsigned>(conf::ReproductionNum, 0);

    return config;
}

static void prepare_config(stree::gp::Config& config) {
    if (config.get<unsigned>(conf::GenerationMax) == 0)
        throw ConfigError("GenerationMax is zero");

    if (config.get<float>(conf::FitnessGoal) < 0.0)
        throw ConfigError("FitnessGoal is < 0");

    if (config.get<unsigned>(conf::StepLimit) == 0)
        throw ConfigError("StepLimit is zero");

    if (config.get<unsigned>(conf::ResultNum) == 0)
        throw ConfigError("ResultNum is zero");

    config_percent_to_num(
        config,
        conf::CrossoverPercent, conf::CrossoverNum,
        stree::gp::conf::PopulationSize);

    config_percent_to_num(
        config,
        conf::MutationPercent, conf::MutationNum,
        stree::gp::conf::PopulationSize);

    config_percent_to_num(
        config,
        conf::ReproductionPercent, conf::ReproductionNum,
        stree::gp::conf::PopulationSize);

    {
        // Check crossover/mutation/reproduction numbers
        unsigned p = config.get<unsigned>(stree::gp::conf::PopulationSize);
        unsigned c = config.get<unsigned>(conf::CrossoverNum);
        unsigned m = config.get<unsigned>(conf::MutationNum);
        unsigned r = config.get<unsigned>(conf::ReproductionNum);
        if (c + m + r > p)
            throw ConfigError(
                "CrossoverNum + MutationNum + ReproductionNum "
                "> PopulationSize");
        if (c + m + r < p) {
            if (c > 0 && m > 0 && r <= 0) {
                config.set<unsigned>(conf::ReproductionNum, p - c - m);
            } else if (m > 0 && r > 0 && c <= 0) {
                config.set<unsigned>(conf::CrossoverNum, p - m - r);
            } else if (c > 0 && m > 0 && r <= 0) {
                config.set<unsigned>(conf::MutationNum, p - r - c);
            } else {
                throw ConfigError(
                    "cannot calculate "
                    "CrossoverNum/MutationNum/ReproductionNum "
                    "numbers: "
                    + std::to_string(c) + "/"
                    + std::to_string(m) + "/"
                    + std::to_string(r));
            }
        }
    }

    config_percent_to_num(
        config,
        conf::MutationSubtreePercent, conf::MutationSubtreeNum,
        conf::MutationNum);

    config_percent_to_num(
        config,
        conf::MutationPointPercent, conf::MutationPointNum,
        conf::MutationNum);

    config_percent_to_num(
        config,
        conf::MutationHoistPercent, conf::MutationHoistNum,
        conf::MutationNum);

    {
        // Check mutation numbers
        unsigned m  = config.get<unsigned>(conf::MutationNum);
        unsigned ms = config.get<unsigned>(conf::MutationSubtreeNum);
        unsigned mp = config.get<unsigned>(conf::MutationPointNum);
        unsigned mh = config.get<unsigned>(conf::MutationHoistNum);
        if (ms + mp + mh != m)
            throw ConfigError(
                "mutation type numbers don't add up to mutation number");
    }
}

static void config_percent_to_num(
    stree::gp::Config& config,
    const char* percent_name,
    const char* num_name,
    const char* whole_num_name)
{
    float percent = config.get<float>(percent_name);

    // Validate percent value
    if (percent > 100) {
        throw std::invalid_argument(
            std::string("`") + percent_name + "' is > 100.0");
    }

    if (percent > 0.0) {
        // Check if num_name is already set
        if (config.get<unsigned>(num_name) > 0)
            throw ConfigError(
                std::string("both `")
                + percent_name + "' and `"
                + num_name + "' are > 0");
        // Calc. integer value
        unsigned whole = config.get<unsigned>(whole_num_name);
        unsigned value = static_cast<unsigned>(std::round(whole * percent / 100));
        assert(value <= whole);
        config.set<unsigned>(num_name, value);
    }
}
