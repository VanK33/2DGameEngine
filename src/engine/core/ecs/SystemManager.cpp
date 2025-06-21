// src/engine/core/ecs/SystemManager.cpp
#include "SystemManager.hpp"
#include <iostream>

namespace engine::ECS {

void SystemManager::AddSystem(std::unique_ptr<System> system, int priority) {
    if (!system) {
        std::cerr << "[SystemManager] Warning: Attempted to add null system" << std::endl;
        return;
    }
    
    std::string name = system->GetName();
    
    // Check if system already exists
    if (HasSystem(name)) {
        std::cerr << "[SystemManager] Warning: System '" << name << "' already exists" << std::endl;
        return;
    }
    
    // Add system
    systems_.emplace_back(std::move(system), priority);
    systemIndices_[name] = systems_.size() - 1;
    needsSort_ = true;
    
    std::cout << "[SystemManager] Added system: " << name << " (priority: " << priority << ")" << std::endl;
}

void SystemManager::RemoveSystem(const std::string& name) {
    auto it = systemIndices_.find(name);
    if (it == systemIndices_.end()) {
        std::cerr << "[SystemManager] Warning: System '" << name << "' not found" << std::endl;
        return;
    }
    
    size_t index = it->second;
    systems_.erase(systems_.begin() + index);
    
    // Update indices
    systemIndices_.erase(it);
    for (auto& [sysName, sysIndex] : systemIndices_) {
        if (sysIndex > index) {
            sysIndex--;
        }
    }
    
    std::cout << "[SystemManager] Removed system: " << name << std::endl;
}

void SystemManager::Update(float deltaTime) {
    if (needsSort_) {
        SortSystems();
    }
    
    for (auto& entry : systems_) {
        if (!entry.isPaused && entry.system) {
            entry.system->Update(deltaTime);
        }
    }
}

void SystemManager::SetSystemPriority(const std::string& name, int priority) {
    auto it = systemIndices_.find(name);
    if (it == systemIndices_.end()) {
        std::cerr << "[SystemManager] Warning: System '" << name << "' not found" << std::endl;
        return;
    }
    
    systems_[it->second].priority = priority;
    needsSort_ = true;
    
    std::cout << "[SystemManager] Set priority for '" << name << "' to " << priority << std::endl;
}

void SystemManager::SortSystems() {
    std::sort(systems_.begin(), systems_.end(),
        [](const SystemEntry& a, const SystemEntry& b) {
            return a.priority < b.priority;
        });
    
    // Update indices after sorting
    systemIndices_.clear();
    for (size_t i = 0; i < systems_.size(); ++i) {
        if (systems_[i].system) {
            systemIndices_[systems_[i].system->GetName()] = i;
        }
    }
    
    needsSort_ = false;
}

bool SystemManager::HasSystem(const std::string& name) const {
    return systemIndices_.find(name) != systemIndices_.end();
}

System* SystemManager::GetSystem(const std::string& name) {
    auto it = systemIndices_.find(name);
    if (it != systemIndices_.end()) {
        return systems_[it->second].system.get();
    }
    return nullptr;
}

void SystemManager::PauseSystem(const std::string& name) {
    auto it = systemIndices_.find(name);
    if (it != systemIndices_.end()) {
        systems_[it->second].isPaused = true;
        std::cout << "[SystemManager] Paused system: " << name << std::endl;
    }
}

void SystemManager::ResumeSystem(const std::string& name) {
    auto it = systemIndices_.find(name);
    if (it != systemIndices_.end()) {
        systems_[it->second].isPaused = false;
        std::cout << "[SystemManager] Resumed system: " << name << std::endl;
    }
}

void SystemManager::PauseAllSystems() {
    for (auto& entry : systems_) {
        entry.isPaused = true;
    }
    std::cout << "[SystemManager] Paused all systems" << std::endl;
}

void SystemManager::ResumeAllSystems() {
    for (auto& entry : systems_) {
        entry.isPaused = false;
    }
    std::cout << "[SystemManager] Resumed all systems" << std::endl;
}

} // namespace engine::ECS