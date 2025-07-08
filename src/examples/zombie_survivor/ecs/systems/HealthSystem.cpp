// src/examples/zombie_survivor/ecs/systems/HealthSystem.cpp

#include "HealthSystem.hpp"
#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "examples/zombie_survivor/ecs/components/HealthComponent.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include "examples/zombie_survivor/events/GameEventTypes.hpp"

namespace ZombieSurvivor::System {

void HealthSystem::Update(float deltaTime) {
    ProcessDeathChecks();
}

void HealthSystem::ModifyHealth(uint32_t entityId, float amount) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* health = componentManager.GetComponent<ZombieSurvivor::Component::HealthComponent>(entityId);
    
    if (!health || !health->isAlive) return;
    
    float oldHealth = health->health;
    health->health += amount;
    
    if (health->health > health->maxHealth) {
        health->health = health->maxHealth;
    } else if (health->health <= 0.0f) {
        health->health = 0.0f;
    }
    
    if (oldHealth != health->health) {
        PublishHealthChangedEvent(entityId, oldHealth, health->health);
    }
}

void HealthSystem::SetMaxHealth(uint32_t entityId, float maxHealth) {
    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();
    auto* health = componentManager.GetComponent<ZombieSurvivor::Component::HealthComponent>(entityId);

    if (!health) return;

    float oldHealth = health->health;
    health->maxHealth = maxHealth;

    if (health->health > health->maxHealth) {
        health->health = health->maxHealth;
        PublishHealthChangedEvent(entityId, oldHealth, health->health);
    }
}

void HealthSystem::RestoreToFull(uint32_t entityId) {
    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();
    auto* health = componentManager.GetComponent<ZombieSurvivor::Component::HealthComponent>(entityId);
    
    if (!health) return;

    if (health->health < health->maxHealth) {
        float oldHealth = health->health;
        health->health = health->maxHealth;
        PublishHealthChangedEvent(entityId, oldHealth, health->health);
    }
}

void HealthSystem::ProcessDeathChecks() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto entities = componentManager.GetEntitiesWithComponent<ZombieSurvivor::Component::HealthComponent>();
    
    for (auto entityId : entities) {
        auto* health = componentManager.GetComponent<ZombieSurvivor::Component::HealthComponent>(entityId);
        if (!health) continue;
        
        if (health->health <= 0.0f && health->isAlive) {
            ProcessDeath(entityId);
        }
    }
}

void HealthSystem::ProcessDeath(uint32_t entityId) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* health = componentManager.GetComponent<ZombieSurvivor::Component::HealthComponent>(entityId);
    
    if (!health) return;
    
    health->isAlive = false;
    
    PublishDeathEvent(entityId);
}

void HealthSystem::PublishHealthChangedEvent(uint32_t entityId, float oldHealth, float newHealth) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    /* Example codes*/
    auto healthData = std::make_shared<Events::HealthChangedData>();
    healthData->entityId = entityId;
    healthData->oldHealth = oldHealth;
    healthData->newHealth = newHealth;
    healthData->healthPercentage = newHealth / GetMaxHealth(entityId);
    
    auto gameEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::HEALTH_CHANGED,
        std::static_pointer_cast<void>(healthData)
    );
    eventManager.Publish(gameEvent);
}

void HealthSystem::PublishDeathEvent(uint32_t entityId) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    /* Example codes*/
    auto deathData = std::make_shared<Events::EntityDiedData>();
    deathData->entityId = entityId;
    deathData->deathCause = "health_depleted";
    deathData->positionX = 0.0f;
    deathData->positionY = 0.0f;
    deathData->survivalTime = 0.0f;
    deathData->killerEntityId = 0;
    
    auto gameEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::ENTITY_DIED,
        std::static_pointer_cast<void>(deathData)
    );
    gameEvent->SetPriority(engine::event::EventPriority::HIGH);
    eventManager.Publish(gameEvent);
}

bool HealthSystem::IsEntityAlive(uint32_t entityId) const {
    auto* world = GetWorld();
    if (!world) return false;
    
    auto& componentManager = world->GetComponentManager();
    auto* health = componentManager.GetComponent<ZombieSurvivor::Component::HealthComponent>(entityId);
    return health ? health->isAlive : false;
}

float HealthSystem::GetCurrentHealth(uint32_t entityId) const {
    auto* world = GetWorld();
    if (!world) return 0.0f;
}

float HealthSystem::GetMaxHealth(uint32_t entityId) const {
    auto* world = GetWorld();
    if (!world) return 0.0f;

    auto& componentManager = world->GetComponentManager();
    auto* health = componentManager.GetComponent<ZombieSurvivor::Component::HealthComponent>(entityId);
    return health ? health->maxHealth : 0.0f;
}

float HealthSystem::GetHealthPercentage(uint32_t entityId) const {
    auto* world = GetWorld();
    if (!world) return 0.0f;
    
    auto& componentManager = world->GetComponentManager();
    auto* health = componentManager.GetComponent<ZombieSurvivor::Component::HealthComponent>(entityId);
    
    if (!health || health->maxHealth <= 0.0f) return 0.0f;
    
    return health->health / health->maxHealth;
}



} // namespace ZombieSurvivor::System
