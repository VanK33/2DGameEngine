// src/examples/zombie_survivor/events/GameEventUtils.hpp

// src/examples/zombie_survivor/events/GameEventUtils.hpp

#pragma once

#include "GameEventTypes.hpp"
#include "GameEventData.hpp"
#include "engine/core/event/EventManager.hpp"
#include <memory>

namespace ZombieSurvivor::Events {

class GameEventUtils {
public:
    static void PublishEnemyKilled(engine::event::EventManager& eventManager,
                                   uint32_t playerId, uint32_t enemyId, 
                                   int expReward, const std::string& enemyType) {
        auto data = std::make_shared<EnemyKilledData>();
        data->playerId = playerId;
        data->enemyId = enemyId;
        data->expReward = expReward;
        data->enemyType = enemyType;
        
        auto gameEvent = std::make_shared<GameEvent>(
            GameEventType::ENEMY_KILLED, 
            std::static_pointer_cast<void>(data)
        );
        gameEvent->SetPriority(engine::event::EventPriority::HIGH);
        std::cout << "  [DEBUG] Created GameEvent with type: " << static_cast<int>(gameEvent->GetType()) << std::endl;
        std::cout << "  [DEBUG] Expected CUSTOM type: " << static_cast<int>(engine::event::EventType::CUSTOM) << std::endl;
        std::cout << "  [DEBUG] GameEventType: " << static_cast<int>(gameEvent->GetGameEventType()) << std::endl;
        
        eventManager.Publish(gameEvent);
        std::cout << "  [DEBUG] Event published to EventManager" << std::endl;
    }
    
    static void PublishPlayerLevelUp(engine::event::EventManager& eventManager,
                                     uint32_t playerId, int oldLevel, int newLevel, int totalExp) {
        auto data = std::make_shared<PlayerLevelUpData>();
        data->playerId = playerId;
        data->oldLevel = oldLevel;
        data->newLevel = newLevel;
        data->totalExperience = totalExp;
        
        auto gameEvent = std::make_shared<GameEvent>(
            GameEventType::PLAYER_LEVEL_UP,
            std::static_pointer_cast<void>(data)
        );
        gameEvent->SetPriority(engine::event::EventPriority::HIGH);
        eventManager.Publish(gameEvent);
    }
    
    static void PublishExperienceGained(engine::event::EventManager& eventManager,
                                        uint32_t playerId, int amount, const std::string& source) {
        auto data = std::make_shared<ExperienceGainedData>();
        data->playerId = playerId;
        data->experienceAmount = amount;
        data->source = source;
        
        auto gameEvent = std::make_shared<GameEvent>(
            GameEventType::EXPERIENCE_GAINED,
            std::static_pointer_cast<void>(data)
        );
        eventManager.Publish(gameEvent);
    }
};

} // namespace ZombieSurvivor::Events