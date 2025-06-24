// src/engine/core/ecs/systems/LifetimeSystem.cpp

#include "LifetimeSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/Lifetime.hpp"

namespace engine::ECS {

void LifetimeSystem::Update(float deltaTime) {
    auto* world = GetWorld();

    if (!world) {
        return;
    }

    auto& componentManager = world->GetComponentManager();
    auto& EntityFactory = world->GetEntityFactory();

    auto entities = componentManager.GetEntitiesWithComponent<Lifetime>();

    for (EntityID entityId : entities) {
        auto* lifetime = componentManager.GetComponent<Lifetime>(entityId);
        if (lifetime) {
            lifetime->remainingTime -= deltaTime;

            if (lifetime->remainingTime <= 0.0f) {
                if (lifetime->destroyOnExpire) {
                    EntityFactory.DestroyEntity(entityId);
                } else {
                    componentManager.RemoveComponent<Lifetime>(entityId);
                }
            }
        }
    }
}

} // namespace engine::ECS