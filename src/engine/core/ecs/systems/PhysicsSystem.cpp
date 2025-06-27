// src/engine/core/ecs/systems/PhysicsSystem.cpp

#include "PhysicsSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "engine/core/ecs/components/PhysicsMode.hpp"
#include "engine/core/ecs/components/Collider2D.hpp"
#include "engine/core/event/EventManager.hpp"
#include "engine/core/event/events/PhysicsEvents.hpp"
#include <cmath>

namespace engine::ECS {

void PhysicsSystem::Init() {
    if (auto* world = GetWorld()) {
        auto& eventManager = world->GetEventManager(); // Here we get event manager, maybe InputManager can use this pattern too to avoid direct assignment in world?
        eventManager.Subscribe(engine::event::EventType::COLLISION_STARTED, this);
    }
}

void PhysicsSystem::Update(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto entities = componentManager.GetEntitiesWithComponents<Transform2D, Velocity2D>();
    
    // Use references to avoid unnecessary copying
    for (const EntityID& entityId : entities) {
        auto* transform = componentManager.GetComponent<Transform2D>(entityId);
        auto* velocity = componentManager.GetComponent<Velocity2D>(entityId);
        auto* physicsMode = componentManager.GetComponent<PhysicsModeComponent>(entityId);
        
        if (!transform || !velocity || !physicsMode) continue;

        // Apply physics updates
        if (physicsMode->enableGravity) {
            ApplyGravity(velocity, physicsMode, deltaTime);
        }

        if (physicsMode->enableFriction) {
            ApplyFriction(velocity, physicsMode, deltaTime);
        }

        LimitVelocity(velocity);

        transform->x += velocity->vx * deltaTime;
        transform->y += velocity->vy * deltaTime;
    }
}

void PhysicsSystem::ApplyGravity(Velocity2D* velocity, const PhysicsModeComponent* mode, float deltaTime) {
    switch (mode->mode) {
        case PhysicsMode::TOP_DOWN:
            velocity->vx += mode->gravityX * deltaTime;
            velocity->vy += mode->gravityY * deltaTime;
            break;
            
        case PhysicsMode::SIDE_VIEW:
            velocity->vy += mode->gravityY * deltaTime;
            break;
            
        case PhysicsMode::ISOMETRIC:
            velocity->vx += mode->gravityX * deltaTime;
            velocity->vy += mode->gravityY * deltaTime;
            break;
            
        case PhysicsMode::CUSTOM:
            velocity->vx += mode->gravityX * deltaTime;
            velocity->vy += mode->gravityY * deltaTime;
            break;
    }
}

void PhysicsSystem::LimitVelocity(Velocity2D* velocity) {
    float currentSpeed = std::sqrt(velocity->vx * velocity->vx + velocity->vy * velocity->vy);
    if (currentSpeed > velocity->maxSpeed) {
        float scale = velocity->maxSpeed / currentSpeed;
        velocity->vx *= scale;
        velocity->vy *= scale;
    }
}

void PhysicsSystem::ApplyFriction(Velocity2D* velocity, const PhysicsModeComponent* mode, float deltaTime) {
    if (!mode || !mode->enableFriction) return;
    
    velocity->vx *= std::pow(mode->frictionFactor, deltaTime);
    velocity->vy *= std::pow(mode->frictionFactor, deltaTime);
    
    const float minSpeed = 0.1f;
    if (std::abs(velocity->vx) < minSpeed) velocity->vx = 0;
    if (std::abs(velocity->vy) < minSpeed) velocity->vy = 0;
}

void PhysicsSystem::onEvent(const std::shared_ptr<engine::event::Event>& event) {
    if (event->GetType() == engine::event::EventType::COLLISION_STARTED) {
        HandleCollisionEvent(*event);
    }
}

void PhysicsSystem::HandleCollisionEvent(const engine::event::Event& event) {
    auto collisionEvent = static_cast<const engine::event::CollisionEvent&>(event);

    EntityID entityA = collisionEvent.GetEntityA();
    EntityID entityB = collisionEvent.GetEntityB();

    auto groupA = entityCollisionGroups_.find(entityA);
    auto groupB = entityCollisionGroups_.find(entityB);

    if (groupA != entityCollisionGroups_.end() && groupB != entityCollisionGroups_.end()) {
        std::string collisionGroup = groupA->second + "_" + groupB->second;
        
        auto callback = collisionCallbacks_.find(collisionGroup);
        if (callback != collisionCallbacks_.end()) {
            CollisionInfo info{entityA, entityB};
            callback->second(info);
        }
    }
}

void PhysicsSystem::RegisterCollisionCallback(const std::string& collisionGroup, CollisionResponseCallBack callback) {
    collisionCallbacks_[collisionGroup] = std::move(callback);
}

void PhysicsSystem::SetCollisionGroup(EntityID entity, const std::string& group) {
    entityCollisionGroups_[entity] = group;
}

PhysicsSystem::~PhysicsSystem() {
    if (auto* world = GetWorld()) {
        auto& eventManager = world->GetEventManager();
        eventManager.Unsubscribe(engine::event::EventType::COLLISION_STARTED, this);
    }
}

} // namespace engine::ECS