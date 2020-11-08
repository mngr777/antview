#ifndef ANTVIEW_ANT_VIEWER_TRAIL_EDITOR_HPP_
#define ANTVIEW_ANT_VIEWER_TRAIL_EDITOR_HPP_

#include <memory>
#include <utility>
#include <SDL2/SDL.h>
#include <stree/stree.hpp>
#include "../ant.hpp"
#include "sdl.hpp"

class AntViewerApp : public SdlApp {
public:
    AntViewerApp(stree::Environment* env)
        : SdlApp(),
          tree_(env),
          cell_size_(32),
          grid_x_(32),
          grid_y_(32) {}

    void reset();
    void step();
    void print_backtrace();

    void set_trail(Trail trail);
    void set_tree(stree::Tree&& tree);

protected:
    virtual bool after_init();
    virtual void handle_event(const SDL_Event& event);
    virtual void do_render();
    virtual void after_render();

    void on_keydown(const SDL_KeyboardEvent& event);

    bool load_textures();
    void render_grid();
    void render_ant();
    void render_trail();
    void toggle_trail_pos(const Pos& pos);

    Pos mouse_motion_to_pos(const SDL_MouseMotionEvent& motion);

    std::unique_ptr<stree::Exec> exec_;
    stree::Tree tree_;
    stree::Params params_;
    Ant ant_;
    Trail trail_;

    int cell_size_;
    int grid_x_;
    int grid_y_;
};

#endif
