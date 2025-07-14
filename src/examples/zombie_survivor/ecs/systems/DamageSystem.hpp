#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/event/EventListener.hpp"
#include "engine/core/event/Event.hpp"
#include "engine/core/ecs/ComponentManager.hpp"
#include <memory>
#include <string>

namespace ZombieSurvivor::System {

class DamageSystem : public engine::ECS::System, public engine::event::EventListener {
public:
    DamageSystem() = default;
    
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "DamageSystem"; }
    
    void onEvent(const std::shared_ptr<engine::event::Event>& event) override;
    
    void DealDamage(uint32_t targetEntityId, uint32_t sourceEntityId, 
                   int damage, const std::string& damageType = "unknown");
    
private:
    void HandleGameEvent(const std::shared_ptr<engine::event::Event>& event);
    void HandleCollisionEvent(const std::shared_ptr<engine::event::Event>& event);
    int CalculateDamage(uint32_t attackerId, uint32_t targetId, int baseDamage);
    void HandleProjectileEnemyCollision(engine::ECS::EntityID projectileId, engine::ECS::EntityID enemyId);
    
    void PublishDamageEvent(uint32_t targetEntityId, uint32_t sourceEntityId, 
                           int damage, const std::string& damageType);

    void HandleEnemyPlayerCollision(uint32_t entityA, uint32_t entityB);
    bool IsPlayer(uint32_t entityId);
    bool IsEnemy(uint32_t entityId);
};

} // namespace ZombieSurvivor::System 