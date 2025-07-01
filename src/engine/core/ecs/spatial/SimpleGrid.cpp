#include "SimpleGrid.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <limits>

namespace engine::ECS {

SimpleGrid::SimpleGrid(float cellSize, const SDL_FRect& worldBounds)
    : cellSize_(cellSize), worldBounds_(worldBounds) {
    
    if(cellSize <= 0.0f) {
        std::cerr << "[SimpleGrid] Warning: cellSize must be positive, using default 64.0f" << std::endl;
        cellSize_ = 64.0f;
    }

    gridWidth_ = static_cast<size_t>(std::ceil(worldBounds.w / cellSize_));
    gridHeight_ = static_cast<size_t>(std::ceil(worldBounds.h / cellSize_));

    size_t totalCells = gridWidth_ * gridHeight_;
    
    if (totalCells > MAX_GRID_CELLS) {
        std::cerr << "[SimpleGrid] Error: Grid would have " << totalCells 
                  << " cells, exceeding maximum of " << MAX_GRID_CELLS << std::endl;
        float minCellSize = std::sqrt((worldBounds.w * worldBounds.h) / MAX_GRID_CELLS);
        cellSize_ = std::max(minCellSize * 1.1f, MIN_CELL_SIZE);
        gridWidth_ = static_cast<size_t>(std::ceil(worldBounds.w / cellSize_));
        gridHeight_ = static_cast<size_t>(std::ceil(worldBounds.h / cellSize_));
        totalCells = gridWidth_ * gridHeight_;
    }
    
    grid_.resize(totalCells);
    lastOptimizeTime_ = std::chrono::high_resolution_clock::now();

    if (debugMode_) {
        std::cout << "[SimpleGrid] Created grid: " << gridWidth_ << "x" << gridHeight_ 
                  << " (" << totalCells << " cells), cellSize: " << cellSize_ << std::endl;
    }
}

void SimpleGrid::Insert(EntityID entity, const SDL_FRect& bounds) {
    if (entityData_.find(entity) != entityData_.end()) {
        Remove(entity);
    }

    auto cells = GetCellsForBounds(bounds);
    EntityData data{bounds, CalculateMaxDimension(bounds), cells};
    entityData_[entity] = std::move(data);

    AddEntityToCells(entity, entityData_[entity].cells);

    if (debugMode_) {
        std::cout << "[SimpleGrid] Inserted entity " << entity 
                  << " into " << cells.size() << " cells" << std::endl;
    }
}

void SimpleGrid::Update(EntityID entity, const SDL_FRect& bounds) {
    auto it = entityData_.find(entity);
    if (it == entityData_.end()) {
        Insert(entity, bounds);
        return;
    }
    
    const SDL_FRect& oldBounds = it->second.bounds;
    if (oldBounds.x == bounds.x && oldBounds.y == bounds.y && 
        oldBounds.w == bounds.w && oldBounds.h == bounds.h) {
        return;
    }
    
    auto& data = it->second;
    auto oldCells = data.cells;
    auto newCells = GetCellsForBounds(bounds);
    
    data.bounds = bounds;
    data.maxDimension = CalculateMaxDimension(bounds);
    data.cells = newCells;

    std::unordered_set<size_t> newCellsSet(newCells.begin(), newCells.end());
    std::unordered_set<size_t> oldCellsSet(oldCells.begin(), oldCells.end());
    
    for (size_t oldCell : oldCells) {
        if (newCellsSet.find(oldCell) == newCellsSet.end()) {
            grid_[oldCell].erase(entity);
        }
    }
    
    for (size_t newCell : newCells) {
        if (oldCellsSet.find(newCell) == oldCellsSet.end()) {
            grid_[newCell].insert(entity);
        }
    }
}

void SimpleGrid::Remove(EntityID entity) {
    auto it = entityData_.find(entity);
    if (it == entityData_.end()) {
        return;
    }

    RemoveEntityFromCells(entity, it->second.cells);
    entityData_.erase(it);

    if (debugMode_) {
        std::cout << "[SimpleGrid] Removed entity " << entity << std::endl;
    }
}

void SimpleGrid::Clear() {
    for (auto& cell : grid_) {
        cell.clear();
    }
    entityData_.clear();

    if (debugMode_) {
        std::cout << "[SimpleGrid] Cleared grid" << std::endl;
    }
}

std::vector<EntityID> SimpleGrid::Query(const SDL_FRect& area) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    lastQueryCount_.store(0);
    std::unordered_set<EntityID> result;
    
    auto cells = GetCellsForBounds(area);
    
    for (size_t cellIndex : cells) {
        const auto& cell = grid_[cellIndex];
        lastQueryCount_.fetch_add(cell.size());
        
        for (EntityID entity : cell) {
            auto it = entityData_.find(entity);
            if (it != entityData_.end() && BoundsIntersect(area, it->second.bounds)) {
                result.insert(entity);
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    
    queryCount_.fetch_add(1);
    totalQueryTime_.fetch_add(duration);
    
    if (autoOptimize_ && queryCount_.load() % AUTO_OPTIMIZE_QUERY_INTERVAL == 0) {
        const_cast<SimpleGrid*>(this)->OptimizeCellSize();
    }
    
    return std::vector<EntityID>(result.begin(), result.end());
}

std::vector<EntityID> SimpleGrid::GetNearbyEntities(EntityID entity, float radius) const {
    auto it = entityData_.find(entity);
    if (it == entityData_.end()) {
        return {};
    }
    
    const SDL_FRect& bounds = it->second.bounds;
    float centerX = bounds.x + bounds.w * 0.5f;
    float centerY = bounds.y + bounds.h * 0.5f;
    
    SDL_FRect queryArea = {
        centerX - radius, centerY - radius,
        radius * 2.0f, radius * 2.0f
    };
    
    auto candidates = Query(queryArea);
    std::vector<EntityID> result;
    
    for (EntityID candidate : candidates) {
        if (candidate == entity) continue;
        
        auto candidateIt = entityData_.find(candidate);
        if (candidateIt != entityData_.end()) {
            float distance = CalculateDistance(bounds, candidateIt->second.bounds);
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
    
    size_t newGridWidth = static_cast<size_t>(std::ceil(worldBounds_.w / cellSize));
    size_t newGridHeight = static_cast<size_t>(std::ceil(worldBounds_.h / cellSize));
    size_t newTotalCells = newGridWidth * newGridHeight;
    
    if (newTotalCells > MAX_GRID_CELLS) {
        std::cerr << "[SimpleGrid] Warning: New cell size would create " 
                  << newTotalCells << " cells, exceeding limit of " 
                  << MAX_GRID_CELLS << std::endl;
        return;
    }
    
    auto entities = entityData_;
    
    cellSize_ = cellSize;
    gridWidth_ = newGridWidth;
    gridHeight_ = newGridHeight;
    
    Clear();
    grid_.clear();
    grid_.resize(newTotalCells);
    
    for (const auto& [entity, data] : entities) {
        Insert(entity, data.bounds);
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

SimpleGrid::GridStats SimpleGrid::GetGridStats() const {
    GridStats stats{};
    stats.totalCells = grid_.size();
    stats.totalEntities = entityData_.size();
    
    size_t maxEntities = 0;
    size_t occupiedCells = 0;
    
    for (const auto& cell : grid_) {
        if (!cell.empty()) {
            occupiedCells++;
            maxEntities = std::max(maxEntities, cell.size());
        }
    }
    
    stats.occupiedCells = occupiedCells;
    stats.maxEntitiesInCell = maxEntities;
    stats.averageEntitiesPerCell = occupiedCells > 0 ? 
        static_cast<float>(stats.totalEntities) / occupiedCells : 0.0f;
    stats.cellUtilization = stats.totalCells > 0 ? 
        static_cast<float>(occupiedCells) / stats.totalCells : 0.0f;
    stats.emptyPercentage = stats.totalCells > 0 ? 
        ((stats.totalCells - occupiedCells) * 100.0f) / stats.totalCells : 0.0f;
    
    return stats;
}

void SimpleGrid::PrintGridStats() const {
    auto stats = GetGridStats();
    
    std::cout << "\n=== SimpleGrid Statistics ===" << std::endl;
    std::cout << "Grid Size: " << gridWidth_ << "x" << gridHeight_ 
              << " (Cell Size: " << std::fixed << std::setprecision(1) << cellSize_ << ")" << std::endl;
    std::cout << "Total Cells: " << stats.totalCells << std::endl;
    std::cout << "Occupied Cells: " << stats.occupiedCells 
              << " (" << std::fixed << std::setprecision(1) << (stats.cellUtilization * 100) << "%)" << std::endl;
    std::cout << "Total Entities: " << stats.totalEntities << std::endl;
    std::cout << "Avg Entities/Cell: " << std::fixed << std::setprecision(2) << stats.averageEntitiesPerCell << std::endl;
    std::cout << "Max Entities in Cell: " << stats.maxEntitiesInCell << std::endl;
    std::cout << "Empty Cells: " << std::fixed << std::setprecision(1) << stats.emptyPercentage << "%" << std::endl;
    
    size_t queries = queryCount_.load();
    if (queries > 0) {
        std::cout << "Query Count: " << queries << std::endl;
        std::cout << "Avg Query Time: " 
                  << std::fixed << std::setprecision(2) 
                  << (static_cast<float>(totalQueryTime_.load()) / queries) << " μs" << std::endl;
    }
    std::cout << "Auto Optimize: " << (autoOptimize_ ? "Enabled" : "Disabled") << std::endl;
    std::cout << "===========================\n" << std::endl;
}

float SimpleGrid::GetOptimalCellSize() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    auto stats = GetGridStats();
    
    if (stats.totalEntities == 0) {
        return cellSize_;
    }
    
    float worldArea = worldBounds_.w * worldBounds_.h;
    float avgEntityArea = worldArea / stats.totalEntities;
    
    float optimalCellArea = avgEntityArea * TARGET_ENTITIES_PER_CELL;
    float optimalCellSize = std::sqrt(optimalCellArea);
    
    float avgEntitySize = 0.0f;
    float minEntitySize = std::numeric_limits<float>::max();
    
    for (const auto& [entity, data] : entityData_) {
        avgEntitySize += data.maxDimension;
        minEntitySize = std::min(minEntitySize, data.maxDimension);
    }
    avgEntitySize /= stats.totalEntities;
    
    optimalCellSize = std::max(optimalCellSize, avgEntitySize * 2.0f);
    optimalCellSize = std::max(optimalCellSize, minEntitySize * 1.5f);
    optimalCellSize = std::clamp(optimalCellSize, MIN_CELL_SIZE, MAX_CELL_SIZE);
    
    size_t potentialCells = static_cast<size_t>(
        std::ceil(worldBounds_.w / optimalCellSize) * 
        std::ceil(worldBounds_.h / optimalCellSize)
    );
    
    if (potentialCells > MAX_GRID_CELLS) {
        optimalCellSize = std::sqrt((worldBounds_.w * worldBounds_.h) / MAX_GRID_CELLS) * 1.1f;
    }
    
    if (debugMode_) {
        std::cout << "[SimpleGrid] Optimal cell size calculation:" << std::endl;
        std::cout << "  Current: " << cellSize_ << ", Optimal: " << optimalCellSize << std::endl;
        std::cout << "  Avg entity size: " << avgEntitySize << std::endl;
        std::cout << "  Target entities per cell: " << TARGET_ENTITIES_PER_CELL << std::endl;
    }
    
    return optimalCellSize;
}

void SimpleGrid::OptimizeCellSize() {
    if (!autoOptimize_) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto timeSinceLastOptimize = std::chrono::duration_cast<std::chrono::milliseconds>
        (now - lastOptimizeTime_).count();
    
    if (timeSinceLastOptimize < OPTIMIZE_INTERVAL_MS) {
        return;
    }
    
    float optimalSize = GetOptimalCellSize();
    float sizeDifference = std::abs(optimalSize - cellSize_);
    
    if (sizeDifference > cellSize_ * 0.2f) {
        if (debugMode_) {
            std::cout << "[SimpleGrid] Auto-optimizing cell size from " 
                      << cellSize_ << " to " << optimalSize << std::endl;
        }
        SetCellSize(optimalSize);
        lastOptimizeTime_ = now;
        ResetPerformanceStats();
    } else {
        lastOptimizeTime_ = now;
    }
}

void SimpleGrid::ResetPerformanceStats() {
    queryCount_.store(0);
    totalQueryTime_.store(0);
    lastQueryCount_.store(0);
    
    if (debugMode_) {
        std::cout << "[SimpleGrid] Performance statistics reset" << std::endl;
    }
}

size_t SimpleGrid::GetCellIndex(int gridX, int gridY) const {
    return static_cast<size_t>(gridY) * gridWidth_ + static_cast<size_t>(gridX);
}

void SimpleGrid::WorldToGrid(float worldX, float worldY, int& gridX, int& gridY) const {
    gridX = static_cast<int>((worldX - worldBounds_.x) / cellSize_);
    gridY = static_cast<int>((worldY - worldBounds_.y) / cellSize_);
    
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

bool SimpleGrid::BoundsIntersect(const SDL_FRect& a, const SDL_FRect& b) const {
    return !(a.x > b.x + b.w || b.x > a.x + a.w ||
             a.y > b.y + b.h || b.y > a.y + a.h);
}

float SimpleGrid::CalculateDistance(const SDL_FRect& a, const SDL_FRect& b) const {
    float centerAX = a.x + a.w * 0.5f;
    float centerAY = a.y + a.h * 0.5f;
    float centerBX = b.x + b.w * 0.5f;
    float centerBY = b.y + b.h * 0.5f;
    
    float dx = centerBX - centerAX;
    float dy = centerBY - centerAY;
    return std::sqrt(dx * dx + dy * dy);
}

float SimpleGrid::CalculateMaxDimension(const SDL_FRect& bounds) {
    return std::max(bounds.w, bounds.h);
}

} // namespace engine::ECS