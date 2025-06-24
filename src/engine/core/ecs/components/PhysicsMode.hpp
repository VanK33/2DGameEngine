#pragma once

namespace engine::ECS {

enum class PhysicsMode {
    TOP_DOWN,      // 无重力，2D平面移动
    SIDE_VIEW,     // 有重力，Y轴重力
    ISOMETRIC,     // 等距视角
    CUSTOM         // 自定义重力方向
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