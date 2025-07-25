// src/examples/zombie_survivor/ecs/systems/BoundarySystem.cpp

#include "BoundarySystem.hpp"
#include "engine/core/ecs/World.hpp"
#include <iostream>

namespace ZombieSurvivor::System {

void BoundarySystem::Init() {
    std::cout << "[BoundarySystem] Initialized" << std::endl;
}

void BoundarySystem::Update(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();

    // Get all entities that need boundary constraints
    auto entities = componentManager.GetEntitiesWithComponents<
        engine::ECS::Transform2D,
        engine::ECS::Velocity2D,
        Component::BoundaryComponent
    >();

    for (const auto& entityId : entities) {
        auto* transform = componentManager.GetComponent<engine::ECS::Transform2D>(entityId);
        auto* velocity = componentManager.GetComponent<engine::ECS::Velocity2D>(entityId);
        auto* boundary = componentManager.GetComponent<Component::BoundaryComponent>(entityId);

        if (!transform || !velocity || !boundary || !boundary->enabled) continue;

        ApplyBoundaryConstraints(transform, velocity, boundary, deltaTime);
    }
}

void BoundarySystem::ApplyBoundaryConstraints(
    engine::ECS::Transform2D* transform,
    engine::ECS::Velocity2D* velocity,
    const Component::BoundaryComponent* boundary,
    float deltaTime
) const {
    switch (boundary->type) {
        case Component::BoundaryType::SCREEN_BOUNDS:
            ApplyScreenBounds(transform, velocity, boundary->entitySize, deltaTime);
            break;
        case Component::BoundaryType::CUSTOM_RECT:
            ApplyCustomBounds(transform, velocity, boundary, deltaTime);
            break;
    }
}

void BoundarySystem::ApplyScreenBounds(
    engine::ECS::Transform2D* transform,
    engine::ECS::Velocity2D* velocity,
    float entitySize,
    float deltaTime
) const {
    // Screen dimensions (matches main.cpp config)
    const float SCREEN_WIDTH = 800.0f;
    const float SCREEN_HEIGHT = 600.0f;
    
    // Boundary calculation: transform is top-left corner coordinate
    const float MIN_X = 0.0f;                           // Left boundary
    const float MAX_X = SCREEN_WIDTH - entitySize;      // Right boundary (800 - 64 = 736)
    const float MIN_Y = 0.0f;                           // Top boundary  
    const float MAX_Y = SCREEN_HEIGHT - entitySize;     // Bottom boundary (600 - 64 = 536)
    
    // Predict next frame position
    float nextX = transform->x + velocity->vx * deltaTime;
    float nextY = transform->y + velocity->vy * deltaTime;
    
    // Constrain X direction movement
    if (nextX < MIN_X) {
        // Would hit left boundary - stop leftward movement
        velocity->vx = std::max(0.0f, velocity->vx);
    } else if (nextX > MAX_X) {
        // Would hit right boundary - stop rightward movement
        velocity->vx = std::min(0.0f, velocity->vx);
    }
    
    // Constrain Y direction movement
    if (nextY < MIN_Y) {
        // Would hit top boundary - stop upward movement
        velocity->vy = std::max(0.0f, velocity->vy);
    } else if (nextY > MAX_Y) {
        // Would hit bottom boundary - stop downward movement
        velocity->vy = std::min(0.0f, velocity->vy);
    }
}

void BoundarySystem::ApplyCustomBounds(
    engine::ECS::Transform2D* transform,
    engine::ECS::Velocity2D* velocity,
    const Component::BoundaryComponent* boundary,
    float deltaTime
) const {
    // Custom boundary implementation
    float nextX = transform->x + velocity->vx * deltaTime;
    float nextY = transform->y + velocity->vy * deltaTime;
    
    // Constrain to custom boundaries
    const float maxX = boundary->maxX - boundary->entitySize;
    const float maxY = boundary->maxY - boundary->entitySize;
    
    if (nextX < boundary->minX) {
        velocity->vx = std::max(0.0f, velocity->vx);
    } else if (nextX > maxX) {
        velocity->vx = std::min(0.0f, velocity->vx);
    }
    
    if (nextY < boundary->minY) {
        velocity->vy = std::max(0.0f, velocity->vy);
    } else if (nextY > maxY) {
        velocity->vy = std::min(0.0f, velocity->vy);
    }
}

} // namespace ZombieSurvivor::System