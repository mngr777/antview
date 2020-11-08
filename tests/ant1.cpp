#include <iostream>
#include <string>
#include <stree/stree.hpp>
#include "../ant.hpp"
#include "../primitives.hpp"
#include "../trail_parser.hpp"

int main() {
    using namespace std;

    stree::Environment env;
    init_environment(env);

    std::string ant_str("(if-food-ahead (forward) (progn2 (left) (forward)))");
    std::string trail_str("((1 0)(2 2)(3 5))");

    // Parse trail
    TrailParser trail_parser;
    trail_parser.parse(trail_str);
    if (!trail_parser.is_done()) {
        cerr << "Cannot parse trail" << endl;
        return -1;
    }
    // Make ant
    Trail trail = trail_parser.result();
    Ant ant(trail);

    // Parse ant program
    stree::Parser parser(&env);
    parser.parse(ant_str);
    if (!parser.is_done()) {
        cerr << "Cannot parse ant program" << endl;
        return -1;
    }

    // Make tree
    stree::Tree tree(&env, parser.result());
    cout << "Ant program: " << tree << endl;

    // Make exec
    stree::Exec exec(tree, stree::Exec::FlagRunLoop | stree::Exec::FlagStopIfCostNotZero);
    stree::Params params;
    exec.init(&params, static_cast<stree::DataPtr>(&ant));

    // Run
    cout << ant << endl;
    for (unsigned step = 0; step < 10; ++step) {
        exec.step();
        cout << ant << endl;
    }

    return 0;
}
