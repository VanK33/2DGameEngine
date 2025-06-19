// src/engine/core/ecs/WorldState.hpp
#pragma once

#include <string>

namespace engine::ECS {

class WorldState {
public:
    void SetPaused(bool paused);
    bool IsPaused() const;

    void SetSceneName(const std::string& name);
    const std::string& GetSceneName() const;

    void IncrementFrame();
    int GetFrameCount() const;

private:
    bool paused_ = false;
    std::string currentSceneName_;
    int frameCount_ = 0;
};

} // namespace engine::ECS
