// src/engine/core/ecs/components/Lifetime.hpp

#pragma once

#include <SDL3/SDL.h>

namespace engine::ECS {

struct Lifetime {
    float remainingTime = 0.0f;
    bool destroyOnExpire = true;
};

} // namespace engine::ECS