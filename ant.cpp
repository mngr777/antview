#include "ant.hpp"
#include <cassert>

Ant::Ant()
    : Ant(Trail()) {}

Ant::Ant(const Trail& trail)
    : Ant(E, 0, 0, trail) {}

Ant::Ant(Dir dir, Coord x, Coord y, const Trail& trail)
    : dir_(dir), x_(x), y_(y),
      trail_(trail), eaten_(0) {}

void Ant::forward() {
    switch (dir_) {
        case N: y_ = norm_y(y_ - 1); break;
        case E: x_ = norm_y(x_ + 1); break;
        case S: y_ = norm_y(y_ + 1); break;
        case W: x_ = norm_x(x_ - 1); break;
    }
    eat();
}

void Ant::left() {
    switch (dir_) {
        case N: dir_ = W; break;
        case W: dir_ = S; break;
        case S: dir_ = E; break;
        case E: dir_ = N; break;
    }
}

void Ant::right() {
    switch (dir_) {
        case N: dir_ = E; break;
        case E: dir_ = S; break;
        case S: dir_ = W; break;
        case W: dir_ = N; break;
    }
}

bool Ant::is_food_ahead() const {
    switch (dir_) {
        case N: return is_food_at_pos(x_, norm_y(y_ - 1));
        case E: return is_food_at_pos(norm_x(x_ + 1), y_);
        case S: return is_food_at_pos(x_, norm_y(y_ + 1));
        case W: return is_food_at_pos(norm_x(x_ - 1), y_);
    }
    assert(false);
}

Ant::Coord Ant::norm_x(Coord x) {
    Coord xn = x % MaxX;
    if (xn < 0) xn = MaxX + xn;
    return xn;
}

Ant::Coord Ant::norm_y(Coord y) {
    Coord yn = y % MaxY;
    if (yn < 0) yn = MaxY + yn;
    return yn;
}

bool Ant::is_food_at_pos(Coord x, Coord y) const {
    return trail_.count(Pos(x, y)) > 0;
}

void Ant::eat() {
    auto it = trail_.find(Pos(x_, y_));
    if (it != trail_.end()) {
        trail_.erase(it);
        ++eaten_;
    }
}

