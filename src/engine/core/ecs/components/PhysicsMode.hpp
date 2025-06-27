#pragma once

namespace engine::ECS {

enum class PhysicsMode {
    TOP_DOWN,      // No gravity, 2D plane movement
    SIDE_VIEW,     // Has gravity, Y-axis gravity
    ISOMETRIC,     // Isometric view
    CUSTOM         // Custom gravity direction
};

struct PhysicsModeComponent {
    PhysicsMode mode = PhysicsMode::TOP_DOWN;
    float gravityX = 0.0f;
    float gravityY = 0.0f;
    float gravityZ = 0.0f;
    bool enableGravity = false;
    bool enableFriction = true;
    float frictionFactor = 0.98f;
};

} // namespace engine::ECS 