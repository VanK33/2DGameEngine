// src/examples/zombie_survivor/ecs/systems/RotationSystem.hpp
#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "examples/zombie_survivor/ecs/components/AimingComponent.hpp"
#include <unordered_map>
#include <cmath>

namespace ZombieSurvivor::System {

class RotationSystem : public engine::ECS::System {
public:

void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "RotationSystem"; }

    float GetCurrentRotation(uint32_t entityId) const;
    void SetRotationSmoothing(uint32_t entityId, float smoothing);

private:
    void UpdateRotation(uint32_t entityId, float deltaTime);
    float CalculateTargetRotation(const engine::Vector2& aimDirection) const;
    float SmoothRotation(float current, float target, float smoothing, float deltaTime) const;
    float NormalizeAngle(float angle) const;

    std::unordered_map<uint32_t, float> rotationSmoothing_;
};

} // namespace ZombieSurvivor::System