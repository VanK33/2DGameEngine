#pragma once
#include "SpatialPartition.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <atomic>
#include <mutex>
#include <chrono>

namespace engine::ECS {

class SimpleGrid : public SpatialPartition {
public:
    explicit SimpleGrid(float cellSize, const SDL_FRect& worldBounds);
    virtual ~SimpleGrid() = default;

    void Insert(EntityID entity, const SDL_FRect& bounds) override;
    void Update(EntityID entity, const SDL_FRect& bounds) override;
    void Remove(EntityID entity) override;
    void Clear() override;
    
    std::vector<EntityID> Query(const SDL_FRect& area) const override;
    std::vector<EntityID> GetNearbyEntities(EntityID entity, float radius) const override;
    
    size_t GetEntityCount() const override { return entityData_.size(); }
    std::string GetImplementationType() const override { return "SimpleGrid"; }
    
    float GetCellSize() const { return cellSize_; }
    void SetCellSize(float cellSize);
    
    size_t GetGridWidth() const { return gridWidth_; }
    size_t GetGridHeight() const { return gridHeight_; }
    size_t GetTotalCells() const { return gridWidth_ * gridHeight_; }
    size_t GetOccupiedCells() const;

    struct GridStats {
        size_t totalCells;
        size_t occupiedCells;
        size_t totalEntities;
        float averageEntitiesPerCell;
        float cellUtilization;
        size_t maxEntitiesInCell;
        float emptyPercentage;
    };

    void OptimizeCellSize();
    float GetOptimalCellSize() const;
    void SetAutoOptimize(bool enabled) { autoOptimize_ = enabled; }
    
    GridStats GetGridStats() const;
    void PrintGridStats() const;
    void SetDebugMode(bool enabled) { debugMode_ = enabled; }

private:
    struct EntityData {
        SDL_FRect bounds;
        float maxDimension;
        std::vector<size_t> cells;
    };

    static constexpr bool DEFAULT_DEBUG_MODE = false;
    static constexpr size_t AUTO_OPTIMIZE_QUERY_INTERVAL = 100;
    static constexpr float TARGET_ENTITIES_PER_CELL = 4.0f;
    static constexpr float MIN_CELL_SIZE = 16.0f;
    static constexpr float MAX_CELL_SIZE = 512.0f;
    static constexpr size_t OPTIMIZE_INTERVAL_MS = 1000;
    static constexpr size_t MAX_GRID_CELLS = 1000000;

    float cellSize_;
    SDL_FRect worldBounds_;
    size_t gridWidth_;
    size_t gridHeight_;
    
    std::vector<std::unordered_set<EntityID>> grid_;
    std::unordered_map<EntityID, EntityData> entityData_;
    
    bool autoOptimize_ = false;
    bool debugMode_ = DEFAULT_DEBUG_MODE;
    
    mutable std::atomic<size_t> queryCount_{0};
    mutable std::atomic<size_t> totalQueryTime_{0};
    mutable std::atomic<size_t> lastQueryCount_{0};
    mutable std::mutex statsMutex_;
    
    std::chrono::high_resolution_clock::time_point lastOptimizeTime_;

    size_t GetCellIndex(int gridX, int gridY) const;
    void WorldToGrid(float worldX, float worldY, int& gridX, int& gridY) const;
    std::vector<size_t> GetCellsForBounds(const SDL_FRect& bounds) const;
    void AddEntityToCells(EntityID entity, const std::vector<size_t>& cells);
    void RemoveEntityFromCells(EntityID entity, const std::vector<size_t>& cells);
    bool IsValidGridPosition(int gridX, int gridY) const;
    
    bool BoundsIntersect(const SDL_FRect& a, const SDL_FRect& b) const;
    float CalculateDistance(const SDL_FRect& a, const SDL_FRect& b) const;
    void ResetPerformanceStats();
    
    static float CalculateMaxDimension(const SDL_FRect& bounds);
};

} // namespace engine::ECS