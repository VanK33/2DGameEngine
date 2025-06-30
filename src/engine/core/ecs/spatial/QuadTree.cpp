#include "QuadTree.hpp"
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace engine::ECS {

// QuadNode Implementation
QuadTree::QuadNode::QuadNode(const SDL_FRect& bounds, int depth)
    : bounds(bounds), depth(depth), isLeaf(true) {
}

void QuadTree::QuadNode::Subdivide() {
    if (!isLeaf) return; // Already subdivided
    
    float halfWidth = bounds.w * 0.5f;
    float halfHeight = bounds.h * 0.5f;
    float centerX = bounds.x + halfWidth;
    float centerY = bounds.y + halfHeight;
    
    // Create four child nodes
    topLeft = std::make_unique<QuadNode>(
        SDL_FRect{bounds.x, bounds.y, halfWidth, halfHeight}, depth + 1);
    topRight = std::make_unique<QuadNode>(
        SDL_FRect{centerX, bounds.y, halfWidth, halfHeight}, depth + 1);
    bottomLeft = std::make_unique<QuadNode>(
        SDL_FRect{bounds.x, centerY, halfWidth, halfHeight}, depth + 1);
    bottomRight = std::make_unique<QuadNode>(
        SDL_FRect{centerX, centerY, halfWidth, halfHeight}, depth + 1);
    
    isLeaf = false;
}

bool QuadTree::QuadNode::ShouldSubdivide(int maxEntitiesPerNode, int maxDepth) const {
    return isLeaf && 
           static_cast<int>(entities.size()) > maxEntitiesPerNode && 
           depth < maxDepth;
}

void QuadTree::QuadNode::Clear() {
    entities.clear();
    entityBounds.clear();
    
    if (!isLeaf) {
        topLeft->Clear();
        topRight->Clear();
        bottomLeft->Clear();
        bottomRight->Clear();
        
        topLeft.reset();
        topRight.reset();
        bottomLeft.reset();
        bottomRight.reset();
        
        isLeaf = true;
    }
}

size_t QuadTree::QuadNode::GetEntityCount() const {
    size_t count = entities.size();
    
    if (!isLeaf) {
        count += topLeft->GetEntityCount();
        count += topRight->GetEntityCount();
        count += bottomLeft->GetEntityCount();
        count += bottomRight->GetEntityCount();
    }
    
    return count;
}

QuadTree::QuadNode* QuadTree::QuadNode::GetChildForPoint(float x, float y) {
    if (isLeaf) return nullptr;
    
    float centerX = bounds.x + bounds.w * 0.5f;
    float centerY = bounds.y + bounds.h * 0.5f;
    
    if (x < centerX) {
        return (y < centerY) ? topLeft.get() : bottomLeft.get();
    } else {
        return (y < centerY) ? topRight.get() : bottomRight.get();
    }
}

QuadTree::QuadNode* QuadTree::QuadNode::GetChildForBounds(const SDL_FRect& bounds) {
    if (isLeaf) return nullptr;
    
    float centerX = this->bounds.x + this->bounds.w * 0.5f;
    float centerY = this->bounds.y + this->bounds.h * 0.5f;
    
    // Check if bounds can fit entirely in one quadrant
    bool fitsLeft = (bounds.x + bounds.w) <= centerX;
    bool fitsRight = bounds.x >= centerX;
    bool fitsTop = (bounds.y + bounds.h) <= centerY;
    bool fitsBottom = bounds.y >= centerY;
    
    if (fitsLeft && fitsTop) return topLeft.get();
    if (fitsRight && fitsTop) return topRight.get();
    if (fitsLeft && fitsBottom) return bottomLeft.get();
    if (fitsRight && fitsBottom) return bottomRight.get();
    
    return nullptr; // Bounds span multiple quadrants
}

// QuadTree Implementation
QuadTree::QuadTree(int maxDepth, int maxEntitiesPerNode, const SDL_FRect& worldBounds)
    : maxDepth_(maxDepth), maxEntitiesPerNode_(maxEntitiesPerNode), worldBounds_(worldBounds) {
    
    if (maxDepth_ <= 0) {
        std::cerr << "[QuadTree] Warning: maxDepth must be positive, using default 8" << std::endl;
        maxDepth_ = 8;
    }
    
    if (maxEntitiesPerNode_ <= 0) {
        std::cerr << "[QuadTree] Warning: maxEntitiesPerNode must be positive, using default 10" << std::endl;
        maxEntitiesPerNode_ = 10;
    }
    
    root_ = std::make_unique<QuadNode>(worldBounds_, 0);
    
    if (debugMode_) {
        std::cout << "[QuadTree] Created with maxDepth: " << maxDepth_ 
                  << ", maxEntitiesPerNode: " << maxEntitiesPerNode_
                  << ", worldBounds: (" << worldBounds_.x << ", " << worldBounds_.y 
                  << ", " << worldBounds_.w << ", " << worldBounds_.h << ")" << std::endl;
    }
}

void QuadTree::Insert(EntityID entity, const SDL_FRect& bounds) {
    // Remove if already exists
    if (entityBounds_.find(entity) != entityBounds_.end()) {
        Remove(entity);
    }
    
    // Check if bounds fit within world bounds
    if (!BoundsOverlap(bounds, worldBounds_)) {
        if (debugMode_) {
            std::cerr << "[QuadTree] Warning: Entity " << entity 
                      << " bounds are outside world bounds" << std::endl;
        }
        return;
    }
    
    entityBounds_[entity] = bounds;
    InsertIntoNode(root_.get(), entity, bounds);
    
    if (debugMode_) {
        std::cout << "[QuadTree] Inserted entity " << entity << std::endl;
    }
}

void QuadTree::Update(EntityID entity, const SDL_FRect& bounds) {
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
    
    // Remove and re-insert
    Remove(entity);
    Insert(entity, bounds);
}

void QuadTree::Remove(EntityID entity) {
    auto it = entityBounds_.find(entity);
    if (it == entityBounds_.end()) {
        return; // Entity not found
    }
    
    RemoveFromNode(root_.get(), entity);
    entityBounds_.erase(it);
    
    if (debugMode_) {
        std::cout << "[QuadTree] Removed entity " << entity << std::endl;
    }
}

void QuadTree::Clear() {
    root_->Clear();
    entityBounds_.clear();
    
    if (debugMode_) {
        std::cout << "[QuadTree] Cleared tree" << std::endl;
    }
}

std::vector<EntityID> QuadTree::Query(const SDL_FRect& area) const {
    lastQueryCount_ = 0;
    std::vector<EntityID> result;
    QueryNode(root_.get(), area, result);
    return result;
}

std::vector<EntityID> QuadTree::GetNearbyEntities(EntityID entity, float radius) const {
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

size_t QuadTree::GetEntityCount() const {
    return entityBounds_.size();
}

void QuadTree::SetMaxDepth(int maxDepth) {
    if (maxDepth <= 0) {
        std::cerr << "[QuadTree] Warning: maxDepth must be positive" << std::endl;
        return;
    }
    
    // Store current entities
    auto entities = entityBounds_;
    
    // Rebuild tree with new max depth
    maxDepth_ = maxDepth;
    Clear();
    
    // Re-insert all entities
    for (const auto& [entity, bounds] : entities) {
        Insert(entity, bounds);
    }
}

void QuadTree::SetMaxEntitiesPerNode(int maxEntitiesPerNode) {
    if (maxEntitiesPerNode <= 0) {
        std::cerr << "[QuadTree] Warning: maxEntitiesPerNode must be positive" << std::endl;
        return;
    }
    
    // Store current entities
    auto entities = entityBounds_;
    
    // Rebuild tree with new max entities per node
    maxEntitiesPerNode_ = maxEntitiesPerNode;
    Clear();
    
    // Re-insert all entities
    for (const auto& [entity, bounds] : entities) {
        Insert(entity, bounds);
    }
}

size_t QuadTree::GetTotalNodes() const {
    size_t totalNodes = 0, leafNodes = 0;
    CountNodes(root_.get(), totalNodes, leafNodes);
    return totalNodes;
}

size_t QuadTree::GetLeafNodes() const {
    size_t totalNodes = 0, leafNodes = 0;
    CountNodes(root_.get(), totalNodes, leafNodes);
    return leafNodes;
}

int QuadTree::GetActualMaxDepth() const {
    return GetNodeDepth(root_.get());
}

void QuadTree::PrintDebugInfo() const {
    std::cout << "\n=== QuadTree Debug Info ===" << std::endl;
    std::cout << "Max Depth: " << maxDepth_ << std::endl;
    std::cout << "Max Entities Per Node: " << maxEntitiesPerNode_ << std::endl;
    std::cout << "Total Entities: " << GetEntityCount() << std::endl;
    std::cout << "Total Nodes: " << GetTotalNodes() << std::endl;
    std::cout << "Leaf Nodes: " << GetLeafNodes() << std::endl;
    std::cout << "Actual Max Depth: " << GetActualMaxDepth() << std::endl;
    std::cout << "\nTree Structure:" << std::endl;
    PrintNodeInfo(root_.get());
    std::cout << "========================\n" << std::endl;
}

// Private helper methods
void QuadTree::InsertIntoNode(QuadNode* node, EntityID entity, const SDL_FRect& bounds) {
    if (node->isLeaf) {
        node->entities.push_back(entity);
        node->entityBounds[entity] = bounds;
        
        // Check if we need to subdivide
        if (node->ShouldSubdivide(maxEntitiesPerNode_, maxDepth_)) {
            node->Subdivide();
            
            // Redistribute entities to children
            auto entities = node->entities;
            auto entityBounds = node->entityBounds;
            node->entities.clear();
            node->entityBounds.clear();
            
            for (EntityID ent : entities) {
                const SDL_FRect& entBounds = entityBounds[ent];
                QuadNode* child = node->GetChildForBounds(entBounds);
                
                if (child) {
                    InsertIntoNode(child, ent, entBounds);
                } else {
                    // Entity spans multiple quadrants, keep in current node
                    node->entities.push_back(ent);
                    node->entityBounds[ent] = entBounds;
                }
            }
        }
    } else {
        // Try to insert into appropriate child
        QuadNode* child = node->GetChildForBounds(bounds);
        
        if (child) {
            InsertIntoNode(child, entity, bounds);
        } else {
            // Entity spans multiple quadrants, store in current node
            node->entities.push_back(entity);
            node->entityBounds[entity] = bounds;
        }
    }
}

bool QuadTree::RemoveFromNode(QuadNode* node, EntityID entity) {
    // Check if entity is in this node
    auto it = std::find(node->entities.begin(), node->entities.end(), entity);
    if (it != node->entities.end()) {
        node->entities.erase(it);
        node->entityBounds.erase(entity);
        
        // Try to merge children if possible
        if (!node->isLeaf) {
            TryMergeNode(node);
        }
        return true;
    }
    
    // Search in children
    if (!node->isLeaf) {
        bool found = RemoveFromNode(node->topLeft.get(), entity) ||
                     RemoveFromNode(node->topRight.get(), entity) ||
                     RemoveFromNode(node->bottomLeft.get(), entity) ||
                     RemoveFromNode(node->bottomRight.get(), entity);
        
        if (found) {
            TryMergeNode(node);
        }
        return found;
    }
    
    return false;
}

void QuadTree::QueryNode(const QuadNode* node, const SDL_FRect& area, std::vector<EntityID>& result) const {
    if (!BoundsOverlap(node->bounds, area)) {
        return; // No overlap, skip this node
    }
    
    // Check entities in this node
    lastQueryCount_ += node->entities.size();
    for (EntityID entity : node->entities) {
        auto it = node->entityBounds.find(entity);
        if (it != node->entityBounds.end() && BoundsIntersect(area, it->second)) {
            result.push_back(entity);
        }
    }
    
    // Recursively query children
    if (!node->isLeaf) {
        QueryNode(node->topLeft.get(), area, result);
        QueryNode(node->topRight.get(), area, result);
        QueryNode(node->bottomLeft.get(), area, result);
        QueryNode(node->bottomRight.get(), area, result);
    }
}

void QuadTree::TryMergeNode(QuadNode* node) {
    if (node->isLeaf || !CanMergeChildren(node)) {
        return;
    }
    
    MergeChildren(node);
}

bool QuadTree::CanMergeChildren(const QuadNode* node) const {
    if (node->isLeaf) return false;
    
    // Check if all children are leaves and total entity count is small enough
    if (!node->topLeft->isLeaf || !node->topRight->isLeaf ||
        !node->bottomLeft->isLeaf || !node->bottomRight->isLeaf) {
        return false;
    }
    
    size_t totalEntities = node->entities.size() +
                          node->topLeft->entities.size() +
                          node->topRight->entities.size() +
                          node->bottomLeft->entities.size() +
                          node->bottomRight->entities.size();
    
    return static_cast<int>(totalEntities) <= maxEntitiesPerNode_;
}

void QuadTree::MergeChildren(QuadNode* node) {
    if (node->isLeaf) return;
    
    // Collect all entities from children
    std::vector<QuadNode*> children = {
        node->topLeft.get(), node->topRight.get(),
        node->bottomLeft.get(), node->bottomRight.get()
    };
    
    for (QuadNode* child : children) {
        for (EntityID entity : child->entities) {
            node->entities.push_back(entity);
            node->entityBounds[entity] = child->entityBounds[entity];
        }
    }
    
    // Remove children
    node->topLeft.reset();
    node->topRight.reset();
    node->bottomLeft.reset();
    node->bottomRight.reset();
    node->isLeaf = true;
}

void QuadTree::CountNodes(const QuadNode* node, size_t& totalNodes, size_t& leafNodes) const {
    totalNodes++;
    
    if (node->isLeaf) {
        leafNodes++;
    } else {
        CountNodes(node->topLeft.get(), totalNodes, leafNodes);
        CountNodes(node->topRight.get(), totalNodes, leafNodes);
        CountNodes(node->bottomLeft.get(), totalNodes, leafNodes);
        CountNodes(node->bottomRight.get(), totalNodes, leafNodes);
    }
}

int QuadTree::GetNodeDepth(const QuadNode* node) const {
    if (node->isLeaf) {
        return node->depth;
    }
    
    int maxChildDepth = std::max({
        GetNodeDepth(node->topLeft.get()),
        GetNodeDepth(node->topRight.get()),
        GetNodeDepth(node->bottomLeft.get()),
        GetNodeDepth(node->bottomRight.get())
    });
    
    return maxChildDepth;
}

void QuadTree::PrintNodeInfo(const QuadNode* node, int indent) const {
    std::string indentStr(indent * 2, ' ');
    
    std::cout << indentStr << "Node[" << node->depth << "] ";
    std::cout << "(" << node->bounds.x << ", " << node->bounds.y 
              << ", " << node->bounds.w << ", " << node->bounds.h << ") ";
    std::cout << "Entities: " << node->entities.size();
    
    if (node->isLeaf) {
        std::cout << " [LEAF]";
    }
    std::cout << std::endl;
    
    if (!node->isLeaf) {
        PrintNodeInfo(node->topLeft.get(), indent + 1);
        PrintNodeInfo(node->topRight.get(), indent + 1);
        PrintNodeInfo(node->bottomLeft.get(), indent + 1);
        PrintNodeInfo(node->bottomRight.get(), indent + 1);
    }
}

bool QuadTree::BoundsContain(const SDL_FRect& container, const SDL_FRect& contained) const {
    return contained.x >= container.x &&
           contained.y >= container.y &&
           (contained.x + contained.w) <= (container.x + container.w) &&
           (contained.y + contained.h) <= (container.y + container.h);
}

bool QuadTree::BoundsOverlap(const SDL_FRect& a, const SDL_FRect& b) const {
    return BoundsIntersect(a, b);
}

} // namespace engine::ECS
