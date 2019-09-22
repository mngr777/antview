#ifndef ANTVIEW_APP_SDL_HPP_
#define ANTVIEW_APP_SDL_HPP_

#include <SDL2/SDL.h>
#include "texture_manager.hpp"

class SdlApp {
public:
    SdlApp()
        : is_running_(false),
          window_(nullptr),
          renderer_(nullptr) {}

    void run(const char* title, int width, int height);
    void stop();

protected:
    virtual ~SdlApp() = 0;

    void init(const char* title, int width, int height);

    virtual bool after_init() { return true; }
    virtual void handle_events() = 0;
    virtual void update() {}

    void render();
    virtual void do_render() = 0;

    void cleanup();

    bool is_running_;

    SDL_Window* window_;
    SDL_Renderer* renderer_;
    TextureManager texture_manager_;
};

#endif
