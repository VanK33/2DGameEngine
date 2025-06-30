// src/sandbox/testbed/integration/QuadTreeTest.cpp

#include "engine/core/ecs/spatial/QuadTree.hpp"
#include "engine/core/ecs/spatial/SpatialPartition.hpp"
#include <iostream>
#include <cassert>
#include <vector>

using namespace engine::ECS;

void TestQuadTreeBasicOperations() {
    std::cout << "\n=== Testing QuadTree Basic Operations ===" << std::endl;
    
    SDL_FRect worldBounds = {0, 0, 1000, 1000};
    QuadTree quadTree(4, 5, worldBounds); // maxDepth=4, maxEntitiesPerNode=5
    
    // Test insertion
    std::cout << "Testing insertion..." << std::endl;
    quadTree.Insert(1, {100, 100, 50, 50});
    quadTree.Insert(2, {200, 200, 30, 30});
    quadTree.Insert(3, {150, 150, 40, 40});
    
    assert(quadTree.GetEntityCount() == 3);
    std::cout << "✓ Insertion test passed" << std::endl;
    
    // Test query
    std::cout << "Testing query..." << std::endl;
    SDL_FRect queryArea = {90, 90, 100, 100};
    auto results = quadTree.Query(queryArea);
    
    std::cout << "Query results: " << results.size() << " entities found" << std::endl;
    for (auto entity : results) {
        std::cout << "  Entity: " << entity << std::endl;
    }
    
    // Test removal
    std::cout << "Testing removal..." << std::endl;
    quadTree.Remove(2);
    assert(quadTree.GetEntityCount() == 2);
    std::cout << "✓ Removal test passed" << std::endl;
    
    // Test clear
    std::cout << "Testing clear..." << std::endl;
    quadTree.Clear();
    assert(quadTree.GetEntityCount() == 0);
    std::cout << "✓ Clear test passed" << std::endl;
}

void TestQuadTreeSubdivision() {
    std::cout << "\n=== Testing QuadTree Subdivision ===" << std::endl;
    
    SDL_FRect worldBounds = {0, 0, 1000, 1000};
    QuadTree quadTree(3, 2, worldBounds); // maxDepth=3, maxEntitiesPerNode=2
    quadTree.SetDebugMode(true);
    
    // Insert entities to trigger subdivision
    std::cout << "Inserting entities to trigger subdivision..." << std::endl;
    quadTree.Insert(1, {100, 100, 10, 10});
    quadTree.Insert(2, {110, 110, 10, 10});
    quadTree.Insert(3, {120, 120, 10, 10}); // This should trigger subdivision
    
    std::cout << "Total nodes: " << quadTree.GetTotalNodes() << std::endl;
    std::cout << "Leaf nodes: " << quadTree.GetLeafNodes() << std::endl;
    std::cout << "Actual max depth: " << quadTree.GetActualMaxDepth() << std::endl;
    
    quadTree.PrintDebugInfo();
}

void TestQuadTreeNearbyEntities() {
    std::cout << "\n=== Testing QuadTree Nearby Entities ===" << std::endl;
    
    SDL_FRect worldBounds = {0, 0, 1000, 1000};
    QuadTree quadTree(4, 5, worldBounds);
    
    // Insert entities in a pattern
    quadTree.Insert(1, {100, 100, 20, 20}); // Center
    quadTree.Insert(2, {150, 100, 20, 20}); // Right
    quadTree.Insert(3, {100, 150, 20, 20}); // Below
    quadTree.Insert(4, {300, 300, 20, 20}); // Far away
    
    // Find nearby entities
    auto nearby = quadTree.GetNearbyEntities(1, 100.0f);
    
    std::cout << "Entities near entity 1 (radius 100): " << nearby.size() << std::endl;
    for (auto entity : nearby) {
        std::cout << "  Entity: " << entity << std::endl;
    }
    
    // Should find entities 2 and 3, but not 4
    assert(nearby.size() == 2);
    std::cout << "✓ Nearby entities test passed" << std::endl;
}

void TestQuadTreeFactory() {
    std::cout << "\n=== Testing SpatialPartitionFactory ===" << std::endl;
    
    SDL_FRect worldBounds = {0, 0, 1000, 1000};
    
    // Test QuadTree creation through factory
    auto quadTree = SpatialPartitionFactory::Create(SpatialPartitionFactory::Type::QUAD_TREE, worldBounds);
    assert(quadTree != nullptr);
    assert(quadTree->GetImplementationType() == "QuadTree");
    
    // Test basic operations
    quadTree->Insert(1, {100, 100, 50, 50});
    quadTree->Insert(2, {200, 200, 30, 30});
    
    assert(quadTree->GetEntityCount() == 2);
    
    auto results = quadTree->Query({90, 90, 100, 100});
    assert(results.size() > 0);
    
    std::cout << "✓ Factory test passed" << std::endl;
}

void TestQuadTreePerformance() {
    std::cout << "\n=== Testing QuadTree Performance ===" << std::endl;
    
    SDL_FRect worldBounds = {0, 0, 2000, 2000};
    QuadTree quadTree(6, 10, worldBounds);
    
    // Insert many entities
    const int numEntities = 1000;
    std::cout << "Inserting " << numEntities << " entities..." << std::endl;
    
    for (int i = 1; i <= numEntities; ++i) {
        float x = static_cast<float>(rand() % 1900);
        float y = static_cast<float>(rand() % 1900);
        quadTree.Insert(i, {x, y, 20, 20});
    }
    
    std::cout << "Inserted " << quadTree.GetEntityCount() << " entities" << std::endl;
    std::cout << "Total nodes: " << quadTree.GetTotalNodes() << std::endl;
    std::cout << "Leaf nodes: " << quadTree.GetLeafNodes() << std::endl;
    std::cout << "Actual max depth: " << quadTree.GetActualMaxDepth() << std::endl;
    
    // Perform queries
    const int numQueries = 100;
    size_t totalFound = 0;
    
    for (int i = 0; i < numQueries; ++i) {
        float x = static_cast<float>(rand() % 1800);
        float y = static_cast<float>(rand() % 1800);
        SDL_FRect queryArea = {x, y, 100, 100};
        
        auto results = quadTree.Query(queryArea);
        totalFound += results.size();
    }
    
    std::cout << "Performed " << numQueries << " queries" << std::endl;
    std::cout << "Average entities found per query: " << (totalFound / numQueries) << std::endl;
    std::cout << "✓ Performance test completed" << std::endl;
}

void RunQuadTreeTests() {
    std::cout << "Starting QuadTree Integration Tests..." << std::endl;
    
    try {
        TestQuadTreeBasicOperations();
        TestQuadTreeSubdivision();
        TestQuadTreeNearbyEntities();
        TestQuadTreeFactory();
        TestQuadTreePerformance();
        
        std::cout << "\n🎉 All QuadTree tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
    }
}

// Main function for standalone testing
#ifdef QUADTREE_TEST_STANDALONE
int main() {
    RunQuadTreeTests();
    return 0;
}
#endif 