// src/examples/zombie_survivor/ecs/systems/AimingSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "examples/zombie_survivor/ecs/components/AimingComponent.hpp"
#include "examples/zombie_survivor/ecs/components/InputComponent.hpp"
#include <cmath>

namespace ZombieSurvivor::System {

class AimingSystem : public engine::ECS::System {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "AimingSystem"; }

    engine::Vector2 CalculateAimDirection(const engine::Vector2& fromPos, const engine::Vector2& toPos) const;
    void UpdateMouseWorldPos(uint32_t entityId, const engine::Vector2& mouseScreenPos);
    
    engine::Vector2 GetAimDirection(uint32_t entityId) const;
    engine::Vector2 GetMouseWorldPos(uint32_t entityId) const;
    
    bool ShouldShowAimLine(uint32_t entityId) const;
    float GetAimDistance(uint32_t entityId) const;

private:
    void UpdateAiming(uint32_t entityId);
};

} // namespace ZombieSurvivor::System