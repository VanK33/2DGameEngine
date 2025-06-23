// src/engine/core/ecs/systems/CollisionSystem.cpp

#include "CollisionSystem.hpp"
#include "engine/core/ecs/World.hpp"
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
    
    // Get entities with Collider2D components
    auto& componentManager = world_->GetComponentManager();
    auto entitiesWithTransform = componentManager.GetEntitiesWithComponent<Transform2D>();
    
    for (auto entityId : entitiesWithTransform) {
        if (componentManager.HasComponent<Collider2D>(entityId)) {
            entitiesWithColliders_.push_back(entityId);
        }
    }
    
    // Update collider bounds for all entities with colliders
    for (auto entityId : entitiesWithColliders_) {
        auto* transform = componentManager.GetComponent<Transform2D>(entityId);
        auto* collider = componentManager.GetComponent<Collider2D>(entityId);
        
        if (transform && collider) {
            UpdateColliderBounds(entityId, *transform, *collider);
        }
    }
    
    // Perform collision detection
    for (size_t i = 0; i < entitiesWithColliders_.size(); ++i) {
        for (size_t j = i + 1; j < entitiesWithColliders_.size(); ++j) {
            auto entityA = entitiesWithColliders_[i];
            auto entityB = entitiesWithColliders_[j];
            
            collisionCheckCount_++;
            
            auto* colliderA = componentManager.GetComponent<Collider2D>(entityA);
            auto* colliderB = componentManager.GetComponent<Collider2D>(entityB);
            
            if (colliderA && colliderB) {
                // Check collision layers
                if (!canLayersCollide(colliderA->layer, colliderB->layer)) {
                    continue;
                }
                
                // Check AABB collision
                if (CheckAABBCollision(colliderA->bounds, colliderB->bounds)) {
                    collisionCount_++;
                    ProcessCollision(entityA, entityB, *colliderA, *colliderB);
                }
            }
        }
    }
    
    // Output performance statistics (debug)
    if (collisionCheckCount_ > 0) {
        std::cout << "[CollisionSystem] Checks: " << collisionCheckCount_ 
                  << ", Collisions: " << collisionCount_ << std::endl;
    }
}

void CollisionSystem::Shutdown() {
    std::cout << "[CollisionSystem] Shutdown" << std::endl;
    entitiesWithColliders_.clear();
    colliderBoundsCache_.clear();
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
    return (a.x < b.x + b.w && a.x + a.w > b.x &&
            a.y < b.y + b.h && a.y + a.h > b.y);
}

void CollisionSystem::UpdateColliderBounds(EntityID entity, Transform2D& transform, Collider2D& collider) {
    // Convert local collider coordinates to world coordinates
    collider.bounds.x = transform.x + collider.bounds.x * transform.scaleX;
    collider.bounds.y = transform.y + collider.bounds.y * transform.scaleY;
    collider.bounds.w *= transform.scaleX;
    collider.bounds.h *= transform.scaleY;
    
    // Cache world coordinates
    colliderBoundsCache_[entity] = collider.bounds;
}

void CollisionSystem::ProcessCollision(EntityID entityA, EntityID entityB, 
                                      const Collider2D& colliderA, const Collider2D& colliderB) {
    // Determine if this is a trigger collision
    bool isTrigger = colliderA.isTrigger || colliderB.isTrigger;
    
    // Publish collision event
    PublishCollisionEvent(entityA, entityB, colliderA, colliderB, isTrigger);
}

void CollisionSystem::PublishCollisionEvent(EntityID entityA, EntityID entityB, 
                                           const Collider2D& colliderA, const Collider2D& colliderB,
                                           bool isTrigger) {
    if (!eventManager_) {
        return;
    }
    
    // Create collision data
    engine::event::CollisionData collisionData;
    collisionData.entityA = entityA;
    collisionData.entityB = entityB;
    collisionData.isTrigger = isTrigger;
    collisionData.layerA = colliderA.layer;
    collisionData.layerB = colliderB.layer;
    
    // Calculate overlap area
    SDL_FRect overlap;
    overlap.x = std::max(colliderA.bounds.x, colliderB.bounds.x);
    overlap.y = std::max(colliderA.bounds.y, colliderB.bounds.y);
    overlap.w = std::min(colliderA.bounds.x + colliderA.bounds.w, 
                        colliderB.bounds.x + colliderB.bounds.w) - overlap.x;
    overlap.h = std::min(colliderA.bounds.y + colliderA.bounds.h, 
                        colliderB.bounds.y + colliderB.bounds.h) - overlap.y;
    
    collisionData.overlap = overlap;
    
    // Create shared_ptr to collision data and Event
    auto collisionDataPtr = std::make_shared<engine::event::CollisionData>(collisionData);
    auto event = std::make_shared<engine::event::Event>(
        isTrigger ? engine::event::EventType::TRIGGER_ENTERED : engine::event::EventType::COLLISION_STARTED,
        std::static_pointer_cast<void>(collisionDataPtr)
    );
    
    // Publish the event
    eventManager_->publish(event);
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

void CollisionSystem::UpdateSpatialGrid() {
    // TODO: Implement spatial partitioning optimization
    // This will improve performance with many entities
}

std::vector<EntityID> CollisionSystem::GetNearbyEntities(EntityID entity) {
    // TODO: Implement spatial partitioning query
    // For now, return all entities (will be optimized later)
    return entitiesWithColliders_;
}

void CollisionSystem::ResetStats() {
    collisionCheckCount_ = 0;
    collisionCount_ = 0;
}

void CollisionSystem::SetEventManager(engine::event::EventManager* eventManager) { 
    eventManager_ = eventManager; 
}

} // namespace engine::ECS