#ifndef ANTVIEW_APP_TRAIL_EDITOR_HPP_
#define ANTVIEW_APP_TRAIL_EDITOR_HPP_

#include <utility>
#include <SDL2/SDL.h>
#include "../ant.hpp"
#include "sdl.hpp"

class TrailEditorApp : public SdlApp {
public:
    TrailEditorApp()
        : SdlApp(),
          cell_size_(32),
          grid_x_(32),
          grid_y_(32) {}

    void set_trail(Trail trail) {
        trail_ = std::move(trail);
    }

protected:
    virtual bool after_init();
    virtual void handle_event(const SDL_Event& event);
    virtual void do_render();
    virtual void after_render();

    bool load_textures();
    void render_grid();
    void render_trail();
    void toggle_trail_pos(const Pos& pos);

    Pos mouse_motion_to_pos(const SDL_MouseMotionEvent& motion);

    Trail trail_;

    int cell_size_;
    int grid_x_;
    int grid_y_;
};

#endif
