#include "DamageSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/Tag.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/event/EventManager.hpp"
#include "engine/core/event/events/PhysicsEvents.hpp"
#include "examples/zombie_survivor/ecs/components/WeaponComponent.hpp"
#include "examples/zombie_survivor/ecs/components/CombatStatsComponent.hpp"
#include "examples/zombie_survivor/ecs/components/HealthComponent.hpp"
#include "examples/zombie_survivor/ecs/components/EnemyComponent.hpp"
#include "examples/zombie_survivor/events/GameEventTypes.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include "examples/zombie_survivor/events/ProjectileEventUtils.hpp"
#include <iostream>

namespace ZombieSurvivor::System {

void DamageSystem::Init() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    eventManager.Subscribe(engine::event::EventType::COLLISION_STARTED, this);
    eventManager.Subscribe(engine::event::EventType::CUSTOM, this);
    
    std::cout << "[DamageSystem] Subscribed to collision events" << std::endl;
}

void DamageSystem::Update(float deltaTime) {
    // DamageSystem 主要是事件驱动的，Update 中可以处理一些周期性的伤害效果
    // 比如毒伤、燃烧等DOT（Damage Over Time）效果
    // 目前保持空实现
}

void DamageSystem::Shutdown() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    eventManager.Unsubscribe(engine::event::EventType::COLLISION_STARTED, this);
    eventManager.Unsubscribe(engine::event::EventType::CUSTOM, this);
    
    std::cout << "[DamageSystem] Shutdown and unsubscribed from events" << std::endl;
}

void DamageSystem::onEvent(const std::shared_ptr<engine::event::Event>& event) {
    if (!event) return;
    
    switch (event->GetType()) {
        case engine::event::EventType::COLLISION_STARTED:
            HandleCollisionEvent(event);
            break;
        case engine::event::EventType::CUSTOM:
            HandleGameEvent(event);
            break;
        default:
            break;
    }
}

void DamageSystem::HandleGameEvent(const std::shared_ptr<engine::event::Event>& event) {
    // 处理游戏自定义事件，比如武器开火事件
    // 这里可以扩展处理其他游戏特定的伤害触发事件
}

void DamageSystem::HandleCollisionEvent(const std::shared_ptr<engine::event::Event>& event) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto collisionData = std::static_pointer_cast<engine::event::CollisionData>(event->GetData());
    if (!collisionData) return;
    
    engine::EntityID entityA = collisionData->entityA;
    engine::EntityID entityB = collisionData->entityB;
    
    std::cout << "[DamageSystem] Collision event: A=" << entityA << " B=" << entityB 
              << " LayerA=" << collisionData->layerA << " LayerB=" << collisionData->layerB << std::endl;
    
    bool isProjectileA = componentManager.HasComponent<Component::ProjectileComponent>(entityA);
    bool isProjectileB = componentManager.HasComponent<Component::ProjectileComponent>(entityB);
    bool isEnemyA = componentManager.HasComponent<Component::EnemyComponent>(entityA);
    bool isEnemyB = componentManager.HasComponent<Component::EnemyComponent>(entityB);
    bool isPlayerA = IsPlayer(entityA);
    bool isPlayerB = IsPlayer(entityB);
    
    std::cout << "[DamageSystem] Entity check - A=" << entityA << "(Enemy:" << isEnemyA << ", Player:" << isPlayerA << ") "
              << "B=" << entityB << "(Enemy:" << isEnemyB << ", Player:" << isPlayerB << ")" << std::endl;
    
    if (isProjectileA && isEnemyB) {
        HandleProjectileEnemyCollision(entityA, entityB);
    } else if (isProjectileB && isEnemyA) {
        HandleProjectileEnemyCollision(entityB, entityA);
    }
    
    if ((isEnemyA && isPlayerB) || (isEnemyB && isPlayerA)) {
        std::cout << "[DamageSystem] Enemy-Player collision detected! EntityA=" << entityA 
                  << " EntityB=" << entityB << std::endl;
        HandleEnemyPlayerCollision(entityA, entityB);
    }
}

void DamageSystem::DealDamage(uint32_t targetEntityId, uint32_t sourceEntityId, 
                             int damage, const std::string& damageType) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    
    auto* health = componentManager.GetComponent<Component::HealthComponent>(targetEntityId);
    if (!health || !health->isAlive) {
        return;
    }
    
    if (damage <= 0) {
        return;
    }
    
    auto* targetStats = componentManager.GetComponent<Component::CombatStatsComponent>(targetEntityId);
    if (targetStats) {
        targetStats->totalDamageTaken += damage;
        targetStats->lastDamageSource = sourceEntityId;
        targetStats->lastDamageTime = SDL_GetTicks() / 1000.0f;
    }
    
    auto* sourceStats = componentManager.GetComponent<Component::CombatStatsComponent>(sourceEntityId);
    if (sourceStats) {
        sourceStats->totalDamageDealt += damage;
    }
    
    PublishDamageEvent(targetEntityId, sourceEntityId, damage, damageType);
    
    std::cout << "[DamageSystem] Entity " << sourceEntityId 
              << " dealt " << damage << " " << damageType 
              << " damage to entity " << targetEntityId << std::endl;
}

int DamageSystem::CalculateDamage(uint32_t attackerId, uint32_t targetId, int baseDamage) {
    auto* world = GetWorld();
    if (!world) return baseDamage;
    
    auto& componentManager = world->GetComponentManager();
    
    // Base Damage Calculation
    int finalDamage = baseDamage;
    
    // 可以在这里添加更复杂的伤害计算逻辑：
    // - 攻击者的攻击力加成
    // - 目标的防御力减免
    // - 暴击计算
    // - 随机伤害波动
    
    auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(attackerId);
    if (weapon) {
        finalDamage = static_cast<int>(weapon->damage);
    }
    
    return std::max(1, finalDamage);
}

void DamageSystem::PublishDamageEvent(uint32_t targetEntityId, uint32_t sourceEntityId, 
                                     int damage, const std::string& damageType) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    auto damageData = std::make_shared<Events::DamageData>();
    damageData->sourceEntityId = sourceEntityId;
    damageData->targetEntityId = targetEntityId;
    damageData->damageAmount = damage;
    damageData->damageType = damageType;
    
    auto damageEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::DAMAGE_TAKEN,
        std::static_pointer_cast<void>(damageData)
    );
    damageEvent->SetPriority(engine::event::EventPriority::HIGH);
    eventManager.Publish(damageEvent);
    
    auto dealtEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::DAMAGE_DEALT,
        std::static_pointer_cast<void>(damageData)
    );
    eventManager.Publish(dealtEvent);
}

void DamageSystem::HandleEnemyPlayerCollision(uint32_t entityA, uint32_t entityB) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    
    Component::EnemyComponent* enemy = nullptr;
    uint32_t playerEntity = 0;
    uint32_t enemyEntity = 0;
    
    if (IsEnemy(entityA)) {
        enemy = componentManager.GetComponent<Component::EnemyComponent>(entityA);
        enemyEntity = entityA;
        playerEntity = entityB;
    } else {
        enemy = componentManager.GetComponent<Component::EnemyComponent>(entityB);
        enemyEntity = entityB;
        playerEntity = entityA;
    }
    
    if (!enemy) return;
    
    auto* playerHealth = componentManager.GetComponent<Component::HealthComponent>(playerEntity);
    if (!playerHealth || !playerHealth->isAlive) return;
    
    float currentTime = SDL_GetTicks() / 1000.0f;
    if (currentTime - enemy->lastDamageTime < enemy->damageCooldown) {
        return;
    }
    
    DealDamage(playerEntity, enemyEntity, enemy->damage, "contact");
    enemy->lastDamageTime = currentTime;
    
    std::cout << "[DamageSystem] Enemy " << enemyEntity 
              << " dealt contact damage to player " << playerEntity << std::endl;
}

bool DamageSystem::IsPlayer(uint32_t entityId) {
    auto* world = GetWorld();
    if (!world) return false;
    
    auto* tag = world->GetComponentManager().GetComponent<engine::ECS::Tag>(entityId);
    return tag && tag->tag == "player";
}

bool DamageSystem::IsEnemy(uint32_t entityId) {
    auto* world = GetWorld();
    if (!world) return false;
    
    return world->GetComponentManager().GetComponent<Component::EnemyComponent>(entityId) != nullptr;
}


void DamageSystem::HandleProjectileEnemyCollision(engine::ECS::EntityID projectileId, engine::ECS::EntityID enemyId) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    
    auto* projectile = componentManager.GetComponent<Component::ProjectileComponent>(projectileId);
    if (!projectile) return;
    
    std::cout << "[DamageSystem] COLLISION EVENT: Projectile " << projectileId 
              << " hit Enemy " << enemyId << " (hasHit=" << projectile->hasHit << ")" << std::endl;
    
    // Prevent duplicate damage from same projectile
    if (projectile->hasHit) {
        std::cout << "[DamageSystem] DUPLICATE HIT PREVENTED: Projectile " << projectileId 
                  << " already hit a target, ignoring collision" << std::endl;
        return;
    }
    
    // Mark projectile as hit immediately to prevent duplicate processing in same frame
    projectile->hasHit = true;
    
    DealDamage(enemyId, projectile->shooterId, 
               static_cast<int>(projectile->damage), "projectile");
    
    auto& eventManager = world->GetEventManager();
    auto* transform = componentManager.GetComponent<engine::ECS::Transform2D>(projectileId);
    
    Events::ProjectileEventUtils::PublishProjectileHit(
        eventManager,
        projectileId,
        enemyId,
        projectile->shooterId,
        projectile->damage,
        transform ? engine::Vector2{transform->x, transform->y} : engine::Vector2{0, 0},
        "enemy"
    );
    
    std::cout << "[DamageSystem] Projectile hit enemy, damage: " << projectile->damage << std::endl;
}

} // namespace ZombieSurvivor::System 