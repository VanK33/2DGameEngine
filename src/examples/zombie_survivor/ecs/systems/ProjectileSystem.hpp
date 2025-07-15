// src/examples/zombie_survivor/ecs/systems/ProjectileSystem.hpp

#pragma once
#include "engine/core/ecs/System.hpp"
#include "engine/core/event/EventListener.hpp"
#include "engine/core/Types.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include "examples/zombie_survivor/events/GameEventTypes.hpp"
#include <unordered_set>



namespace ZombieSurvivor::System {

class ProjectileSystem : public engine::ECS::System, public engine::event::EventListener {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "ProjectileSystem"; }
    
    void onEvent(const std::shared_ptr<engine::event::Event>& event) override;
    
    size_t GetActiveProjectileCount() const { return activeProjectiles_.size(); }
    
private:
    void HandleGameEvent(const std::shared_ptr<engine::event::Event>& event);
    void HandleCreateProjectile(const std::shared_ptr<void>& eventData);
    void HandleProjectileDestroyed(const std::shared_ptr<void>& eventData);
    
    engine::EntityID CreateProjectileEntity(const ZombieSurvivor::Events::CreateProjectileData& data);
    void UpdateProjectileMovement(float deltaTime);
    void UpdateProjectileLifetime(float deltaTime);
    void CleanupExpiredProjectiles();
    void HandleBoundaryChecks();

    engine::EntityID FindOldestProjectile();
    void DestroyProjectile(engine::EntityID projectileId);
    std::unordered_set<engine::EntityID> activeProjectiles_;
    
    engine::Vector2 worldBounds_{2000.0f, 2000.0f};
    size_t maxActiveProjectiles_ = 200;
};

} // namespace ZombieSurvivor::System
