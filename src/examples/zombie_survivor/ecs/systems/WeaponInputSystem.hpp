// src/examples/zombie_survivor/ecs/systems/WeaponInputSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "examples/zombie_survivor/ecs/components/InputComponent.hpp"
#include "examples/zombie_survivor/ecs/components/WeaponComponent.hpp"

namespace ZombieSurvivor::System {

class WeaponInputSystem : public engine::ECS::System {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "WeaponInputSystem"; }

private:
    void ProcessShootInput(uint32_t playerId, bool shootPressed);
    void ProcessReloadInput(uint32_t playerId, bool reloadPressed);
    void ProcessWeaponSwitch(uint32_t playerId, int weaponSlot);
    
    void PublishFireInput(uint32_t playerId);
    void PublishReloadInput(uint32_t playerId);
    void PublishWeaponSwitchInput(uint32_t playerId, int weaponSlot);
};

} // namespace ZombieSurvivor::System