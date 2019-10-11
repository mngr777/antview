#ifndef ANTVIEW_ANT_HPP_
#define ANTVIEW_ANT_HPP_

#include <set>
#include <utility>
#include <ostream>

using Coord = int;
using Pos = std::pair<Coord, Coord>;
using Trail = std::set<Pos>;


std::ostream& operator<<(std::ostream& os, const Trail& trail);
std::ostream& operator<<(std::ostream& os, const Pos& pos);
class Ant;
std::ostream& operator<<(std::ostream& os, const Ant& ant);

class Ant {
public:
    enum Dir { N, E, S, W };

    static Coord norm_x(Coord x);
    static Coord norm_y(Coord y);

    static const Coord MaxX = 32;
    static const Coord MaxY = 32;

    Ant();
    Ant(const Trail& trail);
    Ant(Dir dir, Coord x, Coord y, const Trail& trail);

    void forward();
    void left();
    void right();

    bool is_food_ahead() const;

    Dir dir() const {
        return dir_;
    }

    Coord x() const {
        return x_;
    }

    Coord y() const {
        return y_;
    }

    unsigned food_eaten() const {
        return food_eaten_;
    }

    unsigned food_left() const {
        return trail_.size();
    }

    const Trail& trail() const {
        return trail_;
    }

private:
    bool is_food_at_pos(Coord x, Coord y) const;

    void eat();

    Dir dir_;
    Coord x_;
    Coord y_;
    Trail trail_;
    unsigned food_eaten_;
};

#endif
