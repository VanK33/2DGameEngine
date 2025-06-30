#include "SpatialPartition.hpp"
#include "SimpleGrid.hpp"
#include "QuadTree.hpp"
#include <iostream>

namespace engine::ECS {

std::unique_ptr<SpatialPartition> SpatialPartitionFactory::Create(Type type, const SDL_FRect& worldBounds) {
    switch (type) {
        case Type::SIMPLE_GRID:
            return CreateGrid(64.0f, worldBounds); // Default cell size
            
        case Type::QUAD_TREE:
            return CreateQuadTree(8, 10, worldBounds); // Default values
            
        case Type::ADAPTIVE:
            // For now, use QuadTree as adaptive implementation
            std::cout << "[SpatialPartitionFactory] ADAPTIVE type not fully implemented, using QuadTree" << std::endl;
            return CreateQuadTree(8, 10, worldBounds);
            
        default:
            std::cerr << "[SpatialPartitionFactory] Unknown type, defaulting to SimpleGrid" << std::endl;
            return CreateGrid(64.0f, worldBounds);
    }
}

std::unique_ptr<SpatialPartition> SpatialPartitionFactory::CreateGrid(float cellSize, const SDL_FRect& worldBounds) {
    return std::make_unique<SimpleGrid>(cellSize, worldBounds);
}

std::unique_ptr<SpatialPartition> SpatialPartitionFactory::CreateQuadTree(int maxDepth, int maxEntitiesPerNode, const SDL_FRect& worldBounds) {
    return std::make_unique<QuadTree>(maxDepth, maxEntitiesPerNode, worldBounds);
}

} // namespace engine::ECS 