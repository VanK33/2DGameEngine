// src/ecs/System.hpp

#pragma once

namespace engine::ECS {

class System {
public:
    virtual ~System() = default;

    virtual void Init() {}

    virtual void Shutdown() {}

    virtual void Update(float deltaTime) = 0;

    void SetEnabled(bool enabled) { enabled_ = enabled; }
    bool IsEnabled() const { return enabled_; }

    virtual const char* GetName() const { return "UnnamedSystem"; }

protected:
    bool enabled_ = true;
};

} // namespace engine::ECS