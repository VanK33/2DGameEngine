// src/examples/zombie_survivor/ecs/systems/HealthSystem.hpp
#pragma once

#include "engine/core/ecs/System.hpp"
#include <memory>

namespace ZombieSurvivor::System {

class HealthSystem : public engine::ECS::System {
public:
    void Update(float deltaTime) override;
    const char* GetName() const override { return "HealthSystem"; }

    void ModifyHealth(uint32_t entityId, float amount);
    void SetMaxHealth(uint32_t entityId, float maxHealth);
    void RestoreToFull(uint32_t entityId);

    bool IsEntityAlive(uint32_t entityId) const;
    float GetCurrentHealth(uint32_t entityId) const;
    float GetMaxHealth(uint32_t entityId) const;
    float GetHealthPercentage(uint32_t entityId) const;

private:
    void ProcessDeathChecks();
    void ProcessDeath(uint32_t entityId);
    void PublishDeathEvent(uint32_t entityId);
    void PublishHealthChangedEvent(uint32_t entityId, float oldHealth, float newHealth);
};

} // namespace ZombieSurvivor::System