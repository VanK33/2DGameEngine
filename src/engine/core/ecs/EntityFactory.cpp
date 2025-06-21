// src/engine/core/ecs/EntityFactory.cpp

#include "EntityFactory.hpp"
#include <algorithm>

namespace engine::ECS {

EntityFactory::EntityFactory() : nextId_(1), totalCreated_(0) {}

EntityID EntityFactory::CreateEntity(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    EntityID id;
    if (!availableIds_.empty()) {
        id = availableIds_.back();
        availableIds_.pop_back();
    } else {
        id = nextId_++;
    }
    
    activeIds_.insert(id);
    totalCreated_++;
    return id;
}

void EntityFactory::DestroyEntity(EntityID id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (activeIds_.find(id) != activeIds_.end()) {
        activeIds_.erase(id);
        availableIds_.push_back(id);
    }
}

bool EntityFactory::IsValid(EntityID id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return activeIds_.find(id) != activeIds_.end();
}

void EntityFactory::ClearAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    activeIds_.clear();
    availableIds_.clear();
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
