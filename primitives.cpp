#include "primitives.hpp"
#include <cassert>
#include "ant.hpp"

static Ant* ant_ptr(stree::DataPtr ant) {
    assert(ant);
    return static_cast<Ant*>(ant);
}

void init_environment(stree::Environment& env) {
    env.add_function("forward", 0, ant::forward);
    env.add_function("left", 0, ant::left);
    env.add_function("right", 0, ant::right);
    env.add_function("progn2", 2, ant::progn);
    env.add_function("progn3", 3, ant::progn);
    env.add_select_function("if-food-ahead", 2, 0, ant::if_food_ahead);
}

namespace ant {

stree::Value forward(const stree::Arguments&, stree::DataPtr ant) {
    ant_ptr(ant)->forward();
    return stree::Value();
}

stree::Value left(const stree::Arguments&, stree::DataPtr ant) {
    ant_ptr(ant)->left();
    return stree::Value();
}

stree::Value right(const stree::Arguments&, stree::DataPtr ant) {
    ant_ptr(ant)->right();
    return stree::Value();
}

stree::Value progn(const stree::Arguments&, stree::DataPtr) {
    // do nothing
    return stree::Value();
}

unsigned if_food_ahead(const stree::Arguments&, stree::DataPtr ant) {
    return ant_ptr(ant)->is_food_ahead() ? 0 : 1;
}

}
