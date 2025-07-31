// src/examples/zombie_survivor/ecs/systems/HealthSystem.cpp

#include "HealthSystem.hpp"
#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "examples/zombie_survivor/ecs/components/HealthComponent.hpp"
#include "examples/zombie_survivor/ecs/components/EnemyComponent.hpp"
#include "examples/zombie_survivor/ecs/components/CombatStatsComponent.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include "examples/zombie_survivor/events/GameEventTypes.hpp"
#include <iostream>

namespace ZombieSurvivor::System {

void HealthSystem::Init() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    // 订阅伤害事件
    eventManager.Subscribe(engine::event::EventType::CUSTOM, this);
    
    std::cout << "[HealthSystem] Initialized and subscribed to damage events" << std::endl;
}

void HealthSystem::Update(float deltaTime) {
    ProcessDeathChecks();
}

void HealthSystem::Shutdown() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    // 取消订阅事件
    eventManager.Unsubscribe(engine::event::EventType::CUSTOM, this);
    
    std::cout << "[HealthSystem] Shutdown and unsubscribed from events" << std::endl;
}

void HealthSystem::onEvent(const std::shared_ptr<engine::event::Event>& event) {
    if (!event) return;
    
    switch (event->GetType()) {
        case engine::event::EventType::CUSTOM:
            HandleGameEvent(event);
            break;
        default:
            break;
    }
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
    
    std::cout << "[HealthSystem] Entity " << entityId << " died! Destroying entity..." << std::endl;
    
    PublishDeathEvent(entityId);
    
    // Use same cleanup pattern as ProjectileSystem to ensure complete removal
    auto& entityFactory = world->GetEntityFactory();
    
    // 1. First remove all components (including Sprite2D to stop rendering)
    componentManager.RemoveAllComponents(entityId);
    
    // 2. Then destroy the entity ID
    entityFactory.DestroyEntity(entityId);
    
    std::cout << "[HealthSystem] Entity " << entityId << " destroyed successfully with full component cleanup" << std::endl;
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
    auto& componentManager = world->GetComponentManager();
    
    // Check if the entity is an enemy
    auto* enemyComponent = componentManager.GetComponent<Component::EnemyComponent>(entityId);
    if (enemyComponent) {
        // This is an enemy, publish ENEMY_KILLED event
        auto enemyKilledData = std::make_shared<Events::EnemyKilledData>();
        enemyKilledData->enemyId = entityId;
        enemyKilledData->expReward = static_cast<int>(enemyComponent->expValue);
        
        // Find the killer (usually the last damage source)
        auto* combatStats = componentManager.GetComponent<Component::CombatStatsComponent>(entityId);
        if (combatStats) {
            enemyKilledData->playerId = combatStats->lastDamageSource;
        } else {
            // Default to player entity (ID 4) if no combat stats
            enemyKilledData->playerId = 4;  // TODO: Better way to find player
        }
        
        auto enemyKilledEvent = std::make_shared<Events::GameEvent>(
            Events::GameEventType::ENEMY_KILLED,
            std::static_pointer_cast<void>(enemyKilledData)
        );
        enemyKilledEvent->SetPriority(engine::event::EventPriority::HIGH);
        eventManager.Publish(enemyKilledEvent);
        
        std::cout << "[HealthSystem] Enemy " << entityId << " killed! Exp reward: " 
                  << enemyKilledData->expReward << " to player " << enemyKilledData->playerId << std::endl;
    }
    
    // Always publish the general death event too
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
    
    auto& componentManager = world->GetComponentManager();
    auto* health = componentManager.GetComponent<ZombieSurvivor::Component::HealthComponent>(entityId);
    return health ? health->health : 0.0f;
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



void HealthSystem::HandleGameEvent(const std::shared_ptr<engine::event::Event>& event) {
    auto gameEvent = std::dynamic_pointer_cast<Events::GameEvent>(event);
    if (!gameEvent) return;
    
    auto eventType = gameEvent->GetGameEventType();
    
    switch (eventType) {
        case Events::GameEventType::DAMAGE_TAKEN:
            HandleDamageEvent(gameEvent->GetData());
            break;
        default:
            break;
    }
}

void HealthSystem::HandleDamageEvent(const std::shared_ptr<void>& eventData) {
    auto damageData = std::static_pointer_cast<Events::DamageData>(eventData);
    if (!damageData) return;
    
    ModifyHealth(damageData->targetEntityId, -static_cast<float>(damageData->damageAmount));
    
    std::cout << "[HealthSystem] Applied " << damageData->damageAmount 
              << " damage to entity " << damageData->targetEntityId << std::endl;
}

} // namespace ZombieSurvivor::System
