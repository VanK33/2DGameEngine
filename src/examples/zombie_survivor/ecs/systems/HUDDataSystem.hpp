// src/examples/zombie_survivor/ecs/systems/HUDDataSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/event/EventListener.hpp"
#include "engine/core/event/Event.hpp"
#include "examples/zombie_survivor/ecs/components/HUDComponent.hpp"
#include "examples/zombie_survivor/ecs/UIFactory.hpp"
#include <memory>

namespace ZombieSurvivor::System {

class HUDDataSystem : public engine::ECS::System, public engine::event::EventListener {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "HUDDataSystem"; }
    
    void onEvent(const std::shared_ptr<engine::event::Event>& event) override;
    
    // HUD element creation methods (using UIFactory)
    uint32_t CreateHealthHUD(uint32_t targetPlayerId, Component::HUDPosition position = Component::HUDPosition::TOP_LEFT);
    uint32_t CreateAmmoHUD(uint32_t targetPlayerId, Component::HUDPosition position = Component::HUDPosition::BOTTOM_RIGHT);
    uint32_t CreateExperienceHUD(uint32_t targetPlayerId, Component::HUDPosition position = Component::HUDPosition::BOTTOM_CENTER);
    uint32_t CreateKillCounterHUD(uint32_t targetPlayerId, Component::HUDPosition position = Component::HUDPosition::TOP_RIGHT);
    uint32_t CreateSurvivalTimerHUD(Component::HUDPosition position = Component::HUDPosition::TOP_CENTER);
    uint32_t CreateCrosshairHUD(Component::HUDPosition position = Component::HUDPosition::CENTER);
    
    // Data update methods
    void UpdateHealthData(uint32_t hudEntityId, uint32_t playerId);
    void UpdateAmmoData(uint32_t hudEntityId, uint32_t playerId);
    void UpdateExperienceData(uint32_t hudEntityId, uint32_t playerId);
    void UpdateKillCounterData(uint32_t hudEntityId, uint32_t playerId);
    void UpdateSurvivalTimerData(uint32_t hudEntityId);
    
private:
    // UIFactory for creating HUD elements
    std::unique_ptr<ZombieSurvivor::ECS::UIFactory> uiFactory_;
    
    // Game start time for survival timer
    float gameStartTime_ = 0.0f;
    
    // Helper methods for data retrieval
    float GetHealthPercentage(uint32_t playerId) const;
    int GetCurrentAmmo(uint32_t playerId) const;
    int GetMaxAmmo(uint32_t playerId) const;
    float GetExperiencePercentage(uint32_t playerId) const;
    int GetKillCount(uint32_t playerId) const;
    float GetCurrentTime() const;
    
    // Animation helpers
    void AnimateValueChange(Component::HUDComponent* hud, float targetValue, float deltaTime);
    
    // Event handling
    void HandleGameEvent(const std::shared_ptr<engine::event::Event>& event);
    void OnHealthChanged(uint32_t playerId);
    void OnAmmoChanged(uint32_t playerId);
    void OnExperienceChanged(uint32_t playerId);
    void OnEnemyKilled(uint32_t playerId);
    
    // Utility methods
    std::string FormatTime(float timeInSeconds) const;
    std::string FormatAmmoText(int current, int max) const;
    
    // Validation
    bool ValidatePlayerId(uint32_t playerId, const std::string& operation) const;
    Component::HUDComponent* GetHUDComponent(uint32_t hudEntityId) const;
};

} // namespace ZombieSurvivor::System