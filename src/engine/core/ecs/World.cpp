// src/engine/core/ecs/World.cpp
#include "World.hpp"

namespace engine::ECS {

void World::Update(float deltaTime) {
    if (!IsPaused()) {
        systemManager_.Update(deltaTime);
    }
}

void World::ClearAllEntities() {
    entityFactory_.ClearAll();
}

size_t World::GetEntityCount() const {
    return entityFactory_.GetActiveEntityCount();
}

bool World::HasEntity(EntityID id) const {
    return entityFactory_.IsValid(id);
}

} // namespace engine::ECS

