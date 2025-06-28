#pragma once

#include <vector>
#include <string>

namespace engine::ECS {

using EntityID = uint32_t;

enum class AIBehaviorType {
    IDLE,
    WANDER, 
    FOLLOW,
    PATROL,
    FLEE,
    SEEK
};

enum class AIState {
    ACTIVE,
    INACTIVE,
    STUNNED
};

struct Vector2 {
    float x = 0.0f, y = 0.0f;
};

struct AIComponent {
    // Core AI state
    AIBehaviorType currentBehavior = AIBehaviorType::IDLE;
    AIState state = AIState::ACTIVE;
    
    // Target system
    EntityID targetEntity = 0;
    float detectionRange = 100.0f;
    float actionRange = 50.0f;
    
    // Movement
    float moveSpeed = 50.0f;
    float maxSpeed = 100.0f;
    
    // Patrol system
    std::vector<Vector2> patrolPoints;
    size_t currentPatrolIndex = 0;
    float patrolWaitTime = 2.0f;
    
    // Timing
    float stateTimer = 0.0f;
    float behaviorTimer = 0.0f;
    
    // Behavior parameters
    float wanderRadius = 200.0f;
    float fleeDistance = 150.0f;
    
    // Debug/identification
    std::string aiTag = "default";
    bool debugMode = false;
};

} // namespace engine::ECS
