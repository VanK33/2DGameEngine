// src/engine/core/ecs/spatial/SimpleGrid.cpp

#include "SimpleGrid.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace engine::ECS {

SimpleGrid::SimpleGrid(flaot cellSize, const SDL_FRect& worldBounds)
    : cellSize_(cellSize), worldBounds_(worldBounds) {
    
    if(cellSize <= 0.0f) {
        std::cerr << "[SimpleGrid] Warning: cellSize must be positive, using default 64.0f" << std::endl;
        cellSize_ = 64.0f;
    }

    gridWidth_ = static_cast<size_t>(std::ceil(worldBounds.w / cellSize_));
    gridHeight_ = static_cast<size_t>(std::ceil(worldBounds.h / cellSize_));

    size_t totalCells = gridWidth_ * gridHeight_;
    grid_.resize(totalCells);

    if (debugMode_) {
        std::cout << "[SimpleGrid] Created grid: " << gridWidth_ << "x" << gridHeight_ 
                  << " (" << totalCells << " cells), cellSize: " << cellSize_ << std::endl;
    }
}

void SimpleGrid::Insert(EntityID entity, const SDL_FRect& bounds) {
    if (entityBounds_.find(entity) != entityBounds_.end()) {
        Remove(entity);
    }

    entityBounds_[entity] = bounds;
    auto cells = GetCellsForBounds(bounds);
    entityCells_[entity] = cells;

    AddEntityToCells(entity, cells);

    if (debugMode_) {
        std::cout << "[SimpleGrid] Inserted entity " << entity 
                  << " into " << cells.size() << " cells" << std::endl;
    }
}

void SimpleGrid::Update(EntityID entity, const SDL_FRect& bounds) {
    auto it = entityBounds_.find(entity);
    if (it == entityBounds_.end()) {
        // Entity doesn't exist, treat as insert
        Insert(entity, bounds);
        return;
    }
    
    const SDL_FRect& oldBounds = it->second;
    if (oldBounds.x == bounds.x && oldBounds.y == bounds.y && 
        oldBounds.w == bounds.w && oldBounds.h == bounds.h) {
        return; // No change needed
    }
    
    auto oldCells = entityCells_[entity];
    auto newCells = GetCellsForBounds(bounds);
    entityBounds_[entity] = bounds;
    entityCells_[entity] = newCells;
    
    // Remove from old cells that are not in new cells
    for (size_t oldCell : oldCells) {
        if (std::find(newCells.begin(), newCells.end(), oldCell) == newCells.end()) {
            grid_[oldCell].erase(entity);
        }
    }
    
    // Add to new cells that were not in old cells
    for (size_t newCell : newCells) {
        if (std::find(oldCells.begin(), oldCells.end(), newCell) == oldCells.end()) {
            grid_[newCell].insert(entity);
        }
    }
}

void SimpleGrid::Remove(EntityID entity) {
    auto boundsIt = entityBounds_.find(entity);
    if (boundsIt == entityBounds_.end()) {
        return; // Entity not found
    }

    auto cellsIt = entityCells_.find(entity);
    if (cellsIt != entityCells_.end()) {
        RemoveEntityFromCells(entity, cellsIt->second);
        entityCells_.erase(cellsIt);
    }

    entityBounds_.erase(boundsIt);

    if (debugMode_) {
        std::cout << "[SimpleGrid] Removed entity " << entity << std::endl;
    }
}

void SimpleGrid::Clear() {
    for (auto& cell : grid_) {
        cell.clear();
    }
    entityBounds_.clear();
    entityCells_.clear();

    if (debugMode_) {
        std::cout << "[SimpleGrid] Cleared grid" << std::endl;
    }
}

std::vector<EntityID> SimpleGrid::Query(const SDL_FRect& area) const {
    lastQueryCount_ = 0;
    std::unordered_set<EntityID> result;
    
    auto cells = GetCellsForBounds(area);
    
    for (size_t cellIndex : cells) {
        const auto& cell = grid_[cellIndex];
        lastQueryCount_ += cell.size();
        
        for (EntityID entity : cell) {
            auto it = entityBounds_.find(entity);
            if (it != entityBounds_.end() && BoundsIntersect(area, it->second)) {
                result.insert(entity);
            }
        }
    }
    
    return std::vector<EntityID>(result.begin(), result.end());
}

std::vector<EntityID> SimpleGrid::GetNearbyEntities(EntityID entity, float radius) const {
    auto it = entityBounds_.find(entity);
    if (it == entityBounds_.end()) {
        return {}; // Entity not found
    }
    
    const SDL_FRect& bounds = it->second;
    float centerX = bounds.x + bounds.w * 0.5f;
    float centerY = bounds.y + bounds.h * 0.5f;
    
    SDL_FRect queryArea = {
        centerX - radius, centerY - radius,
        radius * 2.0f, radius * 2.0f
    };
    
    auto candidates = Query(queryArea);
    std::vector<EntityID> result;
    
    for (EntityID candidate : candidates) {
        if (candidate == entity) continue; // Skip self
        
        auto candidateIt = entityBounds_.find(candidate);
        if (candidateIt != entityBounds_.end()) {
            float distance = CalculateDistance(bounds, candidateIt->second);
            if (distance <= radius) {
                result.push_back(candidate);
            }
        }
    }
    
    return result;
}

void SimpleGrid::SetCellSize(float cellSize) {
    if (cellSize <= 0.0f) {
        std::cerr << "[SimpleGrid] Warning: cellSize must be positive" << std::endl;
        return;
    }
    
    // Store current entities
    auto entities = entityBounds_;
    
    // Rebuild grid with new cell size
    cellSize_ = cellSize;
    gridWidth_ = static_cast<size_t>(std::ceil(worldBounds_.w / cellSize_));
    gridHeight_ = static_cast<size_t>(std::ceil(worldBounds_.h / cellSize_));
    
    Clear();
    grid_.clear();
    grid_.resize(gridWidth_ * gridHeight_);
    
    // Re-insert all entities
    for (const auto& [entity, bounds] : entities) {
        Insert(entity, bounds);
    }
}

size_t SimpleGrid::GetOccupiedCells() const {
    size_t count = 0;
    for (const auto& cell : grid_) {
        if (!cell.empty()) {
            count++;
        }
    }
    return count;
}

// Private helper methods

size_t SimpleGrid::GetCellIndex(int gridX, int gridY) const {
    return static_cast<size_t>(gridY) * gridWidth_ + static_cast<size_t>(gridX);
}

void SimpleGrid::WorldToGrid(float worldX, float worldY, int& gridX, int& gridY) const {
    gridX = static_cast<int>((worldX - worldBounds_.x) / cellSize_);
    gridY = static_cast<int>((worldY - worldBounds_.y) / cellSize_);
    
    // Clamp to grid bounds
    gridX = std::max(0, std::min(gridX, static_cast<int>(gridWidth_) - 1));
    gridY = std::max(0, std::min(gridY, static_cast<int>(gridHeight_) - 1));
}

std::vector<size_t> SimpleGrid::GetCellsForBounds(const SDL_FRect& bounds) const {
    std::vector<size_t> cells;
    
    int minX, minY, maxX, maxY;
    WorldToGrid(bounds.x, bounds.y, minX, minY);
    WorldToGrid(bounds.x + bounds.w, bounds.y + bounds.h, maxX, maxY);
    
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            if (IsValidGridPosition(x, y)) {
                cells.push_back(GetCellIndex(x, y));
            }
        }
    }
    
    return cells;
}

void SimpleGrid::AddEntityToCells(EntityID entity, const std::vector<size_t>& cells) {
    for (size_t cellIndex : cells) {
        grid_[cellIndex].insert(entity);
    }
}

void SimpleGrid::RemoveEntityFromCells(EntityID entity, const std::vector<size_t>& cells) {
    for (size_t cellIndex : cells) {
        grid_[cellIndex].erase(entity);
    }
}

bool SimpleGrid::IsValidGridPosition(int gridX, int gridY) const {
    return gridX >= 0 && gridX < static_cast<int>(gridWidth_) && 
           gridY >= 0 && gridY < static_cast<int>(gridHeight_);
}

} // namespace engine::ECS