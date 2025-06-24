// src /engine/core/ecs/systems/LifetimeSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"

namespace engine::ECS {

class LifetimeSystem : public System {

public:
    void Update(float deltaTime) override;
    const char* GetName() const override { return "LifetimeSystem"; }

};
} // namespace engine::ECS