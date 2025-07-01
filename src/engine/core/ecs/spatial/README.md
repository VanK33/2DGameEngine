# Spatial Partitioning System

## Overview

The Spatial Partitioning System provides efficient 2D spatial queries and collision detection optimization. Currently implements two main spatial partitioning data structures:

- **SimpleGrid**: Intelligent grid system with auto-optimization, suitable for uniformly distributed objects
- **QuadTree**: Adaptive quadtree system with smart merging, suitable for dynamic scenes and non-uniformly distributed objects

Both implementations are **thread-safe** and include comprehensive performance monitoring and debugging capabilities.

## Class Hierarchy

```
SpatialPartition (Abstract Base Class)
├── SimpleGrid
└── QuadTree
```

## Core Interface

All spatial partitioning implementations inherit from the `SpatialPartition` base class, providing a unified interface:

```cpp
class SpatialPartition {
public:
    virtual void Insert(EntityID entity, const SDL_FRect& bounds) = 0;
    virtual void Update(EntityID entity, const SDL_FRect& bounds) = 0;
    virtual void Remove(EntityID entity) = 0;
    virtual void Clear() = 0;
    
    virtual std::vector<EntityID> Query(const SDL_FRect& area) const = 0;
    virtual std::vector<EntityID> GetNearbyEntities(EntityID entity, float radius) const = 0;
    
    virtual size_t GetEntityCount() const = 0;
    virtual std::string GetImplementationType() const = 0;
    virtual size_t GetLastQueryCount() const = 0;
    virtual void ResetQueryStats() = 0;
    
    virtual void SetDebugMode(bool enabled) = 0;
    virtual bool IsDebugMode() const = 0;
};
```

## QuadTree Implementation

### Features

- **Adaptive Subdivision**: Automatically subdivides when the number of entities in a node exceeds the threshold
- **Smart Merging**: Automatically merges when the number of entities in child nodes decreases
- **Cross-Quadrant Handling**: Supports large entities that span multiple quadrants
- **Thread-Safe Operations**: Safe for concurrent access from multiple threads
- **Debug Support**: Provides detailed tree structure information and statistics
- **Performance Monitoring**: Real-time query performance tracking

### Constructor Parameters

```cpp
QuadTree(int maxDepth = 8, int maxEntitiesPerNode = 10, const SDL_FRect& worldBounds = {0, 0, 1024, 1024});
```

- `maxDepth`: Maximum depth of the tree (default: 8)
- `maxEntitiesPerNode`: Maximum number of entities per node (default: 10)
- `worldBounds`: World boundaries

### Usage Example

```cpp
#include "engine/core/ecs/spatial/QuadTree.hpp"

// Create QuadTree
SDL_FRect worldBounds = {0, 0, 1000, 1000};
QuadTree quadTree(6, 8, worldBounds);

// Enable debug mode for detailed output
quadTree.SetDebugMode(true);

// Insert entities
quadTree.Insert(1, {100, 100, 50, 50});
quadTree.Insert(2, {200, 200, 30, 30});

// Query entities within area
SDL_FRect queryArea = {90, 90, 100, 100};
auto entities = quadTree.Query(queryArea);

// Find nearby entities
auto nearby = quadTree.GetNearbyEntities(1, 100.0f);

// Update entity position
quadTree.Update(1, {120, 120, 50, 50});

// Get performance statistics
quadTree.PrintDebugInfo();

// Remove entity
quadTree.Remove(2);
```

## SimpleGrid Implementation

### Features

- **Intelligent Auto-Optimization**: Automatically adjusts cell size based on entity distribution
- **Thread-Safe Performance Monitoring**: Real-time query performance tracking with atomic operations
- **Memory Protection**: Prevents memory explosion with configurable cell limits
- **Smart Entity Management**: Efficient cross-cell entity handling
- **Production-Ready**: Comprehensive error handling and validation

### Advanced Features

#### Smart Optimization
```cpp
// Auto-optimization parameters (configurable)
static constexpr float TARGET_ENTITIES_PER_CELL = 4.0f;      // Optimal entity density
static constexpr size_t AUTO_OPTIMIZE_QUERY_INTERVAL = 100;  // Check every 100 queries
static constexpr size_t OPTIMIZE_INTERVAL_MS = 1000;         // Max once per second
static constexpr float MIN_CELL_SIZE = 16.0f;                // Minimum cell size
static constexpr float MAX_CELL_SIZE = 512.0f;               // Maximum cell size
static constexpr size_t MAX_GRID_CELLS = 1000000;            // Memory protection
```

#### Performance Statistics
```cpp
struct GridStats {
    size_t totalCells;              // Total number of grid cells
    size_t occupiedCells;           // Cells containing entities
    size_t totalEntities;           // Total entities in grid
    float averageEntitiesPerCell;   // Average entities per occupied cell
    float cellUtilization;          // Percentage of occupied cells
    size_t maxEntitiesInCell;       // Maximum entities in any single cell
    float emptyPercentage;          // Percentage of empty cells
};
```

### Constructor Parameters

```cpp
SimpleGrid(float cellSize, const SDL_FRect& worldBounds);
```

- `cellSize`: Initial size of grid cells (will auto-optimize if enabled)
- `worldBounds`: World boundaries

### Usage Examples

#### Basic Usage
```cpp
#include "engine/core/ecs/spatial/SimpleGrid.hpp"

// Create SimpleGrid
SDL_FRect worldBounds = {0, 0, 1000, 1000};
SimpleGrid grid(64.0f, worldBounds);

// Enable auto-optimization and debug mode
grid.SetAutoOptimize(true);
grid.SetDebugMode(true);

// Insert entities
grid.Insert(1, {100, 100, 32, 32});
grid.Insert(2, {200, 200, 48, 48});

// Query entities
auto entities = grid.Query({90, 90, 100, 100});

// View performance statistics
grid.PrintGridStats();
```

#### Advanced Optimization Usage
```cpp
// Manual optimization
float optimalSize = grid.GetOptimalCellSize();
if (std::abs(optimalSize - grid.GetCellSize()) > 10.0f) {
    std::cout << "Recommended cell size: " << optimalSize << std::endl;
    grid.SetCellSize(optimalSize);
}

// Get detailed statistics
auto stats = grid.GetGridStats();
std::cout << "Cell utilization: " << (stats.cellUtilization * 100) << "%" << std::endl;
std::cout << "Average query time: " << grid.GetAverageQueryTime() << " μs" << std::endl;

// Reset performance counters
grid.ResetPerformanceStats();
```

## Factory Pattern

Use `SpatialPartitionFactory` to create different types of spatial partitioning structures:

```cpp
#include "engine/core/ecs/spatial/SpatialPartition.hpp"

SDL_FRect worldBounds = {0, 0, 1000, 1000};

// Create QuadTree
auto quadTree = SpatialPartitionFactory::Create(
    SpatialPartitionFactory::Type::QUAD_TREE, worldBounds);

// Create SimpleGrid
auto grid = SpatialPartitionFactory::CreateGrid(64.0f, worldBounds);

// Create custom QuadTree
auto customQuadTree = SpatialPartitionFactory::CreateQuadTree(6, 15, worldBounds);

// Create adaptive system (chooses best based on use case)
auto adaptive = SpatialPartitionFactory::Create(
    SpatialPartitionFactory::Type::ADAPTIVE, worldBounds);
```

## Performance Considerations

### QuadTree Use Cases
- **Non-uniformly distributed objects**: Entities clustered in certain areas
- **Objects with varying sizes**: Mix of small and large entities
- **Dynamic scenes**: Objects move frequently and unpredictably
- **High-precision spatial queries**: Need exact spatial relationships
- **Memory efficiency**: Large sparse worlds

**Performance**: O(log n) average query time, adaptive memory usage

### SimpleGrid Use Cases
- **Uniformly distributed objects**: Entities spread evenly across space
- **Objects with similar sizes**: Consistent entity dimensions
- **High-frequency queries**: Many spatial queries per frame
- **Predictable movement patterns**: Grid-based or regular movement
- **Maximum performance**: When memory usage is not a concern

**Performance**: O(1) insertion/deletion, O(k) query time where k is entities per cell

### Performance Comparison

| Metric | SimpleGrid | QuadTree |
|--------|------------|----------|
| **Insert/Remove** | O(1) | O(log n) |
| **Query** | O(k) | O(log n + k) |
| **Memory** | O(cells) | O(nodes) |
| **Best for** | Uniform distribution | Non-uniform distribution |
| **Thread Safety** | ✅ Atomic operations | ✅ Protected operations |
| **Auto-optimization** | ✅ Cell size tuning | ✅ Tree balancing |

## Thread Safety

Both implementations provide thread-safe operations:

### SimpleGrid Thread Safety
```cpp
// Thread-safe performance monitoring
mutable std::atomic<size_t> queryCount_{0};
mutable std::atomic<size_t> totalQueryTime_{0};
mutable std::atomic<size_t> lastQueryCount_{0};
mutable std::mutex statsMutex_;  // Protects complex operations

// Safe concurrent access
grid.SetAutoOptimize(true);  // Atomic operation
auto count = grid.GetQueryCount();  // Atomic read
auto stats = grid.GetGridStats();   // Mutex-protected
```

### QuadTree Thread Safety
- Protected tree modifications
- Safe concurrent queries
- Thread-safe statistics collection

## Debug Features

### SimpleGrid Debug Features
```cpp
grid.SetDebugMode(true);
grid.PrintGridStats();

// Get detailed statistics
auto stats = grid.GetGridStats();
std::cout << "Grid efficiency: " << stats.cellUtilization << std::endl;

// Performance monitoring
std::cout << "Query count: " << grid.GetQueryCount() << std::endl;
std::cout << "Average query time: " << grid.GetAverageQueryTime() << " μs" << std::endl;
```

### QuadTree Debug Features
```cpp
quadTree.SetDebugMode(true);
quadTree.PrintDebugInfo();

// Get tree statistics
size_t totalNodes = quadTree.GetTotalNodes();
size_t leafNodes = quadTree.GetLeafNodes();
int actualDepth = quadTree.GetActualMaxDepth();

std::cout << "Tree efficiency: " << (float)leafNodes / totalNodes << std::endl;
```

## Integration Testing

Comprehensive test suite available:

```cpp
#include "sandbox/testbed/integration/QuadTreeTest.cpp"

// Run all QuadTree tests
RunQuadTreeTests();

// Test spatial partitioning performance
void TestSpatialPerformance() {
    SimpleGrid grid(64.0f, {0, 0, 1000, 1000});
    QuadTree tree(8, 10, {0, 0, 1000, 1000});
    
    // Performance comparison tests
    // Memory usage tests  
    // Thread safety tests
}
```

## ECS System Integration

The spatial partitioning system integrates seamlessly with the collision detection system:

```cpp
// Usage in CollisionSystem
class CollisionSystem : public System {
private:
    std::unique_ptr<SpatialPartition> spatialPartition_;
    
public:
    void Init() override {
        SDL_FRect worldBounds = {0, 0, 2000, 2000};
        
        // Choose based on game type
        if (gameType == "uniform_grid_based") {
            spatialPartition_ = SpatialPartitionFactory::CreateGrid(64.0f, worldBounds);
            static_cast<SimpleGrid*>(spatialPartition_.get())->SetAutoOptimize(true);
        } else {
            spatialPartition_ = SpatialPartitionFactory::CreateQuadTree(8, 10, worldBounds);
        }
        
        spatialPartition_->SetDebugMode(debugMode);
    }
    
    void Update(float deltaTime) override {
        // Update spatial partitioning structure
        spatialPartition_->Clear();
        
        // Insert all entities with colliders
        for (auto entity : entitiesWithColliders) {
            spatialPartition_->Insert(entity, GetEntityBounds(entity));
        }
        
        // Perform spatial collision detection
        for (auto entityA : entitiesWithColliders) {
            auto candidates = spatialPartition_->Query(GetEntityBounds(entityA));
            for (auto entityB : candidates) {
                if (entityA != entityB) {
                    CheckCollision(entityA, entityB);
                }
            }
        }
    }
};
```

## Production Usage Guidelines

### Choosing the Right Implementation

1. **Use SimpleGrid when**:
   - Entities are uniformly distributed
   - Entity sizes are similar
   - High query frequency (>1000 queries/frame)
   - Memory is not a constraint
   - Grid-based games (Tetris, tower defense)

2. **Use QuadTree when**:
   - Entities cluster in certain areas
   - Entity sizes vary significantly
   - Large sparse worlds
   - Memory efficiency is important
   - Open-world games, space simulations

### Performance Tuning

```cpp
// SimpleGrid tuning
grid.SetAutoOptimize(true);                    // Enable auto-optimization
auto optimalSize = grid.GetOptimalCellSize();  // Get recommended size
grid.SetCellSize(optimalSize);                 // Apply optimization

// QuadTree tuning
tree.SetMaxDepth(6);           // Reduce for better performance
tree.SetMaxEntitiesPerNode(15); // Increase for fewer subdivisions
```

### Memory Management

```cpp
// SimpleGrid memory protection
static constexpr size_t MAX_GRID_CELLS = 1000000;  // Prevents memory explosion

// Monitor memory usage
auto stats = grid.GetGridStats();
if (stats.totalCells > 500000) {
    std::cout << "Warning: High memory usage" << std::endl;
    grid.SetCellSize(grid.GetCellSize() * 1.5f);  // Increase cell size
}
```

## Future Extensions

- **Octree**: 3D spatial partitioning support for 3D games
- **BSP Tree**: More complex spatial partitioning for irregular geometries
- **R-Tree**: Spatial indexing suitable for irregular shapes and complex queries
- **Adaptive Hybrid**: Dynamic switching between Grid and QuadTree based on entity distribution
- **GPU Acceleration**: CUDA/OpenCL implementations for massive entity counts
- **Persistent Spatial Indices**: Save/load spatial structures for large worlds 