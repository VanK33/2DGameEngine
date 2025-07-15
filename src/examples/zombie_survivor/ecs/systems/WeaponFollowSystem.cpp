#include "WeaponFollowSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "examples/zombie_survivor/ecs/components/InputComponent.hpp"
#include "examples/zombie_survivor/ecs/components/AimingComponent.hpp"
#include <iostream>

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
        
        // Also copy mouse input from player to weapon for aiming
        CopyMouseInputFromPlayer(entityId, *followComponent);
    }
}

void WeaponFollowSystem::UpdateFollowPosition(engine::EntityID followerId, const Component::FollowComponent& follow, float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();
    
    // Get the follower's transform
    auto* followerTransform = componentManager.GetComponent<engine::ECS::Transform2D>(followerId);
    if (!followerTransform) return;

    // Get the target's transform
    auto* targetTransform = componentManager.GetComponent<engine::ECS::Transform2D>(follow.targetEntityId);
    if (!targetTransform) return;

    // Get weapon's aiming component to get aim direction
    auto* weaponAiming = componentManager.GetComponent<Component::AimingComponent>(followerId);
    if (!weaponAiming) return;

    // Calculate weapon position like a clock hand
    // Center point: player position + offset (5 pixels to the left)
    float centerX = targetTransform->x + follow.offset.x;
    float centerY = targetTransform->y + follow.offset.y;
    
    // Calculate angle from aim direction
    float angle = std::atan2(weaponAiming->aimDirection.y, weaponAiming->aimDirection.x);
    
    // Position weapon at the center (like clock center)
    // The weapon will rotate around this point, but start at center
    followerTransform->x = centerX;
    followerTransform->y = centerY;
    
    // Set rotation to point in aim direction
    followerTransform->rotation = angle;
}

void WeaponFollowSystem::CopyMouseInputFromPlayer(engine::EntityID weaponId, const Component::FollowComponent& follow) {
    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();
    
    // Get player's input component (source of mouse data)
    auto* playerInput = componentManager.GetComponent<Component::InputComponent>(follow.targetEntityId);
    if (!playerInput) return;

    // Get weapon's input component (needs mouse data for AimingSystem)
    auto* weaponInput = componentManager.GetComponent<Component::InputComponent>(weaponId);
    if (!weaponInput) return;

    // Copy mouse position from player to weapon's input component
    weaponInput->mousePosition = playerInput->mousePosition;
}

void WeaponFollowSystem::Shutdown() {
    std::cout << "[WeaponFollowSystem] Shutdown" << std::endl;
}

} // namespace ZombieSurvivor::System