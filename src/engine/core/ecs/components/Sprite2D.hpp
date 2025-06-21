// src/engine/core/ecs/components/Sprite2D.hpp

#pragma once
#include <SDL3/SDL.h>
#include <string>

namespace engine::ECS {

struct Sprite2D {
    std::string texturePath;
    SDL_Rect sourceRect = {0, 0, 0, 0};
    bool visible = true;
    SDL_Color tint = {255, 255, 255, 255};
};

} // namespace engine::ECS