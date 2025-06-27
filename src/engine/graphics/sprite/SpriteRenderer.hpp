// src/engine/graphics/sprite/SpriteRenderer.hpp

#pragma once

#include <SDL3/SDL.h>

namespace engine::graphics {

class SpriteRenderer {
public:
    explicit SpriteRenderer(SDL_Renderer* renderer);

    void Draw(SDL_Texture* texture, float x, float y, float width, float height, float rotation = 0.0f, SDL_FlipMode flip = SDL_FLIP_NONE);

private:
    SDL_Renderer* renderer_;
};

} // namespace engine::graphics

