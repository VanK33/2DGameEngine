// src/engine/core/ecs/components/AnimationState.hpp

#pragma once

namespace engine::ECS {

struct AnimationState {
    int currentFrame = 0;         // Current frame index (0-based)
    float elapsedTime = 0.0f;     // Time since current frame started
    bool isPlaying = true;        // Is animation currently playing?
    int loopCount = 0;            // Number of times animation has completed
    bool hasCompleted = false;    // Has non-looping animation finished?
};

} // namespace engine::ECS