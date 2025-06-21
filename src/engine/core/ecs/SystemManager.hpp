// src/engine/core/ecs/SystemManager.hpp

#pragma once

#include "System.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <algorithm>
#include <iostream>

namespace engine::ECS {

class SystemManager {
public:
    SystemManager() = default;
    ~SystemManager() = default;
    
    void AddSystem(std::unique_ptr<System> system, int priority = 0);
    void RemoveSystem(const std::string& name);
    void Update(float deltaTime);
    void SortSystems();

    void SetSystemPriority(const std::string& name, int priority);
    bool HasSystem(const std::string& name) const;
    System* GetSystem(const std::string& name);

    void PauseSystem(const std::string& name);
    void ResumeSystem(const std::string& name);
    void PauseAllSystems();
    void ResumeAllSystems();

private:
    struct SystemEntry {
        std::unique_ptr<System> system;
        int priority;
        bool isPaused;
        
        SystemEntry(std::unique_ptr<System> sys, int prio) 
            : system(std::move(sys)), priority(prio), isPaused(false) {}
        
        // Make move-only since unique_ptr is move-only
        SystemEntry(SystemEntry&&) = default;
        SystemEntry& operator=(SystemEntry&&) = default;
        SystemEntry(const SystemEntry&) = delete;
        SystemEntry& operator=(const SystemEntry&) = delete;
    };
    
    std::vector<SystemEntry> systems_;
    std::unordered_map<std::string, size_t> systemIndices_;
    bool needsSort_ = false;
};

} // namespace engine::ECS
