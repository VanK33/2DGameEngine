// src/engine/core/ecs/System.hpp

#pragma once

namespace engine::ECS {

class World; // forward declaration    
class System {
public:
    virtual ~System() = default;

    virtual void Init() {}

    virtual void Shutdown() {}

    virtual void Update(float deltaTime) = 0;

    void SetEnabled(bool enabled) { enabled_ = enabled; }
    bool IsEnabled() const { return enabled_; }

    virtual const char* GetName() const { return "UnnamedSystem"; }

    void SetWorld(World* world) { world_ = world; }
    World* GetWorld() const { return world_; }

protected:
    bool enabled_ = true;
    World* world_ = nullptr;
};

} // namespace engine::ECS