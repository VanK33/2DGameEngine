// src/engine/graphics/sprite/SpriteRenderer.cpp

#include "SpriteRenderer.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <iostream>

namespace engine::graphics {

SpriteRenderer::SpriteRenderer(SDL_Renderer* renderer)
    : renderer_(renderer) {}

void SpriteRenderer::Draw(SDL_Texture* texture,
                          float x, float y,
                          float width, float height,
                          float rotation,
                          SDL_FlipMode flip) {
    if (!texture || !renderer_) return;

    SDL_FRect dstRect = { x, y, width, height };
    
    // Use simple SDL_RenderTexture for most cases
    if (rotation == 0.0f) {
        SDL_RenderTexture(renderer_, texture, nullptr, &dstRect);
    } else {
        SDL_FPoint center = { width / 2.0f, height / 2.0f };
        SDL_RenderTextureRotated(renderer_, texture, nullptr, &dstRect,
                                 rotation, &center, flip);
    }
}

} // namespace engine::graphics