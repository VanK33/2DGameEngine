// src/engine/core/ecs/WorldState.cpp
#include "WorldState.hpp"

namespace engine::ECS {

void WorldState::SetPaused(bool paused) {
    paused_ = paused;
}

bool WorldState::IsPaused() const {
    return paused_;
}

void WorldState::SetSceneName(const std::string& name) {
    currentSceneName_ = name;
}

const std::string& WorldState::GetSceneName() const {
    return currentSceneName_;
}

void WorldState::IncrementFrame() {
    frameCount_++;
}

int WorldState::GetFrameCount() const {
    return frameCount_;
}

} // namespace engine::ECS