// src/engine/core/ecs/systems/PhysicsSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "engine/core/ecs/components/PhysicsMode.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/event/Event.hpp"
#include "engine/core/event/EventListener.hpp"
#include <unordered_map>
#include <functional>

namespace engine::ECS {

using EntityID = uint32_t;

struct CollisionInfo {
    EntityID entityA;
    EntityID entityB;
};

class PhysicsSystem : public System, public engine::event::EventListener {
public:

    using CollisionResponseCallBack = std::function<void(const CollisionInfo&)>;

    void Init() override;
    void Update(float deltaTime) override;
    const char* GetName() const override { return "PhysicsSystem"; }
    
    // EventListener interface
    void onEvent(const std::shared_ptr<engine::event::Event>& event) override;

    void RegisterCollisionCallback(const std::string& collisionGroup, CollisionResponseCallBack callback);

    void SetCollisionGroup(EntityID entity, const std::string& group);

    ~PhysicsSystem() override;

private:
    void ApplyGravity(Velocity2D* velocity, const PhysicsModeComponent* mode, float deltaTime);
    void LimitVelocity(Velocity2D* velocity);
    void ApplyFriction(Velocity2D* velocity, const PhysicsModeComponent* mode, float deltaTime);
    void CheckBoundaries(Transform2D* transform, Velocity2D* velocity);
    
    void HandleCollisionEvent(const engine::event::Event& event);
    
    std::unordered_map<std::string, CollisionResponseCallBack> collisionCallbacks_;
    std::unordered_map<EntityID, std::string> entityCollisionGroups_;
};

} // namespace engine::ECS