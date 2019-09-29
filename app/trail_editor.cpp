#include "trail_editor.hpp"
#include <iostream>
#include "texture_manager.hpp"

static const char FoodTextureId[] = "food";

bool TrailEditorApp::after_init() {
    return load_textures();
}

void TrailEditorApp::handle_event(const SDL_Event& event) {
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
            toggle_trail_pos(mouse_motion_to_pos(event.motion));
            break;

        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_SPACE)
                std::cout << trail_ << std::endl;
            break;
    }
}

void TrailEditorApp::do_render() {
    render_grid();
    render_trail();
}

bool TrailEditorApp::load_textures() {
    bool ok = true;
    if (!texture_manager_.load(renderer_, FoodTextureId, "food.png")) {
        ok = false;
        std::cerr << "failed to load food image" << std::endl;
    }
    return ok;
}

void TrailEditorApp::render_grid() {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);

    // Horizontal lines
    for (int i = 0; i < grid_y_; ++i) {
        SDL_RenderDrawLine(
            renderer_,
            0, cell_size_ * i,
            cell_size_ * grid_x_, cell_size_ * i);
    }
    // Vertical lines
    for (int i = 0; i < grid_x_; ++i) {
        SDL_RenderDrawLine(
            renderer_,
            cell_size_ * i, 0,
            cell_size_ * i, cell_size_ * grid_y_);
    }
}

void TrailEditorApp::render_trail() {
    for (const Pos& pos : trail_) {
        SDL_Rect dst_rect;
        dst_rect.x = pos.first * cell_size_;
        dst_rect.y = pos.second * cell_size_;
        dst_rect.w = cell_size_;
        dst_rect.h = cell_size_;
        texture_manager_.draw(renderer_, FoodTextureId, dst_rect);
    }
}

void TrailEditorApp::toggle_trail_pos(const Pos& pos) {
    auto it = trail_.find(pos);
    if (it == trail_.end()) {
        trail_.insert(pos);
    } else {
        trail_.erase(it);
    }
}

Pos TrailEditorApp::mouse_motion_to_pos(const SDL_MouseMotionEvent& motion) {
    return Pos(motion.x / cell_size_, motion.y / cell_size_);
}
