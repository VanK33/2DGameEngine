// src/examples/zombie_survivor/ecs/systems/MovementSystem.cpp

#include "MovementSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include <cmath>

namespace ZombieSurvivor::System {

void MovementSystem::Init() {

}

void MovementSystem::Update(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();

    auto entities = componentManager.GetEntitiesWithComponents<
        ZombieSurvivor::Component::InputComponent,
        ZombieSurvivor::Component::MovementComponent,
        engine::ECS::Velocity2D
    >();

    for (const auto& entityId : entities) {
        auto* input = componentManager.GetComponent<ZombieSurvivor::Component::InputComponent>(entityId);
        auto* movement = componentManager.GetComponent<ZombieSurvivor::Component::MovementComponent>(entityId);
        auto* velocity = componentManager.GetComponent<engine::ECS::Velocity2D>(entityId);

        if (!input || !movement || !velocity) continue;

        ProcessMovement(input, movement, velocity, deltaTime);
    }
}

void MovementSystem::ProcessMovement(
    const ZombieSurvivor::Component::InputComponent* input,
    const ZombieSurvivor::Component::MovementComponent* movement,
    engine::ECS::Velocity2D* velocity,
    float deltaTime
) const {
    if (!movement->canMove) {
        ApplyFriction(velocity, movement->acceleration * 2.0f, deltaTime);
        return;
    }

    if (HasInput(input->moveInput)) {
        if (movement->instantResponse) {
            velocity->vx = input->moveInput.x * movement->speed;
            velocity->vy = input->moveInput.y * movement->speed;
        } else {
            float targetSpeed = movement->speed;
            ApplyAcceleration(input->moveInput, targetSpeed, movement->acceleration, velocity, deltaTime);
        }
    } else {
        if (movement->instantResponse) {
            velocity->vx = 0.0f;
            velocity->vy = 0.0f;
        } else {
            ApplyFriction(velocity, movement->acceleration, deltaTime);
        }
    }

    float speedSquared = velocity->vx * velocity->vx + velocity->vy * velocity->vy;
    float maxSpeedSquared = velocity->maxSpeed * velocity->maxSpeed;

    if (speedSquared > maxSpeedSquared) {
        float currentSpeed = std::sqrt(speedSquared);
        float scale = velocity->maxSpeed / currentSpeed;
        velocity->vx *= scale;
        velocity->vy *= scale;
    }
}

void MovementSystem::ApplyAcceleration(
    const engine::Vector2& inputDirection,
    float targetSpeed,
    float acceleration,
    engine::ECS::Velocity2D* velocity,
    float deltaTime
) const {
    engine::Vector2 targetVelocity{
        inputDirection.x * targetSpeed,
        inputDirection.y * targetSpeed
    };

    engine::Vector2 currentVelocity{velocity->vx, velocity->vy};

    engine::Vector2 velocityDiff{
        targetVelocity.x - currentVelocity.x,
        targetVelocity.y - currentVelocity.y
    };

    float maxSpeedChange = acceleration * deltaTime;
    float diffLength = std::sqrt(velocityDiff.x * velocityDiff.x + velocityDiff.y * velocityDiff.y);


    if (diffLength > maxSpeedChange) {
        float scale = maxSpeedChange / diffLength;
        velocityDiff.x *= scale;
        velocityDiff.y *= scale;
    }

    velocity->vx += velocityDiff.x;
    velocity->vy += velocityDiff.y;
}

void MovementSystem::ApplyFriction(
    engine::ECS::Velocity2D* velocity,
    float deceleration,
    float deltaTime
) const {
    float currentSpeed = std::sqrt(velocity->vx * velocity->vx + velocity->vy * velocity->vy);

    if (currentSpeed < 0.1f) {
        velocity->vx = 0.0f;
        velocity->vy = 0.0f;
        return;
    }

    float speedReduction = deceleration * deltaTime;
    float newSpeed = std::max(0.0f, currentSpeed - speedReduction);

    if (currentSpeed > 0.0f) {
        float scale = newSpeed / currentSpeed;
        velocity->vx *= scale;
        velocity->vy *= scale;
    }
}

bool MovementSystem::HasInput(const engine::Vector2& input) const {
    return std::abs(input.x) > 0.001f || std::abs(input.y) > 0.001f;
}

} // namespace ZombieSurvivor::System