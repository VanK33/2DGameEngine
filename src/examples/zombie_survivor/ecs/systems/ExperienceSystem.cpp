// src/examples/zombie_survivor/systems/ExperienceSystem.cpp

#include "ExperienceSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "examples/zombie_survivor/ecs/components/ExperienceComponent.hpp"
#include "examples/zombie_survivor/events/GameEventTypes.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include "examples/zombie_survivor/events/GameEventUtils.hpp"

namespace ZombieSurvivor::System {

void ExperienceSystem::Init() {
    std::cout << "[DEBUG] ExperienceSystem::Init() called" << std::endl;
    if (auto* world = GetWorld()) {
        std::cout << "[DEBUG] World found: " << world << std::endl;
        auto& eventManager = world->GetEventManager();
        std::cout << "[DEBUG] EventManager: " << &eventManager << std::endl;
        eventManager.Subscribe(engine::event::EventType::CUSTOM, this);
        std::cout << "[DEBUG] ExperienceSystem subscribed to CUSTOM events" << std::endl;
    } else {
        std::cout << "[DEBUG] ERROR: No world found in ExperienceSystem::Init()" << std::endl;
    }
}

void ExperienceSystem::Update(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();
    auto entities = componentManager.GetEntitiesWithComponent<ZombieSurvivor::Component::ExperienceComponent>();

    for (auto entityId : entities) {
        auto* exp = componentManager.GetComponent<ZombieSurvivor::Component::ExperienceComponent>(entityId);
        if (!exp || !exp->canLevelUp) continue;

        ProcessLevelUp(entityId, exp);
    }
}

void ExperienceSystem::onEvent(const std::shared_ptr<engine::event::Event>& event) {
    std::cout << "  [DEBUG] ExperienceSystem received event, type: " << static_cast<int>(event->GetType()) << std::endl;
    std::cout << "  [DEBUG] Expected CUSTOM type: " << static_cast<int>(engine::event::EventType::CUSTOM) << std::endl;
    
    if (event->GetType() == engine::event::EventType::CUSTOM) {
        std::cout << "  [DEBUG] Processing CUSTOM event" << std::endl;
        HandleGameEvent(event);
    } else {
        std::cout << "  [DEBUG] Event type mismatch!" << std::endl;
    }
}


void ExperienceSystem::HandleGameEvent(const std::shared_ptr<engine::event::Event>& event) {
    auto gameEvent = std::dynamic_pointer_cast<Events::GameEvent>(event);
    if (!gameEvent) {
        std::cout << "  [DEBUG] Failed to cast to GameEvent" << std::endl;
        return;
    }

    auto eventType = gameEvent->GetGameEventType();
    std::cout << "  [DEBUG] GameEvent type: " << static_cast<int>(eventType) << std::endl;

    switch (eventType) {
        case Events::GameEventType::ENEMY_KILLED:
            std::cout << "  [DEBUG] Handling ENEMY_KILLED event" << std::endl;
            HandleEnemyKilled(gameEvent->GetData());
            break;
        case Events::GameEventType::EXPERIENCE_GAINED:
            std::cout << "  [DEBUG] Handling EXPERIENCE_GAINED event" << std::endl;
            HandleExperienceGained(gameEvent->GetData());
            break;
        default:
            std::cout << "  [DEBUG] Unhandled event type: " << static_cast<int>(eventType) << std::endl;
            break;
    }
}

void ExperienceSystem::HandleEnemyKilled(const std::shared_ptr<void>& eventData) {
    if (auto data = std::static_pointer_cast<Events::EnemyKilledData>(eventData)) {
        std::cout << "  [DEBUG] Enemy killed - Player: " << data->playerId 
                  << ", ExpReward: " << data->expReward << std::endl;
        AddExperience(data->playerId, data->expReward);
    } else {
        std::cout << "  [DEBUG] Failed to cast EnemyKilledData" << std::endl;
    }
}


void ExperienceSystem::HandleExperienceGained(const std::shared_ptr<void>& eventData) {
    if (auto data = std::static_pointer_cast<Events::ExperienceGainedData>(eventData)) {
        AddExperience(data->playerId, data->experienceAmount);
    }
}

void ExperienceSystem::AddExperience(uint32_t entityId, int experience) {
    std::cout << "  [DEBUG] Adding " << experience << " experience to entity " << entityId << std::endl;
    
    auto* world = GetWorld();
    if (!world) {
        std::cout << "  [DEBUG] No world available" << std::endl;
        return;
    }

    auto& componentManager = world->GetComponentManager();
    auto* exp = componentManager.GetComponent<ZombieSurvivor::Component::ExperienceComponent>(entityId);

    if (!exp) {
        std::cout << "  [DEBUG] No ExperienceComponent found for entity " << entityId << std::endl;
        return;
    }

    std::cout << "  [DEBUG] Before: Level=" << exp->level 
              << ", Exp=" << exp->experience 
              << ", ToNext=" << exp->experienceToNext << std::endl;

    exp->experience += experience;

    std::cout << "  [DEBUG] After: Exp=" << exp->experience << std::endl;

    if (exp->experience >= exp->experienceToNext) {
        exp->canLevelUp = true;
        std::cout << "  [DEBUG] Can level up!" << std::endl;
    }
}

void ExperienceSystem::ProcessLevelUp(uint32_t entityId, ZombieSurvivor::Component::ExperienceComponent* exp) {
    if (!exp->canLevelUp) return;

    int oldLevel = exp->level;

    int overflowExp = exp->experience - exp->experienceToNext;
    exp->level++;
    exp->experienceToNext = CalculateExperienceForNextLevel(exp->level);
    exp->experience = overflowExp;
    exp->canLevelUp = false;

    PublishLevelUpEvent(entityId, oldLevel, exp->level);

    if (exp->experience >= exp->experienceToNext) {
        exp->canLevelUp = true;
    }
}

int ExperienceSystem::CalculateExperienceForNextLevel(int level) const {
    // temporary calculation
    return 100 + (level - 1) * 50;
}

void ExperienceSystem::PublishLevelUpEvent(uint32_t entityId, int oldLevel, int newLevel) {
    if (auto* world = GetWorld()) {
        auto& eventManager = world->GetEventManager();
        
        int totalExp = 0;
        auto& componentManager = world->GetComponentManager();
        if (auto* exp = componentManager.GetComponent<ZombieSurvivor::Component::ExperienceComponent>(entityId)) {
            totalExp = exp->experience;
        }

        Events::GameEventUtils::PublishPlayerLevelUp(
            eventManager, entityId, oldLevel, newLevel, totalExp
        );
    }
}

void ExperienceSystem::Shutdown() {
    if (auto* world = GetWorld()) {
        auto& eventManager = world->GetEventManager();
        eventManager.Unsubscribe(engine::event::EventType::CUSTOM, this);
    }
}

} // namesapce ZombieSurvivor:: System