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
    // Game world dimensions (850x850 game world)
    const float GAME_WORLD_WIDTH = 850.0f;
    const float GAME_WORLD_HEIGHT = 850.0f;
    
    // For center-based entities (player), we need to account for half the entity size
    // For top-left based entities, the bounds work as before
    const float halfSize = entitySize / 2.0f;
    
    // Boundary calculation for center-based positioning
    const float MIN_X = halfSize;                               // Left boundary (center can't go past half size)
    const float MAX_X = GAME_WORLD_WIDTH - halfSize;           // Right boundary 
    const float MIN_Y = halfSize;                               // Top boundary  
    const float MAX_Y = GAME_WORLD_HEIGHT - halfSize;          // Bottom boundary
    
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
    
    // For center-based positioning
    const float halfSize = boundary->entitySize / 2.0f;
    const float minX = boundary->minX + halfSize;
    const float maxX = boundary->maxX - halfSize;
    const float minY = boundary->minY + halfSize;
    const float maxY = boundary->maxY - halfSize;
    
    if (nextX < minX) {
        velocity->vx = std::max(0.0f, velocity->vx);
    } else if (nextX > maxX) {
        velocity->vx = std::min(0.0f, velocity->vx);
    }
    
    if (nextY < minY) {
        velocity->vy = std::max(0.0f, velocity->vy);
    } else if (nextY > maxY) {
        velocity->vy = std::min(0.0f, velocity->vy);
    }
}

} // namespace ZombieSurvivor::System