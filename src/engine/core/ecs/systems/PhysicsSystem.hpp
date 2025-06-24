// src/engine/core/ecs/systems/PhysicsSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "engine/core/ecs/components/PhysicsMode.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/event/Event.hpp"
#include "engine/core/event/EventListener.hpp"
#include <unordered_map>

namespace engine::ECS {

using EntityID = uint32_t;

class PhysicsSystem : public System, public engine::event::EventListener {
public:
    void Init() override;
    void Update(float deltaTime) override;
    const char* GetName() const override { return "PhysicsSystem"; }
    
    // EventListener interface
    void onEvent(const std::shared_ptr<engine::event::Event>& event) override;

private:
    void ApplyGravity(Velocity2D* velocity, const PhysicsModeComponent* mode, float deltaTime);
    void LimitVelocity(Velocity2D* velocity);
    void ApplyFriction(Velocity2D* velocity, float frictionFactor);
    void CheckBoundaries(Transform2D* transform, Velocity2D* velocity);
    bool HasCollision(Transform2D* transform);
    void HandleCollision(Transform2D* transform, Velocity2D* velocity, float oldX, float oldY);
    
    void HandleCollisionEvent(const engine::event::Event& event);
    void HandleCollisionResponse(EntityID entityA, EntityID entityB);
    
    std::unordered_map<EntityID, std::pair<float, float>> lastPositions_;
};

} // namespace engine::ECS