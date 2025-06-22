// src/engine/core/ecs/components/Velocity2D.hpp

#pragma once

#include <SDL3/SDL.h>

namespace engine::ECS {

struct Velocity2D {
    float vx = 0.0f, vy = 0.0f;
    float maxSpeed = 100.0f;
};

} // namespace engine::ECS