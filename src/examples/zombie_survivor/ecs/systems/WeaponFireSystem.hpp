// src/examples/zombie_survivor/ecs/systems/WeaponFireSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/event/EventListener.hpp"
#include "examples/zombie_survivor/events/GameEventTypes.hpp"
#include "examples/zombie_survivor/ecs/components/WeaponComponent.hpp"
#include "examples/zombie_survivor/ecs/components/AmmoComponent.hpp"

namespace ZombieSurvivor::System {

class WeaponFireSystem : public engine::ECS::System, public engine::event::EventListener {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "WeaponFireSystem"; }
    
    void onEvent(const std::shared_ptr<engine::event::Event>& event) override;

private:
    void HandleGameEvent(const std::shared_ptr<engine::event::Event>& event);
    void HandleFireInput(uint32_t playerId);
    void HandleAmmoConsumed(const std::shared_ptr<void>& eventData);
    void CreateProjectile(uint32_t playerId, Component::AmmoType ammoType); 
    
    bool CanFire(uint32_t playerId) const;
    Component::AmmoType GetWeaponAmmoType(uint32_t playerId) const;
    
    void PublishAmmoConsumeRequest(uint32_t playerId, Component::AmmoType ammoType);
};

} // namespace ZombieSurvivor::System