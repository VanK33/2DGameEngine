// src/engine/core/ecs/components/SpriteAnimation.hpp

#pragma once

namespace engine::ECS {

struct SpriteAnimation {
    int frameCount = 1;           // Total frames in animation
    int framesPerRow = 1;         // Frames per horizontal row
    int frameWidth = 0;           // Width of each frame (0 = auto-calculate)
    int frameHeight = 0;          // Height of each frame (0 = auto-calculate)
    float frameDuration = 0.1f;   // Seconds per frame
    bool loop = true;             // Should animation loop?
    bool autoPlay = true;         // Start playing immediately?
};

} // namespace engine::ECS