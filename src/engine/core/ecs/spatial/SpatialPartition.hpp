// src/engine/core/ecs/spatial/SpatialPartition.hpp

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <SDL3/SDL.h>
#include "engine/core/Types.hpp"

namespace engine::ECS {

using engine::EntityID;

class SpatialPartition {
public:
    virtual ~SpatialPartition() = default;
    
    virtual void Insert(EntityID entity, const SDL_FRect& bounds) = 0;
    virtual void Update(EntityID entity, const SDL_FRect& bounds) = 0;
    virtual void Remove(EntityID entity) = 0;
    virtual void Clear() = 0;
    
    virtual std::vector<EntityID> Query(const SDL_FRect& area) const = 0;
    virtual std::vector<EntityID> GetNearbyEntities(EntityID entity, float radius) const = 0;
    
    virtual size_t GetEntityCount() const = 0;
    virtual std::string GetImplementationType() const = 0;
    virtual size_t GetLastQueryCount() const { return lastQueryCount_; }
    virtual void ResetQueryStats() { lastQueryCount_ = 0; }
    
    virtual void SetDebugMode(bool enabled) { debugMode_ = enabled; }
    virtual bool IsDebugMode() const { return debugMode_; }

protected:
    mutable size_t lastQueryCount_ = 0;
    bool debugMode_ = false;
    
    bool BoundsIntersect(const SDL_FRect& a, const SDL_FRect& b) const {
        return (a.x < b.x + b.w && a.x + a.w > b.x &&
                a.y < b.y + b.h && a.y + a.h > b.y);
    }
    
    // Calculate distance between two rectangles
    float CalculateDistance(const SDL_FRect& a, const SDL_FRect& b) const {
        float centerAX = a.x + a.w * 0.5f;
        float centerAY = a.y + a.h * 0.5f;
        float centerBX = b.x + b.w * 0.5f;
        float centerBY = b.y + b.h * 0.5f;
        
        float dx = centerAX - centerBX;
        float dy = centerAY - centerBY;
        return std::sqrt(dx * dx + dy * dy);
    }
};


class SpatialPartitionFactory {
public:
    enum class Type {
        SIMPLE_GRID,
        QUAD_TREE,
        ADAPTIVE
    };
    
    static std::unique_ptr<SpatialPartition> Create(Type type, const SDL_FRect& worldBounds);
    static std::unique_ptr<SpatialPartition> CreateGrid(float cellSize, const SDL_FRect& worldBounds);
    static std::unique_ptr<SpatialPartition> CreateQuadTree(int maxDepth, int maxEntitiesPerNode, const SDL_FRect& worldBounds);
};

} // namespace engine::ECS