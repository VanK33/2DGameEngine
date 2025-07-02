// src/engine/core/ecs/components/AIComponent.hpp
#pragma once

#include "engine/core/Types.hpp"

namespace engine::ECS {

// 基础 AI 状态
enum class AIState {
    INACTIVE,
    ACTIVE,
    DISABLED
};

struct AIComponent {
    AIState state = AIState::ACTIVE;
    EntityID targetEntity = 0;
    Vector2 targetPosition{0, 0};
    
    float speed = 100.0f;
    float detectionRadius = 200.0f;
    
    float updateTimer = 0.0f;
    float updateInterval = 0.1f;
};

} // namespace engine::ECS