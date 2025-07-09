// src/examples/zombie_survivor/ecs/systems/RotationSystem.cpp

#include "RotationSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include <iostream>

namespace ZombieSurvivor::System {

void RotationSystem::Init() {
    std::cout << "[RotationSystem] Initialized" << std::endl;
}

void RotationSystem::Update(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();
    auto entities = componentManager.GetEntitiesWithComponents<
        Component::AimingComponent,
        engine::ECS::Transform2D
    >();

    for (const auto& entityId : entities) {
        auto* aiming = componentManager.GetComponent<Component::AimingComponent>(entityId);
        auto* transform = componentManager.GetComponent<engine::ECS::Transform2D>(entityId);

        if (!aiming || !transform) continue;

        UpdateRotation(entityId, deltaTime);
    }
}

void RotationSystem::Shutdown() {
    rotationSmoothing_.clear();
    std::cout << "[RotationSystem] Shutdown" << std::endl;
}

float RotationSystem::GetCurrentRotation(uint32_t entityId) const {
    auto* world = GetWorld();
    if (!world) return 0.0f;

    auto* transform = world->GetComponentManager().GetComponent<engine::ECS::Transform2D>(entityId);
    return transform ? transform->rotation : 0.0f;
}

void RotationSystem::SetRotationSmoothing(uint32_t entityId, float smoothing) {
    if (smoothing > 0.0f) {
        rotationSmoothing_[entityId] = smoothing;
    } else {
        rotationSmoothing_.erase(entityId);
    }
}

void RotationSystem::UpdateRotation(uint32_t entityId, float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();
    auto* aiming = componentManager.GetComponent<Component::AimingComponent>(entityId);
    auto* transform = componentManager.GetComponent<engine::ECS::Transform2D>(entityId);
    
    if (!aiming || !transform) return;

    float targetRotation = CalculateTargetRotation(aiming->aimDirection);

    auto it = rotationSmoothing_.find(entityId);
    if (it != rotationSmoothing_.end()) {
        transform->rotation = SmoothRotation(transform->rotation, targetRotation, it->second, deltaTime);
    } else {
        transform->rotation = targetRotation;
    }


    transform->rotation = NormalizeAngle(transform->rotation);
}

float RotationSystem::CalculateTargetRotation(const engine::Vector2& aimDirection) const {
    return std::atan2(aimDirection.y, aimDirection.x);
}

float RotationSystem::SmoothRotation(float current, float target, float smoothing, float deltaTime) const {
    float diff = target - current;

    // Picking the minimum rotational route
    if (diff > M_PI) {
        diff -= 2.0f * M_PI;
    } else if (diff < -M_PI) {
        diff += 2.0f * M_PI;
    }

    float rotationSpeed = smoothing * deltaTime;
    if (std::abs(diff) < rotationSpeed) {
        return target;
    }

    return current + (diff > 0 ? rotationSpeed : -rotationSpeed);
}

float RotationSystem::NormalizeAngle(float angle) const {
    while (angle >= 2.0f * M_PI) angle -= 2.0f * M_PI;
    while (angle < 0.0f) angle += 2.0f * M_PI;
    return angle;
}

} // namespace ZombieSurvivor::System