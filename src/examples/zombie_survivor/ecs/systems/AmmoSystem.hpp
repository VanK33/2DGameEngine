// src/examples/zombie_survivor/ecs/systems/AmmoSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/event/EventListener.hpp"
#include "examples/zombie_survivor/events/GameEventTypes.hpp"
#include "examples/zombie_survivor/ecs/components/AmmoComponent.hpp"
#include <string>
#include <unordered_map>

namespace ZombieSurvivor::System {

class AmmoSystem : public engine::ECS::System, public engine::event::EventListener {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "AmmoSystem"; }
    
    void onEvent(const std::shared_ptr<engine::event::Event>& event) override;

    bool HasAmmo(uint32_t entityId) const;
    bool CanConsume(uint32_t entityId, int amount = 1) const;
    
    void ConsumeAmmo(uint32_t entityId, int amount = 1);
    void AddAmmo(uint32_t entityId, int amount);
    
    int GetCurrentAmmo(uint32_t entityId) const;
    int GetTotalAmmo(uint32_t entityId) const;
    int GetMaxAmmo(uint32_t entityId) const;
    
    void InitializeAmmoFromWeapon(uint32_t entityId);
    void SetAmmo(uint32_t entityId, int currentAmmo, int totalAmmo);
    void SetMaxAmmo(uint32_t entityId, int maxAmmo);

    void ExecuteReload(uint32_t entityId, int reloadAmount, int magazineCapacity);

private:
    // 换弹状态跟踪
    struct ReloadState {
        float reloadTime = 0.0f;
        float elapsedTime = 0.0f;
        bool isActive = false;
    };
    std::unordered_map<uint32_t, ReloadState> reloadStates_;
    
    void UpdateReloadProgress(float deltaTime);

    void HandleGameEvent(const std::shared_ptr<engine::event::Event>& event);
    void HandleAmmoConsumeRequest(const std::shared_ptr<void>& eventData);
    void HandleAmmoConsumed(const std::shared_ptr<void>& eventData);
    void HandleReloadCompleted(const std::shared_ptr<void>& eventData);
    void HandleWeaponFireRequested(const std::shared_ptr<void>& eventData);
    void HandleReloadStarted(const std::shared_ptr<void>& eventData);

    void HandleReloadExecute(const std::shared_ptr<void>& eventData);
    void HandleWeaponInitialized(const std::shared_ptr<void>& eventData);
    
    bool ValidateEntityId(uint32_t entityId, const std::string& operation) const;
    bool ValidateAmmoAmount(int amount, const std::string& operation) const;
    
    Component::AmmoComponent* GetAmmoComponent(uint32_t entityId) const;
    
    void PublishAmmoDepletedEvent(uint32_t entityId);
    void PublishAmmoChangedEvent(uint32_t entityId, int oldCurrent, int oldTotal, int newCurrent, int newTotal);
    void PublishWeaponFiredEvent(uint32_t entityId);

    void PublishReloadExecutedEvent(uint32_t entityId, int actualReload, int newCurrent, int newTotal);
};

} // namespace ZombieSurvivor::System