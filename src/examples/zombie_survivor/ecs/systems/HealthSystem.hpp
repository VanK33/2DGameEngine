// src/examples/zombie_survivor/ecs/systems/HealthSystem.hpp
#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/event/EventListener.hpp"
#include "engine/core/event/Event.hpp"
#include <memory>

namespace ZombieSurvivor::System {

class HealthSystem : public engine::ECS::System, public engine::event::EventListener {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "HealthSystem"; }

    void onEvent(const std::shared_ptr<engine::event::Event>& event) override;

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
    
    void HandleGameEvent(const std::shared_ptr<engine::event::Event>& event);
    void HandleDamageEvent(const std::shared_ptr<void>& eventData);
};

} // namespace ZombieSurvivor::System