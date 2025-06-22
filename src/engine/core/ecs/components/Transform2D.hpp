// src/engine/core/ecs/components/Transform2D.hpp

#pragma once

#include <SDL3/SDL.h>

namespace engine::ECS {

struct Transform2D {
    float x = 0.0f, y = 0.0f;
    float rotation = 0.0f;
    float scaleX = 1.0f, scaleY = 1.0f;
};    

} // namespace engine::ECS