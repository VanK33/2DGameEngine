#pragma once

#include <vector>
#include <unordered_set>
#include <string>
#include <mutex>

namespace engine::ECS {

using EntityID = uint32_t;

class EntityFactory {
public:
    EntityFactory();
    ~EntityFactory() = default;

    // Core functionality
    EntityID CreateEntity(const std::string& name = "");
    void DestroyEntity(EntityID id);
    bool IsValid(EntityID id) const;
    void ClearAll();
    
    // Utility
    size_t GetActiveEntityCount() const;
    size_t GetTotalCreatedCount() const;

private:
    std::vector<EntityID> availableIds_;
    std::unordered_set<EntityID> activeIds_;
    EntityID nextId_ = 1;
    size_t totalCreated_ = 0;
    mutable std::mutex mutex_;
};

} // namespace engine::ECS