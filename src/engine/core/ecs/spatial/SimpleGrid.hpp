#pragma once

#include "SpatialPartition.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
    size_t GetEntityCount() const override { return entityBounds_.size(); }
    std::string GetImplementationType() const override { return "SimpleGrid"; }

    float GetCellSize() const { return cellSize_; }
    void SetCellSize(float cellSize);
    
    // Debug information
    size_t GetGridWidth() const { return gridWidth_; }
    size_t GetGridHeight() const { return gridHeight_; }
    size_t GetTotalCells() const { return gridWidth_ * gridHeight_; }
    size_t GetOccupiedCells() const;

private:
    float cellSize_;
    SDL_FRect worldBounds_;
    size_t gridWidth_;
    size_t gridHeight_;
    
    // Grid storage - each cell contains a set of entities
    std::vector<std::unordered_set<EntityID>> grid_;
    std::unordered_map<EntityID, SDL_FRect> entityBounds_;
    std::unordered_map<EntityID, std::vector<size_t>> entityCells_;
    
    // Helper methods
    size_t GetCellIndex(int gridX, int gridY) const;
    void WorldToGrid(float worldX, float worldY, int& gridX, int& gridY) const;
    std::vector<size_t> GetCellsForBounds(const SDL_FRect& bounds) const;
    void AddEntityToCells(EntityID entity, const std::vector<size_t>& cells);
    void RemoveEntityFromCells(EntityID entity, const std::vector<size_t>& cells);
    
    bool IsValidGridPosition(int gridX, int gridY) const;
    void ValidateGridBounds() const;
};

} // namespace engine::ECS
