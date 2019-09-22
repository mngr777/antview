#include "texture_manager.hpp"
#include <SDL2/SDL_image.h>

TextureManager::TextureManager() {}

TextureManager::~TextureManager() {
    for (auto& item : texture_map_)
        SDL_DestroyTexture(item.second.texture);
}

bool TextureManager::load(
    const std::string& id,
    const std::string& filename,
    SDL_Renderer* renderer)
{
    // Load image into to surface
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface)
        return false;

    // Make texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Free surface
    SDL_FreeSurface(surface);

    if (texture) {
        // Get texture width and height
        int width, height;
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
        // Add texture to map
        texture_map_.emplace(id, texture, width, height);
        return true;
    } else {
        return false;
    }
}

void TextureManager::draw(
    SDL_Renderer* renderer,
    const std::string& id,
    const SDL_Rect& rect_src,
    const SDL_Rect& rect_dst,
    float angle,
    SDL_RendererFlip flip)
{
    SDL_Point center;
    center.x = rect_src.w / 2;
    center.y = rect_src.h / 2;
    SDL_RenderCopyEx(
        renderer,
        texture_map_[id].texture,
        &rect_src, &rect_dst,
        angle, &center,
        flip);
}

void TextureManager::draw(
    SDL_Renderer* renderer,
    const std::string& id,
    const SDL_Rect& rect_dst,
    float angle,
    SDL_RendererFlip flip)
{
    auto texture_item = texture_map_[id];
    // Default source rect
    SDL_Rect rect_src;
    rect_src.x = rect_src.y = 0;
    rect_src.w = texture_item.width;
    rect_src.h = texture_item.height;
    draw(
        renderer,
        id,
        rect_src, rect_dst,
        angle,
        flip);
}
