// src/examples/zombie_survivor/ecs/systems/WeaponSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/event/EventListener.hpp"
#include "examples/zombie_survivor/events/GameEventTypes.hpp"
#include "examples/zombie_survivor/ecs/components/WeaponComponent.hpp"
#include "examples/zombie_survivor/ecs/components/AmmoComponent.hpp"
#include "examples/zombie_survivor/ecs/components/InputComponent.hpp"
#include <unordered_map>
#include <string>

namespace ZombieSurvivor::System {

struct PairHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        std::size_t h1 = std::hash<T1>()(pair.first);
        std::size_t h2 = std::hash<T2>()(pair.second);
        return h1 ^ (h2 << 1);
    }
};

class WeaponSystem : public engine::ECS::System, public engine::event::EventListener {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "WeaponSystem"; }
    
    void onEvent(const std::shared_ptr<engine::event::Event>& event) override;

    bool CanShoot(uint32_t entityId) const;
    void TryShoot(uint32_t entityId);
    bool IsWeaponReady(uint32_t entityId) const;
    
    void StartReload(uint32_t entityId);
    bool IsReloading(uint32_t entityId) const;
    float GetReloadProgress(uint32_t entityId) const;
    
    float GetFireCooldown(uint32_t entityId) const;
    
    void SetCurrentWeapon(uint32_t entityId, uint32_t weaponId);

private:
    struct WeaponState {
        float timeSinceLastShot = 0.0f;
        float reloadTimer = 0.0f;
        bool isReloading = false;
        uint32_t weaponId = 0;
    };
    
    std::unordered_map<std::pair<uint32_t, uint32_t>, WeaponState, PairHash> weaponStates_;
    
    std::unordered_map<uint32_t, uint32_t> currentWeapons_;
    
    std::unordered_map<uint32_t, uint32_t> reloadingWeapons_;
    
    void HandleGameEvent(const std::shared_ptr<engine::event::Event>& event);
    void HandleReloadRequested(const std::shared_ptr<void>& eventData);
    void HandleAmmoConsumed(const std::shared_ptr<void>& eventData);
    void HandleReloadExecuted(const std::shared_ptr<void>& eventData);
    
    void ExecuteWeaponFire(uint32_t entityId, int currentAmmo, int totalAmmo);
    
    void UpdateFireCooldowns(float deltaTime);
    void UpdateReloading(float deltaTime);
    void CompleteReload(uint32_t entityId, uint32_t weaponId);
    
    void ProcessShootInput(uint32_t entityId, bool shootPressed);
    void ProcessReloadInput(uint32_t entityId, bool reloadPressed);
    
    bool ValidateEntityId(uint32_t entityId, const std::string& operation) const;
    Component::WeaponComponent* GetWeaponComponent(uint32_t entityId) const;
    Component::AmmoComponent* GetAmmoComponent(uint32_t entityId) const;
    
    void PublishWeaponFiredEvent(uint32_t entityId, float damage, int currentAmmo, int totalAmmo);
    void PublishReloadStartedEvent(uint32_t entityId, float reloadTime, int magazineCapacity, int totalAmmo);
    void PublishReloadCompletedEvent(uint32_t entityId, int newAmmo, int totalAmmo);
    
    WeaponState& GetOrCreateWeaponState(const std::pair<uint32_t, uint32_t>& stateKey);
    
    uint32_t GetCurrentWeaponId(uint32_t entityId) const;
    std::pair<uint32_t, uint32_t> GetWeaponStateKey(uint32_t entityId, uint32_t weaponId) const;
    void StopReload(uint32_t entityId);
};

} // namespace ZombieSurvivor::System 