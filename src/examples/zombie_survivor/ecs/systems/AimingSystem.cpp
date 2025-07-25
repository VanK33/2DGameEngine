// src/examples/zombie_survivor/ecs/systems/AimingSystem.cpp

#include "AimingSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include <iostream>

namespace ZombieSurvivor::System {

void AimingSystem::Init() {
    std::cout << "[AimingSystem] Initialized" << std::endl;
}

void AimingSystem::Update(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto entities = componentManager.GetEntitiesWithComponents<
        Component::AimingComponent,
        Component::InputComponent,
        engine::ECS::Transform2D
    >();
    
    for (const auto& entityId : entities) {
        auto* aiming = componentManager.GetComponent<Component::AimingComponent>(entityId);
        auto* input = componentManager.GetComponent<Component::InputComponent>(entityId);
        auto* transform = componentManager.GetComponent<engine::ECS::Transform2D>(entityId);
        
        if (!aiming || !input || !transform) continue;
        
        UpdateAiming(entityId);
    }
}

void AimingSystem::Shutdown() {
    std::cout << "[AimingSystem] Shutdown" << std::endl;
}

void AimingSystem::UpdateAiming(uint32_t entityId) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* aiming = componentManager.GetComponent<Component::AimingComponent>(entityId);
    auto* input = componentManager.GetComponent<Component::InputComponent>(entityId);
    auto* transform = componentManager.GetComponent<engine::ECS::Transform2D>(entityId);
    
    if (!aiming || !input || !transform) return;
    
    UpdateMouseWorldPos(entityId, input->mousePosition);
    
    engine::Vector2 entityPos{transform->x, transform->y};
    aiming->aimDirection = CalculateAimDirection(entityPos, aiming->mouseWorldPos);
    
    // Apply rotation to Transform2D so entity faces mouse direction
    transform->rotation = std::atan2(aiming->aimDirection.y, aiming->aimDirection.x);
}

engine::Vector2 AimingSystem::CalculateAimDirection(const engine::Vector2& fromPos, const engine::Vector2& toPos) const {
    // Note: In SDL, Y increases downward, but for aiming we want standard math coordinates
    // So we flip the Y direction to make aiming feel natural
    engine::Vector2 direction{toPos.x - fromPos.x, -(toPos.y - fromPos.y)};
    
    // Normalize direction vector
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0.001f) {
        direction.x /= length;
        direction.y /= length;
    } else {
        direction = engine::Vector2{1.0f, 0.0f};
    }
    
    return direction;
}

void AimingSystem::UpdateMouseWorldPos(uint32_t entityId, const engine::Vector2& mouseScreenPos) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto* aiming = world->GetComponentManager().GetComponent<Component::AimingComponent>(entityId);
    if (!aiming) return;
    
    // 简单的屏幕到世界坐标转换（1:1映射）
    // 将来可以添加相机变换
    aiming->mouseWorldPos = mouseScreenPos;
}

engine::Vector2 AimingSystem::GetAimDirection(uint32_t entityId) const {
    auto* world = GetWorld();
    if (!world) return engine::Vector2{1.0f, 0.0f};
    
    auto* aiming = world->GetComponentManager().GetComponent<Component::AimingComponent>(entityId);
    return aiming ? aiming->aimDirection : engine::Vector2{1.0f, 0.0f};
}

engine::Vector2 AimingSystem::GetMouseWorldPos(uint32_t entityId) const {
    auto* world = GetWorld();
    if (!world) return engine::Vector2{0.0f, 0.0f};
    
    auto* aiming = world->GetComponentManager().GetComponent<Component::AimingComponent>(entityId);
    return aiming ? aiming->mouseWorldPos : engine::Vector2{0.0f, 0.0f};
}

bool AimingSystem::ShouldShowAimLine(uint32_t entityId) const {
    auto* world = GetWorld();
    if (!world) return false;
    
    auto* aiming = world->GetComponentManager().GetComponent<Component::AimingComponent>(entityId);
    if (!aiming) return false;
    
    if (!aiming->showAimLine) return false;
    float aimDistance = GetAimDistance(entityId);
    return aimDistance > 5.0f;
}

float AimingSystem::GetAimDistance(uint32_t entityId) const {
    auto* world = GetWorld();
    if (!world) return 0.0f;
    
    auto& componentManager = world->GetComponentManager();
    auto* aiming = componentManager.GetComponent<Component::AimingComponent>(entityId);
    auto* transform = componentManager.GetComponent<engine::ECS::Transform2D>(entityId);
    
    if (!aiming || !transform) return 0.0f;
    
    engine::Vector2 entityPos{transform->x, transform->y};
    float dx = aiming->mouseWorldPos.x - entityPos.x;
    float dy = aiming->mouseWorldPos.y - entityPos.y;
    return std::sqrt(dx * dx + dy * dy);
}

} // namespace ZombieSurvivor::System