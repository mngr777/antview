#ifndef ANTVIEW_APP_TEXTURE_MANAGER_HPP_
#define ANTVIEW_APP_TEXTURE_MANAGER_HPP_

#include <map>
#include <string>
#include <SDL2/SDL.h>

class TextureManager {
public:
    TextureManager();
    ~TextureManager();

    bool load(
        SDL_Renderer* renderer,
        const std::string& id,
        const std::string& filename);

    void draw(
        SDL_Renderer* renderer,
        const std::string& id,
        const SDL_Rect& rect_src,
        const SDL_Rect& rect_dst,
        float angle = 0.0,
        SDL_RendererFlip flip = SDL_FLIP_NONE);

    void draw(
        SDL_Renderer* renderer,
        const std::string& id,
        const SDL_Rect& rect_dst,
        float angle = 0.0,
        SDL_RendererFlip flip = SDL_FLIP_NONE);

private:
    struct TextureItem {
        TextureItem(SDL_Texture* texture, int width, int height)
            : texture(texture),
              width(width),
              height(height) {}

        SDL_Texture* texture;
        int width;
        int height;
    };

    using TextureMap = std::map<std::string, TextureItem>;

    TextureMap texture_map_;
};

#endif
