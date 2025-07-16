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
    int renderLayer = 0;  // 渲染层级，数值越大越靠前渲染
    
    // Pivot point for rotation (relative to sprite dimensions)
    // Default {-1, -1} means use sprite center
    // {0, 0} = top-left, {0.5, 0.5} = center, {1, 1} = bottom-right
    SDL_FPoint pivotOffset = {-1.0f, -1.0f};
};

} // namespace engine::ECS