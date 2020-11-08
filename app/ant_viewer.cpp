#include "ant_viewer.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include "texture_manager.hpp"

static const char AntTextureId[]  = "ant";
static const char FoodTextureId[] = "food";

void AntViewerApp::reset() {
    ant_ = Ant(trail_);
    if (exec_) exec_->restart();
}

void AntViewerApp::step() {
    if (exec_) {
        exec_->step();
        std::cout << ant_ << std::endl;
    }
}

void AntViewerApp::print_backtrace() {
    if (exec_) {
        stree::ExecDebug debug(*exec_);
        debug.print_backtrace(std::cout);
        std::cout << std::endl;
    }
}

void AntViewerApp::set_trail(Trail trail) {
    trail_ = std::move(trail);
    reset();
}

void AntViewerApp::set_tree(stree::Tree&& tree) {
    tree_.swap(std::move(tree));
    exec_.reset(
        new stree::Exec(
            tree_,
            stree::Exec::FlagStopIfCostNotZero));
    exec_->init(&params_, &ant_);
    exec_->set_cost_limit(0);
    reset();
}

bool AntViewerApp::after_init() {
    return load_textures();
}

void AntViewerApp::handle_event(const SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            on_keydown(event.key);
            break;
    }
}

void AntViewerApp::do_render() {
    render_grid();
    render_trail();
    render_ant();
}

void AntViewerApp::after_render() {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void AntViewerApp::on_keydown(const SDL_KeyboardEvent& event) {
    switch (event.keysym.sym) {
        case SDLK_SPACE:
            step();
            break;
        case SDLK_b:
            print_backtrace();
            break;

    }
}

bool AntViewerApp::load_textures() {
    bool ok = true;
    // Ant texture
    if (!texture_manager_.load(renderer_, AntTextureId, "ant.png")) {
        ok = false;
        std::cerr << "failed to load ant image" << std::endl;
    }
    // Food texture
    if (!texture_manager_.load(renderer_, FoodTextureId, "food.png")) {
        ok = false;
        std::cerr << "failed to load food image" << std::endl;
    }
    return ok;
}

void AntViewerApp::render_grid() {
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

void AntViewerApp::render_ant() {
    // angle
    float angle = 0.0;
    switch (ant_.dir()) {
        case Ant::N: break;
        case Ant::E: angle = 90.0; break;
        case Ant::S: angle = 180.0; break;
        case Ant::W: angle = 270.0; break;
    }
    // rect
    SDL_Rect dst_rect;
    dst_rect.x = ant_.x() * cell_size_;
    dst_rect.y = ant_.y() * cell_size_;
    dst_rect.w = cell_size_;
    dst_rect.h = cell_size_;
    // draw
    texture_manager_.draw(renderer_, AntTextureId, dst_rect, angle);
}

void AntViewerApp::render_trail() {
    for (const Pos& pos : ant_.trail()) {
        // rect
        SDL_Rect dst_rect;
        dst_rect.x = pos.first * cell_size_;
        dst_rect.y = pos.second * cell_size_;
        dst_rect.w = cell_size_;
        dst_rect.h = cell_size_;
        // draw
        texture_manager_.draw(renderer_, FoodTextureId, dst_rect);
    }
}
