// src/examples/zombie_survivor/ecs/systems/BoundarySystem.hpp
#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "examples/zombie_survivor/ecs/components/BoundaryComponent.hpp"

namespace ZombieSurvivor::System {

class BoundarySystem : public engine::ECS::System {
public:
    BoundarySystem() = default;
    ~BoundarySystem() = default;

    void Init() override;
    void Update(float deltaTime) override;
    const char* GetName() const override { return "BoundarySystem"; }

private:
    void ApplyBoundaryConstraints(
        engine::ECS::Transform2D* transform,
        engine::ECS::Velocity2D* velocity,
        const Component::BoundaryComponent* boundary,
        float deltaTime
    ) const;
    
    void ApplyScreenBounds(
        engine::ECS::Transform2D* transform,
        engine::ECS::Velocity2D* velocity,
        float entitySize,
        float deltaTime
    ) const;
    
    void ApplyCustomBounds(
        engine::ECS::Transform2D* transform,
        engine::ECS::Velocity2D* velocity,
        const Component::BoundaryComponent* boundary,
        float deltaTime
    ) const;
};

} // namespace ZombieSurvivor::System