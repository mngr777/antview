#include <cassert>
#include <iostream>
#include <stree/stree.hpp>
#include "app/ant_viewer.hpp"
#include "ant.hpp"
#include "data.hpp"
#include "primitives.hpp"

static void usage(const std::string& name);

static Trail load_trail_or_exit(const std::string& filename);

static stree::Tree load_tree_or_exit(
    stree::Environment& env,
    const std::string& filename);


int main(int argc, char** argv) {
    // Check arguments
    if (argc < 3) usage(argv[0]);

    // Initialize environment
    stree::Environment env;
    env.add_function("forward", 0, &ant::forward);
    env.add_function("left", 0, &ant::left);
    env.add_function("right", 0, &ant::right);
    env.add_function("progn2", 2, &ant::progn);
    env.add_function("progn3", 3, &ant::progn);
    env.add_select_function("is-food-ahead", 2, 0, &ant::forward);

    // Load data
    stree::Tree tree(load_tree_or_exit(env, argv[1]));
    Trail trail(load_trail_or_exit(argv[2]));

    AntViewerApp app(&env);
    app.set_trail(std::move(trail));
    app.set_tree(std::move(tree));
    app.run("Ant Viewer", Ant::MaxX * 32, Ant::MaxY * 32);

    return 0;
}


void usage(const std::string& name) {
    using namespace std;
    cout << "Usage:" << endl
         << name << " <tree-filename> <trail-filename>" << endl;
    exit(-1);
}

Trail load_trail_or_exit(const std::string& filename) {
    try {
        return load_trail(filename);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(-1);
    }
    assert(false);
}

stree::Tree load_tree_or_exit(
    stree::Environment& env,
    const std::string& filename)
{
    try {
        return load_tree(env, filename);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(-1);
    }
    assert(false);
}
