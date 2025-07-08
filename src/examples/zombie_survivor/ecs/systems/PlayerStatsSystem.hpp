// src/examples/zombie_survivor/ecs/systems/PlayerStatsSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/event/EventListener.hpp"
#include "examples/zombie_survivor/events/GameEventTypes.hpp"
#include "examples/zombie_survivor/ecs/components/PlayerStatsComponent.hpp"

namespace ZombieSurvivor::System {

class PlayerStatsSystem : public engine::ECS::System, public engine::event::EventListener {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "PlayerStatsSystem"; }

    void onEvent(const std::shared_ptr<engine::event::Event>& event) override;

    Component::PlayerStatsComponent* GetPlayerStats(uint32_t playerId);
    void ResetStats(uint32_t playerId);
    
    void RecordKill(uint32_t playerId);
    void RecordDamage(uint32_t playerId, float damageDealt, float damageTaken);

private:
    void HandleGameEvent(const std::shared_ptr<engine::event::Event>& event);
    void HandleEnemyKilled(const std::shared_ptr<void>& eventData);
    void HandlePlayerLevelUp(const std::shared_ptr<void>& eventData);
    void HandleExperienceGained(const std::shared_ptr<void>& eventData);
    void HandleDamageDealt(const std::shared_ptr<void>& eventData);
    void HandleDamageTaken(const std::shared_ptr<void>& eventData);
    
    void UpdateSurvivalTime(float deltaTime);
    void PrintStatsUpdate(uint32_t playerId, const std::string& statName, const std::string& value);
};

} // namespace ZombieSurvivor::System
