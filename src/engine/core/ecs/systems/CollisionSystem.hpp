// src/core/ecs/systems/CollisionSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Collider2D.hpp"
#include "engine/core/ecs/ComponentManager.hpp" 
#include "engine/core/event/EventManager.hpp"
#include "engine/core/event/events/PhysicsEvents.hpp"
#include "engine/core/ecs/spatial/SpatialPartition.hpp"
#include <vector>
#include <unordered_map>
#include <string>

namespace engine::ECS {

class CollisionSystem : public System {
public:

    enum class SpatialType {
        BRUTE_FORCE,
        SIMPLE_GRID,
        QUAD_TREE
    };

    CollisionSystem();
    ~CollisionSystem() = default;

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    const char* GetName() const override { return "CollisionSystem"; }

    void AddCollisionLayer(const std::string& layer, bool enabled = true);
    void SetCollisionRule(const std::string& layerA, const std::string& layerB, bool canCollide);

    void SetEventManager(engine::event::EventManager* eventManager);

    void SetSpatialType(SpatialType type);
    void SetWorldBounds(const SDL_FRect& bounds);
    void SetGridCellSize(float cellSize);
    void SetQuadTreeParams(int maxDepth, int maxEntitiesPerNode);
    
    size_t GetCollisionCheckCount() const { return collisionCheckCount_; }
    size_t GetCollisionCount() const { return collisionCount_; }
    void ResetStats();

    void PrintSpatialStats() const;
    SpatialType GetCurrentSpatialType() const { return currentSpatialType_; }

private:
    bool CheckAABBCollision(const SDL_FRect& a, const SDL_FRect& b) const;
    void PublishCollisionEvent(EntityID entityA, EntityID entityB, const SDL_FRect& boundA, const SDL_FRect& boundB, const std::string& layerA, const std::string& layerB, bool isTrigger);
    
    bool canLayersCollide(const std::string& layerA, const std::string& layerB) const;
    
    // Spatial Optimization (Reserved)
    void InitializeSpatialPartition();
    void UpdateSpatialPartition();
    void PerformBruteForceCollisionDetection();
    void PerformSpatialCollisionDetection();

    void ProcessCollisionSafe(EntityID entityA, EntityID entityB, const Collider2D& colliderA, const Collider2D& colliderB, const SDL_FRect& worldBoundsA, const SDL_FRect& worldBoundsB);

    std::unordered_map<std::string, bool> enabledLayers_;
    std::unordered_map<std::string, std::unordered_map<std::string, bool>> collisionRules_;
    
    size_t collisionCheckCount_ = 0;
    size_t collisionCount_ = 0;
    struct EntityCollisionData {
        Collider2D* collider;
        SDL_FRect worldBounds;
    };
    
    std::vector<EntityID> entitiesWithColliders_;
    std::unordered_map<EntityID, SDL_FRect> colliderBoundsCache_;
    std::unordered_map<EntityID, EntityCollisionData> entityDataCache_;

    std::unique_ptr<SpatialPartition> spatialPartition_;
    SpatialType currentSpatialType_ = SpatialType::BRUTE_FORCE;
    SDL_FRect worldBounds_ = {0, 0, 2000, 2000};
    float gridCellSize_ = 64.0f;
    int quadTreeMaxDepth_ = 8;
    int quadTreeMaxEntities_ = 10;
    
    engine::event::EventManager* eventManager_;

    struct CollisionPair {
        EntityID entityA;
        EntityID entityB;
        Collider2D* colliderA;
        Collider2D* colliderB;
        SDL_FRect boundsA;
        SDL_FRect boundsB;
    };
};

} // namespace engine::ECS
    