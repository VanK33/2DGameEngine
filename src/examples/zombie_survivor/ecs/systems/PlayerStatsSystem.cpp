// src/examples/zombie_survivor/ecs/systems/PlayerStatsSystem.cpp

#include "PlayerStatsSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include "examples/zombie_survivor/ecs/components/ExperienceComponent.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include "engine/core/ecs/ComponentManager.hpp"
#include <iostream>

namespace ZombieSurvivor::System {

void PlayerStatsSystem::Init() {
    std::cout << "[PlayerStatsSystem] Initializing..." << std::endl;
    
    auto* world = GetWorld();
    if (world) {
        auto& eventManager = world->GetEventManager();
        eventManager.Subscribe(engine::event::EventType::CUSTOM, this);
        std::cout << "[PlayerStatsSystem] Subscribed to events" << std::endl;
    }
    
    std::cout << "[PlayerStatsSystem] ✅ Initialized successfully" << std::endl;
}

void PlayerStatsSystem::Update(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;
    
    // 更新所有玩家的生存时间
    UpdateSurvivalTime(deltaTime);
}

void PlayerStatsSystem::Shutdown() {
    auto* world = GetWorld();
    if (world) {
        auto& eventManager = world->GetEventManager();
        eventManager.Unsubscribe(engine::event::EventType::CUSTOM, this);
        std::cout << "[PlayerStatsSystem] Unsubscribed from events" << std::endl;
    }
    
    std::cout << "[PlayerStatsSystem] 🧹 Shutdown complete" << std::endl;
}

void PlayerStatsSystem::onEvent(const std::shared_ptr<engine::event::Event>& event) {
    if (!event) return;
    
    if (event->GetType() == engine::event::EventType::CUSTOM) {
        HandleGameEvent(event);
    }
}

void PlayerStatsSystem::HandleGameEvent(const std::shared_ptr<engine::event::Event>& event) {
    auto gameEvent = std::dynamic_pointer_cast<Events::GameEvent>(event);
    if (!gameEvent) return;
    
    switch (gameEvent->GetGameEventType()) {
        case Events::GameEventType::ENEMY_KILLED:
            HandleEnemyKilled(gameEvent->GetData());
            break;
            
        case Events::GameEventType::PLAYER_LEVEL_UP:
            HandlePlayerLevelUp(gameEvent->GetData());
            break;
            
        case Events::GameEventType::EXPERIENCE_GAINED:
            HandleExperienceGained(gameEvent->GetData());
            break;
            
        case Events::GameEventType::DAMAGE_DEALT:
            HandleDamageDealt(gameEvent->GetData());
            break;
            
        case Events::GameEventType::DAMAGE_TAKEN:
            HandleDamageTaken(gameEvent->GetData());
            break;
            
        default:
            break;
    }
}

void PlayerStatsSystem::HandleEnemyKilled(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::EnemyKilledData>(eventData);
    if (!data) return;
    
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* stats = componentManager.GetComponent<Component::PlayerStatsComponent>(data->playerId);
    
    if (stats) {
        stats->killCount++;
        PrintStatsUpdate(data->playerId, "Kill Count", std::to_string(stats->killCount));
    }
}

void PlayerStatsSystem::HandlePlayerLevelUp(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::PlayerLevelUpData>(eventData);
    if (!data) return;
    
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* stats = componentManager.GetComponent<Component::PlayerStatsComponent>(data->playerId);
    
    if (stats) {
        if (data->newLevel > stats->highestLevel) {
            stats->highestLevel = data->newLevel;
            PrintStatsUpdate(data->playerId, "Highest Level", std::to_string(stats->highestLevel));
        }
    }
}

void PlayerStatsSystem::HandleExperienceGained(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::ExperienceGainedData>(eventData);
    if (!data) return;
    
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* stats = componentManager.GetComponent<Component::PlayerStatsComponent>(data->playerId);
    
    if (stats) {
        stats->totalExperienceGained += data->experienceAmount;
        PrintStatsUpdate(data->playerId, "Total Experience", std::to_string(stats->totalExperienceGained));
    }
}

void PlayerStatsSystem::HandleDamageDealt(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::DamageData>(eventData);  // 改用DamageData
    if (!data) return;
    
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* stats = componentManager.GetComponent<Component::PlayerStatsComponent>(data->sourceEntityId);  // 改用sourceEntityId
    
    if (stats) {
        stats->totalDamageDealt += data->damageAmount;
        PrintStatsUpdate(data->sourceEntityId, "Total Damage Dealt", 
                        std::to_string(static_cast<int>(stats->totalDamageDealt)));
    }
}

void PlayerStatsSystem::HandleDamageTaken(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::DamageData>(eventData);  // 改用DamageData
    if (!data) return;
    
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* stats = componentManager.GetComponent<Component::PlayerStatsComponent>(data->targetEntityId);  // 改用targetEntityId
    
    if (stats) {
        stats->totalDamageTaken += data->damageAmount;
        PrintStatsUpdate(data->targetEntityId, "Total Damage Taken", 
                        std::to_string(static_cast<int>(stats->totalDamageTaken)));
    }
}

void PlayerStatsSystem::UpdateSurvivalTime(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    
    // 遍历所有有PlayerStatsComponent的实体
    auto entities = componentManager.GetEntitiesWithComponent<Component::PlayerStatsComponent>();
    for (auto entityId : entities) {
        auto* stats = componentManager.GetComponent<Component::PlayerStatsComponent>(entityId);
        if (stats) {
            stats->survivalTime += deltaTime;
        }
    }
}

Component::PlayerStatsComponent* PlayerStatsSystem::GetPlayerStats(uint32_t playerId) {
    auto* world = GetWorld();
    if (!world) return nullptr;
    
    auto& componentManager = world->GetComponentManager();
    return componentManager.GetComponent<Component::PlayerStatsComponent>(playerId);
}

void PlayerStatsSystem::ResetStats(uint32_t playerId) {
    auto* stats = GetPlayerStats(playerId);
    if (stats) {
        stats->killCount = 0;
        stats->survivalTime = 0.0f;
        stats->totalExperienceGained = 0;
        stats->highestLevel = 1;
        stats->totalDamageTaken = 0.0f;
        stats->totalDamageDealt = 0.0f;
        
        std::cout << "[PlayerStatsSystem] 🔄 Stats reset for player " << playerId << std::endl;
    }
}

void PlayerStatsSystem::RecordKill(uint32_t playerId) {
    auto* stats = GetPlayerStats(playerId);
    if (stats) {
        stats->killCount++;
        PrintStatsUpdate(playerId, "Kill Count (Manual)", std::to_string(stats->killCount));
    }
}

void PlayerStatsSystem::RecordDamage(uint32_t playerId, float damageDealt, float damageTaken) {
    auto* stats = GetPlayerStats(playerId);
    if (stats) {
        stats->totalDamageDealt += damageDealt;
        stats->totalDamageTaken += damageTaken;
        
        if (damageDealt > 0) {
            PrintStatsUpdate(playerId, "Damage Dealt", std::to_string(static_cast<int>(stats->totalDamageDealt)));
        }
        if (damageTaken > 0) {
            PrintStatsUpdate(playerId, "Damage Taken", std::to_string(static_cast<int>(stats->totalDamageTaken)));
        }
    }
}

void PlayerStatsSystem::PrintStatsUpdate(uint32_t playerId, const std::string& statName, const std::string& value) {
    std::cout << "[PlayerStatsSystem] 📊 Player " << playerId << " - " << statName << ": " << value << std::endl;
}

} // namespace ZombieSurvivor::System
