// src/engine/core/ecs/World.hpp

#pragma once

#include "System.hpp"
#include "EntityFactory.hpp"
#include "ComponentManager.hpp"
#include "SystemManager.hpp"
#include "WorldState.hpp"
#include "engine/core/event/EventManager.hpp"
#include <vector>
#include <memory>

namespace engine::ECS {

class World {
public:
    World() = default;
    ~World() = default;

    // ECS Integration
    EntityFactory& GetEntityFactory() { return entityFactory_; }
    ComponentManager& GetComponentManager() { return componentManager_; }
    SystemManager& GetSystemManager() { return systemManager_; }
    
    // EventSystem
    engine::event::EventManager& GetEventManager() { return eventManager_; }
    
    // Entity management
    void ClearAllEntities();
    size_t GetEntityCount() const;
    bool HasEntity(EntityID id) const;
    
    // Scene management
    void Pause() { worldState_.SetPaused(true); }
    void Resume() { worldState_.SetPaused(false); }
    bool IsPaused() const { return worldState_.IsPaused(); }

    void Update(float deltaTime);

private:
    EntityFactory entityFactory_;
    ComponentManager componentManager_;
    SystemManager systemManager_;
    WorldState worldState_;
    engine::event::EventManager eventManager_;
};

} // namespace engine::ECS
