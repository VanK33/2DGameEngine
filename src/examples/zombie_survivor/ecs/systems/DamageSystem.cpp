#include "DamageSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "engine/core/event/events/PhysicsEvents.hpp"
#include "examples/zombie_survivor/ecs/components/WeaponComponent.hpp"
#include "examples/zombie_survivor/ecs/components/CombatStatsComponent.hpp"
#include "examples/zombie_survivor/ecs/components/HealthComponent.hpp"
#include "examples/zombie_survivor/events/GameEventTypes.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include <iostream>

namespace ZombieSurvivor::System {

void DamageSystem::Init() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    // 订阅碰撞事件
    eventManager.Subscribe(engine::event::EventType::ENTITY_COLLISION, this);
    eventManager.Subscribe(engine::event::EventType::COLLISION_STARTED, this);
    
    // 订阅游戏自定义事件
    eventManager.Subscribe(engine::event::EventType::CUSTOM, this);
    
    std::cout << "[DamageSystem] Initialized and subscribed to collision events" << std::endl;
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
    
    // 取消订阅事件
    eventManager.Unsubscribe(engine::event::EventType::ENTITY_COLLISION, this);
    eventManager.Unsubscribe(engine::event::EventType::COLLISION_STARTED, this);
    eventManager.Unsubscribe(engine::event::EventType::CUSTOM, this);
    
    std::cout << "[DamageSystem] Shutdown and unsubscribed from events" << std::endl;
}

void DamageSystem::onEvent(const std::shared_ptr<engine::event::Event>& event) {
    if (!event) return;
    
    switch (event->GetType()) {
        case engine::event::EventType::ENTITY_COLLISION:
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
    
    // 尝试获取碰撞数据
    auto collisionData = std::static_pointer_cast<engine::event::CollisionData>(event->GetData());
    if (!collisionData) {
        std::cout << "[DamageSystem] Warning: No collision data in collision event" << std::endl;
        return;
    }
    
    uint32_t entityA = collisionData->entityA;
    uint32_t entityB = collisionData->entityB;
    
    // 检查哪个实体有武器组件（攻击者）
    auto* weaponA = componentManager.GetComponent<Component::WeaponComponent>(entityA);
    auto* weaponB = componentManager.GetComponent<Component::WeaponComponent>(entityB);
    
    // 检查哪个实体有生命值组件（可以受伤的目标）
    auto* healthA = componentManager.GetComponent<Component::HealthComponent>(entityA);
    auto* healthB = componentManager.GetComponent<Component::HealthComponent>(entityB);
    
    // 处理伤害：有武器的攻击有生命值的
    if (weaponA && healthB && healthB->isAlive) {
        int damage = CalculateDamage(entityA, entityB, static_cast<int>(weaponA->damage));
        DealDamage(entityB, entityA, damage, "weapon");
    }
    
    if (weaponB && healthA && healthA->isAlive) {
        int damage = CalculateDamage(entityB, entityA, static_cast<int>(weaponB->damage));
        DealDamage(entityA, entityB, damage, "weapon");
    }
}

void DamageSystem::DealDamage(uint32_t targetEntityId, uint32_t sourceEntityId, 
                             int damage, const std::string& damageType) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    
    // 验证目标实体是否有生命值组件
    auto* health = componentManager.GetComponent<Component::HealthComponent>(targetEntityId);
    if (!health || !health->isAlive) {
        return;  // 目标没有生命值或已死亡
    }
    
    // 确保伤害值为正
    if (damage <= 0) {
        return;
    }
    
    auto* targetStats = componentManager.GetComponent<Component::CombatStatsComponent>(targetEntityId);
    if (targetStats) {
        targetStats->totalDamageTaken += damage;
        targetStats->lastDamageSource = sourceEntityId;
        targetStats->lastDamageTime = 0.0f; // 这里应该是当前时间，简化处理
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

} // namespace ZombieSurvivor::System 