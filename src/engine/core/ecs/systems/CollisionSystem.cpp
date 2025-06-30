// src/engine/core/ecs/systems/CollisionSystem.cpp

#include "CollisionSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/spatial/SpatialPartition.hpp"
#include <iostream>
#include <algorithm>

namespace engine::ECS {

CollisionSystem::CollisionSystem()
    : collisionCheckCount_(0)
    , collisionCount_(0)
    , eventManager_(nullptr) {

    AddCollisionLayer("default", true);
}

void CollisionSystem::Init() {
    std::cout << "[CollisionSystem] Initialized" << std::endl;

    InitializeSpatialPartition();
}

void CollisionSystem::Update(float deltaTime) {
    if (!world_) {
        return;
    }

    // Reset statistics
    collisionCheckCount_ = 0;
    collisionCount_ = 0;
    
    // Clear previous frame's data
    entitiesWithColliders_.clear();
    colliderBoundsCache_.clear();
    entityDataCache_.clear();
    
    auto& componentManager = world_->GetComponentManager();
    auto entitiesWithTransform = componentManager.GetEntitiesWithComponent<Transform2D>();
    entitiesWithColliders_.reserve(entitiesWithTransform.size());
    
    for (auto entityId : entitiesWithTransform) {
        auto* collider = componentManager.GetComponent<Collider2D>(entityId);
        if (!collider) continue;
        
        auto* transform = componentManager.GetComponent<Transform2D>(entityId);
        if (!transform) continue;
        
        entitiesWithColliders_.push_back(entityId);
        
        SDL_FRect worldBounds;
        worldBounds.x = transform->x + collider->bounds.x * transform->scaleX;
        worldBounds.y = transform->y + collider->bounds.y * transform->scaleY;
        worldBounds.w = collider->bounds.w * transform->scaleX;
        worldBounds.h = collider->bounds.h * transform->scaleY;
        
        colliderBoundsCache_[entityId] = worldBounds;
        entityDataCache_[entityId] = {collider, worldBounds};
    }

    if (currentSpatialType_ == SpatialType::BRUTE_FORCE) {
        PerformBruteForceCollisionDetection();
    } else {
        UpdateSpatialPartition();
        PerformSpatialCollisionDetection();
    }
    
    #ifdef DEBUG
    if (collisionCheckCount_ > 0) {
        std::cout << "[CollisionSystem] Checks: " << collisionCheckCount_ 
                  << ", Collisions: " << collisionCount_ << std::endl;
    }
    #endif
}

void CollisionSystem::Shutdown() {
    std::cout << "[CollisionSystem] Shutdown" << std::endl;
    entitiesWithColliders_.clear();
    colliderBoundsCache_.clear();
    entityDataCache_.clear();
}

void CollisionSystem::AddCollisionLayer(const std::string& layer, bool enabled) {
    enabledLayers_[layer] = enabled;
    std::cout << "[CollisionSystem] Added layer: " << layer 
              << " (" << (enabled ? "enabled" : "disabled") << ")" << std::endl;
}

void CollisionSystem::SetCollisionRule(const std::string& layerA, const std::string& layerB, bool canCollide) {
    collisionRules_[layerA][layerB] = canCollide;
    collisionRules_[layerB][layerA] = canCollide; // Symmetric rule
    std::cout << "[CollisionSystem] Rule: " << layerA << " <-> " << layerB 
              << " = " << (canCollide ? "collide" : "ignore") << std::endl;
}

bool CollisionSystem::CheckAABBCollision(const SDL_FRect& a, const SDL_FRect& b) const {
    if (a.x > b.x + b.w || b.x > a.x + a.w) return false;
    if (a.y > b.y + b.h || b.y > a.y + a.h) return false;
    return true;
}

void CollisionSystem::PublishCollisionEvent(
    EntityID entityA,
    EntityID entityB,
    const SDL_FRect& boundsA,
    const SDL_FRect& boundsB,
    const std::string& layerA,
    const std::string& layerB,
    bool isTrigger
) {
    if (!eventManager_) {
        return;
    }
    
    // Create collision data
    engine::event::CollisionData collisionData;
    collisionData.entityA = entityA;
    collisionData.entityB = entityB;
    collisionData.isTrigger = isTrigger;
    collisionData.layerA = layerA;
    collisionData.layerB = layerB;
    
    // Calculate overlap area
    SDL_FRect overlap;
    overlap.x = std::max(boundsA.x, boundsB.x);
    overlap.y = std::max(boundsA.y, boundsB.y);
    overlap.w = std::min(boundsA.x + boundsA.w, boundsB.x + boundsB.w) - overlap.x;
    overlap.h = std::min(boundsA.y + boundsA.h, boundsB.y + boundsB.h) - overlap.y;
    
    collisionData.overlap = overlap;
    
    // Create shared_ptr to collision data and Event
    auto collisionDataPtr = std::make_shared<engine::event::CollisionData>(collisionData);
    auto event = std::make_shared<engine::event::Event>(
        isTrigger ? engine::event::EventType::TRIGGER_ENTERED : engine::event::EventType::COLLISION_STARTED,
        std::static_pointer_cast<void>(collisionDataPtr)
    );
    
    // Publish the event
    eventManager_->Publish(event);
}

bool CollisionSystem::canLayersCollide(const std::string& layerA, const std::string& layerB) const {
    // Check if layers exist and are enabled
    auto itA = enabledLayers_.find(layerA);
    auto itB = enabledLayers_.find(layerB);
    
    // If layers don't exist, default to allowing collision (backward compatibility)
    if (itA == enabledLayers_.end() || itB == enabledLayers_.end()) {
        return true;
    }
    
    // Check if layers are enabled
    if (!itA->second || !itB->second) {
        return false;
    }
    
    // Check collision rules
    auto ruleIt = collisionRules_.find(layerA);
    if (ruleIt != collisionRules_.end()) {
        auto subRuleIt = ruleIt->second.find(layerB);
        if (subRuleIt != ruleIt->second.end()) {
            return subRuleIt->second;
        }
    }
    
    // Default to allowing collision
    return true;
}

void CollisionSystem::ResetStats() {
    collisionCheckCount_ = 0;
    collisionCount_ = 0;
}

// Spatial Related Code
void CollisionSystem::InitializeSpatialPartition() {
    if (currentSpatialType_ == SpatialType::BRUTE_FORCE) {
        spatialPartition_.reset();
        return;
    }

    SpatialPartitionFactory::Type factoryType;
    switch (currentSpatialType_) {
        case SpatialType::SIMPLE_GRID:
            spatialPartition_ = SpatialPartitionFactory::CreateGrid(gridCellSize_, worldBounds_);
            std::cout << "[CollisionSystem] Initialized SimpleGrid with cellSize:" << gridCellSize_ << std::endl;
            break;
        case SpatialType::QUAD_TREE:
            spatialPartition_ = SpatialPartitionFactory::CreateQuadTree(quadTreeMaxDepth_, quadTreeMaxEntities_, worldBounds_);
            std::cout << "[CollisionSystem] Initialized QuadTree with maxDepth: " << quadTreeMaxDepth_ << ", maxEntities: " << quadTreeMaxEntities_ << std::endl;
            break;
        default:
            spatialPartition_.reset();
    }
}

void CollisionSystem::UpdateSpatialPartition() {
    if (!spatialPartition_) return;

    spatialPartition_->Clear();

    for (auto entityId : entitiesWithColliders_) {
        auto it = colliderBoundsCache_.find(entityId);
        if (it != colliderBoundsCache_.end()) {
            spatialPartition_->Insert(entityId, it->second);
        }
    }
}
    
void CollisionSystem::PerformBruteForceCollisionDetection() {
    for (size_t i = 0; i < entitiesWithColliders_.size(); ++i) {
        auto entityA = entitiesWithColliders_[i];
        auto itA = entityDataCache_.find(entityA);
        if (itA == entityDataCache_.end()) continue;

        for (size_t j = i + 1; j < entitiesWithColliders_.size(); ++j) {
            auto entityB = entitiesWithColliders_[j];
            auto itB = entityDataCache_.find(entityB);
            if (itB == entityDataCache_.end()) continue;
            
            collisionCheckCount_++;
            
            if (!canLayersCollide(itA->second.collider->layer, itB->second.collider->layer)) {
                continue;
            }

            if (CheckAABBCollision(itA->second.worldBounds, itB->second.worldBounds)) {
                collisionCount_++;
                ProcessCollisionSafe(entityA, entityB, *itA->second.collider, *itB->second.collider, itA->second.worldBounds, itB->second.worldBounds);
            }
        }
    }
}

void CollisionSystem::PerformSpatialCollisionDetection() {
    if (!spatialPartition_) {
        PerformBruteForceCollisionDetection();
        return;
    }

    for (auto entityA : entitiesWithColliders_) {
        auto itA = entityDataCache_.find(entityA);
        if (itA == entityDataCache_.end()) continue;

        std::vector<EntityID> candidates = spatialPartition_->Query(itA->second.worldBounds);

        for (auto entityB : candidates) {
            if (entityA >= entityB) continue;

            auto itB = entityDataCache_.find(entityB);
            if (itB == entityDataCache_.end()) continue;

            collisionCheckCount_++;

            if (!canLayersCollide(itA->second.collider->layer, itB->second.collider->layer)) {
                continue;
            }

            if (CheckAABBCollision(itA->second.worldBounds, itB->second.worldBounds)) {
                collisionCount_++;
                ProcessCollisionSafe(entityA, entityB, 
                                   *itA->second.collider, *itB->second.collider,
                                   itA->second.worldBounds, itB->second.worldBounds);
            }
        }
    }
}

void CollisionSystem::ProcessCollisionSafe(
    EntityID entityA, EntityID entityB,
    const Collider2D& colliderA, const Collider2D& colliderB,
    const SDL_FRect& worldBoundsA, const SDL_FRect& worldBoundsB
) {
    bool isTrigger = colliderA.isTrigger || colliderB.isTrigger;
    
    PublishCollisionEvent(entityA, entityB, worldBoundsA, worldBoundsB, 
                         colliderA.layer, colliderB.layer, isTrigger);
}

void CollisionSystem::SetSpatialType(SpatialType type) {
    if (currentSpatialType_ != type) {
        currentSpatialType_ = type;
        InitializeSpatialPartition();
        std::cout << "[CollisionSystem] Switched to " << 
            (type == SpatialType::BRUTE_FORCE ? "BruteForce" :
             type == SpatialType::SIMPLE_GRID ? "SimpleGrid" : "QuadTree") << std::endl;
    }
}

void CollisionSystem::SetWorldBounds(const SDL_FRect& bounds) {
    if (bounds.w <= 0 || bounds.h <= 0) {
        std::cerr << "[CollisionSystem] Warning: Invalid world bounds" << std::endl;
        return;
    }
    worldBounds_ = bounds;
    if (spatialPartition_) {
        InitializeSpatialPartition();
    }
}

void CollisionSystem::SetGridCellSize(float cellSize) {
    if (cellSize <= 0) {
        std::cerr << "[CollisionSystem] Warning: Invalid cell size " << cellSize 
                  << ", keeping current value: " << gridCellSize_ << std::endl;
        return;
    }
    gridCellSize_ = cellSize;
    if (currentSpatialType_ == SpatialType::SIMPLE_GRID && spatialPartition_) {
        InitializeSpatialPartition();
    }
}

void CollisionSystem::SetQuadTreeParams(int maxDepth, int maxEntitiesPerNode) {
    if (maxDepth <= 0 || maxDepth > 20) {
        std::cerr << "[CollisionSystem] Warning: Invalid max depth " << maxDepth 
                  << ", must be between 1 and 20" << std::endl;
        return;
    }
    if (maxEntitiesPerNode <= 0) {
        std::cerr << "[CollisionSystem] Warning: Invalid max entities " << maxEntitiesPerNode 
                  << ", must be positive" << std::endl;
        return;
    }
    
    quadTreeMaxDepth_ = maxDepth;
    quadTreeMaxEntities_ = maxEntitiesPerNode;
    if (currentSpatialType_ == SpatialType::QUAD_TREE && spatialPartition_) {
        InitializeSpatialPartition();
    }
}

void CollisionSystem::PrintSpatialStats() const {
    std::cout << "\n=== CollisionSystem Spatial Stats ===" << std::endl;
    std::cout << "Current Type: " << 
        (currentSpatialType_ == SpatialType::BRUTE_FORCE ? "BruteForce" :
         currentSpatialType_ == SpatialType::SIMPLE_GRID ? "SimpleGrid" : "QuadTree") << std::endl;
    std::cout << "Entities with Colliders: " << entitiesWithColliders_.size() << std::endl;
    std::cout << "Last Frame Checks: " << collisionCheckCount_ << std::endl;
    std::cout << "Last Frame Collisions: " << collisionCount_ << std::endl;
    
    if (spatialPartition_) {
        std::cout << "Spatial Partition Type: " << spatialPartition_->GetImplementationType() << std::endl;
        std::cout << "Spatial Entity Count: " << spatialPartition_->GetEntityCount() << std::endl;
        std::cout << "Last Query Count: " << spatialPartition_->GetLastQueryCount() << std::endl;
    }
    std::cout << "=====================================\n" << std::endl;
}



void CollisionSystem::SetEventManager(engine::event::EventManager* eventManager) { 
    eventManager_ = eventManager; 
}

} // namespace engine::ECS