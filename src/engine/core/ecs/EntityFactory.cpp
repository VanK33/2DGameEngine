// src/engine/core/ecs/EntityFactory.cpp

#include "EntityFactory.hpp"
#include <algorithm>

namespace engine::ECS {

EntityFactory::EntityFactory() : nextId_(1), totalCreated_(0) {}

EntityID EntityFactory::CreateEntity(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Use unique IDs - no reuse to avoid component conflicts
    EntityID id = nextId_++;
    
    activeIds_.insert(id);
    totalCreated_++;
    return id;
}

void EntityFactory::DestroyEntity(EntityID id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (activeIds_.find(id) != activeIds_.end()) {
        activeIds_.erase(id);
        // No longer add to availableIds_ - use unique IDs only
    }
}

bool EntityFactory::IsValid(EntityID id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return activeIds_.find(id) != activeIds_.end();
}

void EntityFactory::ClearAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    activeIds_.clear();
    availableIds_.clear();  // Keep for consistency, though no longer used
    nextId_ = 1;
    totalCreated_ = 0;
}

size_t EntityFactory::GetActiveEntityCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return activeIds_.size();
}

size_t EntityFactory::GetTotalCreatedCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return totalCreated_;
}

} // namespace engine::ECS
