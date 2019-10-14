#ifndef ANTVIEW_DATA_HPP_
#define ANTVIEW_DATA_HPP_

#include <stree/stree.hpp>
#include "ant.hpp"

Trail load_trail(const std::string& filename);

stree::Tree load_tree(stree::Environment& env, const std::string& filename);

#endif
