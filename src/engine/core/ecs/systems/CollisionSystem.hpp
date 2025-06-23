// src/core/ecs/systems/CollisionSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Collider2D.hpp"
#include "engine/core/ecs/ComponentManager.hpp" 
#include "engine/core/event/EventManager.hpp"
#include "engine/core/event/events/PhysicsEvents.hpp"
#include <vector>
#include <unordered_map>
#include <string>

namespace engine::ECS {

class CollisionSystem : public System {
public:
    CollisionSystem();
    ~CollisionSystem() = default;

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    const char* GetName() const override { return "CollisionSystem"; }

    void AddCollisionLayer(const std::string& layer, bool enabled = true);
    void SetCollisionRule(const std::string& layerA, const std::string& layerB, bool canCollide);

    void SetEventManager(engine::event::EventManager* eventManager);
    
    size_t GetCollisionCheckCount() const { return collisionCheckCount_; }
    size_t GetCollisionCount() const { return collisionCount_; }
    void ResetStats();

private:
    bool CheckAABBCollision(const SDL_FRect& a, const SDL_FRect& b) const;
    void UpdateColliderBounds(EntityID entity, Transform2D& transform, Collider2D& collider);
    
    void ProcessCollision(EntityID entityA, EntityID entityB, const Collider2D& colliderA, const Collider2D& colliderB);
    void PublishCollisionEvent(EntityID entityA, EntityID entityB, const Collider2D& colliderA, const Collider2D& colliderB, bool isTrigger);
    
    bool canLayersCollide(const std::string& layerA, const std::string& layerB) const;
    
    // Spatial Optimization (Reserved)
    void UpdateSpatialGrid();
    std::vector<EntityID> GetNearbyEntities(EntityID entity);

    std::unordered_map<std::string, bool> enabledLayers_;
    std::unordered_map<std::string, std::unordered_map<std::string, bool>> collisionRules_;
    
    size_t collisionCheckCount_ = 0;
    size_t collisionCount_ = 0;
    
    std::vector<EntityID> entitiesWithColliders_;
    std::unordered_map<EntityID, SDL_FRect> colliderBoundsCache_;
    
    engine::event::EventManager* eventManager_;
};

} // namespace engine::ECS
    