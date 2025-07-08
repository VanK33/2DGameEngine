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
    if (auto* world = GetWorld()) {
        auto& eventManager = world->GetEventManager();
        eventManager.Subscribe(engine::event::EventType::CUSTOM, this);
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
    if (event->GetType() == engine::event::EventType::CUSTOM) {
        HandleGameEvent(event);
    }
}

void ExperienceSystem::HandleGameEvent(const std::shared_ptr<engine::event::Event>& event) {
    auto gameEvent = std::dynamic_pointer_cast<Events::GameEvent>(event);
    if (!gameEvent) return;

    auto eventType = gameEvent->GetGameEventType();

    switch (eventType) {
        case Events::GameEventType::ENEMY_KILLED:
            HandleEnemyKilled(gameEvent->GetData());
            break;
        case Events::GameEventType::EXPERIENCE_GAINED:
            HandleExperienceGained(gameEvent->GetData());
            break;
        default:
            break;
    }
}

void ExperienceSystem::HandleEnemyKilled(const std::shared_ptr<void>& eventData) {
    if (auto data = std::static_pointer_cast<Events::EnemyKilledData>(eventData)) {
        AddExperience(data->playerId, data->expReward);
    }
}

void ExperienceSystem::HandleExperienceGained(const std::shared_ptr<void>& eventData) {
    if (auto data = std::static_pointer_cast<Events::ExperienceGainedData>(eventData)) {
        AddExperience(data->playerId, data->experienceAmount);
    }
}

void ExperienceSystem::AddExperience(uint32_t entityId, int experience) {
    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();
    auto* exp = componentManager.GetComponent<ZombieSurvivor::Component::ExperienceComponent>(entityId);

    if (!exp) return;

    exp->experience += experience;

    while (exp->experience >= exp->experienceToNext) {
        exp->canLevelUp = true;
        break;
    }
}

void ExperienceSystem::ProcessLevelUp(uint32_t entityId, ZombieSurvivor::Component::ExperienceComponent* exp) {
    if (!exp->canLevelUp) return;

    int oldLevel = exp->level;

    exp->experience -= exp->experienceToNext;
    exp->level++;
    exp->experienceToNext = CalculateExperienceForNextLevel(exp->level);
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