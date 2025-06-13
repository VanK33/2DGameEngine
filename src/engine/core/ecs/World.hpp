// src/engine/core/ecs/World.hpp

#pragma once

#include "System.hpp"
#include <vector>
#include <memory>

namespace engine {
namespace ECS {

class World {
public:
    using SystemPtr = std::shared_ptr<System>;

    World() = default;
    ~World() = default;

    void AddSystem(SystemPtr system);
    void Update(float deltaTime);
    void Shutdown();

private:
    std::vector<SystemPtr> systems_;
};

} // namespace ECS
} // namespace engine
