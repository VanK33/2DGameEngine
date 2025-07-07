// src/examples/zombie_survivor/ecs/systems/MovementSystem.hpp
#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "examples/zombie_survivor/ecs/components/InputComponent.hpp"
#include "examples/zombie_survivor/ecs/components/MovementComponent.hpp"


namespace ZombieSurvivor::System {

class MovementSystem : public engine::ECS::System {

public:
    MovementSystem() = default;
    ~MovementSystem() = default;

    void Init() override;
    void Update(float deltaTime) override;
    const char* GetName() const override { return "MovementSystem"; }

private:
    void ProcessMovement(
        const ZombieSurvivor::Component::InputComponent* input,
        const ZombieSurvivor::Component::MovementComponent* movement,
        engine::ECS::Velocity2D* velocity,
        float deltaTime
    ) const;

    void ApplyAcceleration(
        const engine::Vector2& inputDirection,
        float targetSpeed,
        float accerlation,
        engine::ECS::Velocity2D* velocity,
        float deltaTime
    ) const;

    void ApplyFriction(
        engine::ECS::Velocity2D* velocity,
        float deceleration,
        float deltaTime
    ) const;

    bool HasInput(const engine::Vector2& input) const;
};

} // namespace ZombieSurvivor::System