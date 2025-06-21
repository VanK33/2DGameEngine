// src/engine/core/ecs/WorldState.cpp
#include "WorldState.hpp"

namespace engine::ECS {

void WorldState::SetPaused(bool paused) {
    paused_ = paused;
}

} // namespace engine::ECS