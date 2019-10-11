#include "sdl.hpp"
#include <iostream>

void SdlApp::run(const char* title, int width, int height) {
    init(title, width, height);
    is_running_ = true;
    while (is_running_) {
        handle_events();
        update();
        render();
    }
    cleanup();
}

void SdlApp::stop() {
    is_running_ = false;
}

void SdlApp::init(const char* title, int width, int height) {
    int flags = SDL_WINDOW_SHOWN;
    bool ok = true;

    // Init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        ok = false;
        std::cerr << "Failed to initialize SDL" << std::endl;
    }

    // Init window
    if (ok) {
        window_ = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height,
            flags);
        ok = window_;
        if (!ok) {
            std::cerr << "Failed to create window" << std::endl;
        }
    }

    // Init renderer
    if (ok) {
        renderer_ = SDL_CreateRenderer(window_, -1, 0);
        ok = renderer_;
        if (!ok) {
            std::cerr << "Failed to create renderer" << std::endl;
        }
    }

    // After init
    if (ok) {
        ok = after_init();
    }

    is_running_ = ok;
}

void SdlApp::handle_events() {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        if (event.type != SDL_QUIT)  {
            handle_event(event);
        } else {
            stop();
        }
    }
}

void SdlApp::render() {
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
    SDL_RenderClear(renderer_);
    do_render();
    SDL_RenderPresent(renderer_);
    after_render();
}

void SdlApp::cleanup() {
    if (window_) SDL_DestroyWindow(window_);
    if (renderer_) SDL_DestroyRenderer(renderer_);
    SDL_Quit();
}
