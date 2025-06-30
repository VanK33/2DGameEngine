#pragma once

#include "SpatialPartition.hpp"
#include <unordered_map>
#include <vector>
#include <memory>

namespace engine::ECS {

class QuadTree : public SpatialPartition {
private:
    struct QuadNode {
        SDL_FRect bounds;
        std::vector<EntityID> entities;
        std::unordered_map<EntityID, SDL_FRect> entityBounds;
        
        // Children nodes (nullptr if leaf)
        std::unique_ptr<QuadNode> topLeft;
        std::unique_ptr<QuadNode> topRight;
        std::unique_ptr<QuadNode> bottomLeft;
        std::unique_ptr<QuadNode> bottomRight;
        
        int depth;
        bool isLeaf;
        
        QuadNode(const SDL_FRect& bounds, int depth);
        ~QuadNode() = default;
        
        bool IsLeaf() const { return isLeaf; }
        void Subdivide();
        bool ShouldSubdivide(int maxEntitiesPerNode, int maxDepth) const;
        void Clear();
        size_t GetEntityCount() const;
        
        // Helper to determine which child node contains a point
        QuadNode* GetChildForPoint(float x, float y);
        QuadNode* GetChildForBounds(const SDL_FRect& bounds);
    };

public:
    explicit QuadTree(int maxDepth = 8, int maxEntitiesPerNode = 10, const SDL_FRect& worldBounds = {0, 0, 1024, 1024});
    virtual ~QuadTree() = default;

    void Insert(EntityID entity, const SDL_FRect& bounds) override;
    void Update(EntityID entity, const SDL_FRect& bounds) override;
    void Remove(EntityID entity) override;
    void Clear() override;
    
    std::vector<EntityID> Query(const SDL_FRect& area) const override;
    std::vector<EntityID> GetNearbyEntities(EntityID entity, float radius) const override;
    size_t GetEntityCount() const override;
    std::string GetImplementationType() const override { return "QuadTree"; }

    // QuadTree specific methods
    int GetMaxDepth() const { return maxDepth_; }
    int GetMaxEntitiesPerNode() const { return maxEntitiesPerNode_; }
    SDL_FRect GetWorldBounds() const { return worldBounds_; }
    
    void SetMaxDepth(int maxDepth);
    void SetMaxEntitiesPerNode(int maxEntitiesPerNode);
    
    // Debug information
    size_t GetTotalNodes() const;
    size_t GetLeafNodes() const;
    int GetActualMaxDepth() const;
    void PrintDebugInfo() const;

private:
    int maxDepth_;
    int maxEntitiesPerNode_;
    SDL_FRect worldBounds_;
    std::unique_ptr<QuadNode> root_;
    
    // Entity tracking
    std::unordered_map<EntityID, SDL_FRect> entityBounds_;
    
    // Helper methods
    void InsertIntoNode(QuadNode* node, EntityID entity, const SDL_FRect& bounds);
    bool RemoveFromNode(QuadNode* node, EntityID entity);
    void QueryNode(const QuadNode* node, const SDL_FRect& area, std::vector<EntityID>& result) const;
    
    // Tree maintenance
    void TryMergeNode(QuadNode* node);
    bool CanMergeChildren(const QuadNode* node) const;
    void MergeChildren(QuadNode* node);
    
    // Debug helpers
    void CountNodes(const QuadNode* node, size_t& totalNodes, size_t& leafNodes) const;
    int GetNodeDepth(const QuadNode* node) const;
    void PrintNodeInfo(const QuadNode* node, int indent = 0) const;
    
    // Bounds checking
    bool BoundsContain(const SDL_FRect& container, const SDL_FRect& contained) const;
    bool BoundsOverlap(const SDL_FRect& a, const SDL_FRect& b) const;
};

} // namespace engine::ECS
