#ifndef ANTVIEW_PRIMITIVES_HPP_
#define ANTVIEW_PRIMITIVES_HPP_

#include <stree/stree.hpp>

void init_environment(stree::Environment& env);

namespace ant {

stree::Value forward(const stree::Arguments&, stree::DataPtr ant);
stree::Value left(const stree::Arguments&, stree::DataPtr ant);
stree::Value right(const stree::Arguments&, stree::DataPtr ant);
stree::Value progn(const stree::Arguments&, stree::DataPtr ant);
unsigned if_food_ahead(const stree::Arguments&, stree::DataPtr ant);

}

#endif
