// src/engine/core/ecs/World.cpp
#include "World.hpp"

namespace engine::ECS {

void World::AddSystem(SystemPtr system) {
    systems_.push_back(system);
    system->Init();
}

void World::Update(float deltaTime) {
    for (const auto& system : systems_) {
        if (system->IsEnabled()) {
            system->Update(deltaTime);
        }
    }
}

void World::Shutdown() {
    for (auto& system : systems_) {
        system->Shutdown();
    }
}

} // namespace engine::ECS

