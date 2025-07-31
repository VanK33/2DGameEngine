#include "WeaponFollowSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "examples/zombie_survivor/ecs/components/InputComponent.hpp"
#include <iostream>
#include <cmath>

namespace ZombieSurvivor::System {

void WeaponFollowSystem::Init() {
    std::cout << "[WeaponFollowSystem] Initialized" << std::endl;
}

void WeaponFollowSystem::Update(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();
    auto entities = componentManager.GetEntitiesWithComponent<Component::FollowComponent>();

    for (const auto& entityId : entities) {
        auto* followComponent = componentManager.GetComponent<Component::FollowComponent>(entityId);
        if (!followComponent) continue;

        UpdateFollowPosition(entityId, *followComponent, deltaTime);
        
        // Copy input from player to weapon so weapon can aim independently
        CopyInputFromPlayer(entityId, *followComponent);
    }
}

void WeaponFollowSystem::UpdateFollowPosition(engine::EntityID followerId, const Component::FollowComponent& follow, float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();
    
    // Get the follower's transform
    auto* followerTransform = componentManager.GetComponent<engine::ECS::Transform2D>(followerId);
    if (!followerTransform) return;

    // Get the target's transform (player)
    auto* targetTransform = componentManager.GetComponent<engine::ECS::Transform2D>(follow.targetEntityId);
    if (!targetTransform) return;

    // Get weapon's input component to access mouse position
    auto* weaponInput = componentManager.GetComponent<Component::InputComponent>(followerId);
    if (!weaponInput) return;

    // Calculate weapon position using clock needle logic
    // Since player now uses pivotOffset {0.5, 0.5}, Transform2D position is already the center
    engine::Vector2 playerPos{targetTransform->x, targetTransform->y};
    engine::Vector2 mousePos = weaponInput->mousePosition;
    
    // 1. Calculate angle from player to mouse
    float playerToMouseAngle = std::atan2(mousePos.y - playerPos.y, mousePos.x - playerPos.x);
    
    // 2. Needle direction = mouse direction + 90 degrees (perpendicular)
    float needleAngle = playerToMouseAngle + M_PI / 2.0f;
    
    // 3. Calculate weapon attachment position (needle tip)
    float needleLength = 25.0f; // Distance from player center to weapon grip point
    engine::Vector2 weaponAttachPos{
        playerPos.x + std::cos(needleAngle) * needleLength,
        playerPos.y + std::sin(needleAngle) * needleLength
    };
    
    // 4. Set weapon position to attachment point
    // The weapon's pivotOffset is {0.25, 0.5}, so its grip point will be properly positioned
    // WeaponAttachPos is where we want the weapon's grip point to be
    followerTransform->x = weaponAttachPos.x;
    followerTransform->y = weaponAttachPos.y;

}

void WeaponFollowSystem::CopyInputFromPlayer(engine::EntityID weaponId, const Component::FollowComponent& follow) {
    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();
    
    // Get player's input component (source of mouse data)
    auto* playerInput = componentManager.GetComponent<Component::InputComponent>(follow.targetEntityId);
    if (!playerInput) return;

    // Get weapon's input component (needs mouse data for independent aiming)
    auto* weaponInput = componentManager.GetComponent<Component::InputComponent>(weaponId);
    if (!weaponInput) return;

    // Copy mouse position from player to weapon so weapon can aim at same target
    weaponInput->mousePosition = playerInput->mousePosition;
}

void WeaponFollowSystem::Shutdown() {
    std::cout << "[WeaponFollowSystem] Shutdown" << std::endl;
}

} // namespace ZombieSurvivor::System