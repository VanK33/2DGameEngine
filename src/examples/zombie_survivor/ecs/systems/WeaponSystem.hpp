// src/examples/zombie_survivor/ecs/systems/WeaponSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/event/EventListener.hpp"
#include "examples/zombie_survivor/events/GameEventTypes.hpp"
#include <unordered_map>

namespace ZombieSurvivor::System {

class WeaponSystem : public engine::ECS::System, public engine::event::EventListener {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "WeaponSystem"; }
    
    void onEvent(const std::shared_ptr<engine::event::Event>& event) override;
    
    // Query methods for other systems
    bool IsWeaponReady(uint32_t playerId) const;
    bool IsReloading(uint32_t playerId) const;
    float GetCooldownProgress(uint32_t playerId) const;
    float GetReloadProgress(uint32_t playerId) const;

private:
    struct WeaponState {
        float cooldown = 0.0f;
        float reloadTimer = 0.0f;
        bool isReloading = false;
        
        // Current weapon stats (updated when weapon switches)
        float fireRate = 2.0f;
        float reloadTime = 1.5f;
    };
    
    std::unordered_map<uint32_t, WeaponState> playerWeaponStates_;
    
    void UpdateWeaponCooldowns(float deltaTime);
    void UpdateWeaponReloads(float deltaTime);
    
    void HandleGameEvent(const std::shared_ptr<engine::event::Event>& event);
    void OnWeaponFired(uint32_t playerId);
    void OnReloadStarted(uint32_t playerId);
    void OnReloadCompleted(uint32_t playerId);
    void OnWeaponSwitched(uint32_t playerId, float fireRate, float reloadTime);
    
    WeaponState& GetOrCreateWeaponState(uint32_t playerId);
    
    float GetWeaponFireRate(uint32_t playerId) const;
    float GetWeaponReloadTime(uint32_t playerId) const;
};

} // namespace ZombieSurvivor::System
