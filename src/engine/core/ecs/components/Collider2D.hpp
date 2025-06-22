// src/engine/core/ecs/components/Collider2D.hpp

#pragma once

#include <SDL3/SDL.h>
#include <string>

namespace engine::ECS {

struct Collider2D {
    SDL_FRect bounds = {0,0,0,0};
    bool isTrigger = false;
    std::string layer = "default";
};

} // namespace engine::ECS