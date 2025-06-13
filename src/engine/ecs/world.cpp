// src/ecs/World.hpp

#pragma once

#include "System.hpp"
#include <vector>
#include <memory>

namespace engine {
namespace ECS {

class World {
public:
    using SystemPtr = std::shared_ptr<System>;

    void AddSystem(SystemPtr system) {
        systems_.push_back(system);
        system->Init();
    }

    void Update(float deltaTime) {
        for (const auto& system : systems_) {
            if (system->IsEnabled()) {
                system->Update(deltaTime);
            }
        }
    }

    void Shutdown() {
        for (auto& system : systems_) {
            system->Shutdown();
        }
    }

private:
    std::vector<SystemPtr> systems_;
};

} // namespace ECS
} // namespace engine
