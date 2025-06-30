# Spatial Partitioning System

## Overview

The Spatial Partitioning System provides efficient 2D spatial queries and collision detection optimization. Currently implements two main spatial partitioning data structures:

- **SimpleGrid**: Simple grid system, suitable for uniformly distributed objects
- **QuadTree**: Quadtree system, suitable for dynamic scenes and non-uniformly distributed objects

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
};
```

## QuadTree Implementation

### Features

- **Adaptive Subdivision**: Automatically subdivides when the number of entities in a node exceeds the threshold
- **Smart Merging**: Automatically merges when the number of entities in child nodes decreases
- **Cross-Quadrant Handling**: Supports large entities that span multiple quadrants
- **Debug Support**: Provides detailed tree structure information and statistics

### Constructor Parameters

```cpp
QuadTree(int maxDepth = 8, int maxEntitiesPerNode = 10, const SDL_FRect& worldBounds = {0, 0, 1024, 1024});
```

- `maxDepth`: Maximum depth of the tree
- `maxEntitiesPerNode`: Maximum number of entities per node
- `worldBounds`: World boundaries

### Usage Example

```cpp
#include "engine/core/ecs/spatial/QuadTree.hpp"

// Create QuadTree
SDL_FRect worldBounds = {0, 0, 1000, 1000};
QuadTree quadTree(6, 8, worldBounds);

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

// Remove entity
quadTree.Remove(2);
```

## SimpleGrid Implementation

### Features

- **Fixed Grid**: Uses fixed-size grid cells
- **Fast Queries**: O(1) insertion and deletion operations
- **Memory Efficient**: Suitable for large numbers of uniformly distributed small objects

### Constructor Parameters

```cpp
SimpleGrid(float cellSize, const SDL_FRect& worldBounds);
```

- `cellSize`: Size of grid cells
- `worldBounds`: World boundaries

### Usage Example

```cpp
#include "engine/core/ecs/spatial/SimpleGrid.hpp"

// Create SimpleGrid
SDL_FRect worldBounds = {0, 0, 1000, 1000};
SimpleGrid grid(64.0f, worldBounds);

// Usage is the same as QuadTree
grid.Insert(1, {100, 100, 32, 32});
auto entities = grid.Query({90, 90, 100, 100});
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
```

## Performance Considerations

### QuadTree Use Cases
- Non-uniformly distributed objects
- Objects with varying sizes
- Dynamic scenes (objects move frequently)
- High-precision spatial queries needed

### SimpleGrid Use Cases
- Relatively uniformly distributed objects
- Objects with similar sizes
- Static or semi-static scenes
- Highest performance simple queries needed

## Debug Features

QuadTree provides rich debugging functionality:

```cpp
quadTree.SetDebugMode(true);
quadTree.PrintDebugInfo();

// Get statistics
size_t totalNodes = quadTree.GetTotalNodes();
size_t leafNodes = quadTree.GetLeafNodes();
int actualDepth = quadTree.GetActualMaxDepth();
```

## Integration Testing

Run integration tests to verify functionality:

```cpp
#include "sandbox/testbed/integration/QuadTreeTest.cpp"

// Run all tests
RunQuadTreeTests();
```

## ECS System Integration

The spatial partitioning system can be integrated with the collision detection system to provide efficient collision detection:

```cpp
// Usage in CollisionSystem
class CollisionSystem : public System {
private:
    std::unique_ptr<SpatialPartition> spatialPartition_;
    
public:
    void Init() override {
        SDL_FRect worldBounds = {0, 0, 2000, 2000};
        spatialPartition_ = SpatialPartitionFactory::Create(
            SpatialPartitionFactory::Type::QUAD_TREE, worldBounds);
    }
    
    void Update(float deltaTime) override {
        // Update spatial partitioning structure
        // Use spatial queries for collision detection
    }
};
```

## Future Extensions

- **Octree**: 3D spatial partitioning support
- **BSP Tree**: More complex spatial partitioning
- **R-Tree**: Spatial indexing suitable for irregular shapes
- **Adaptive Grid**: Hybrid solution with dynamic grid size adjustment 