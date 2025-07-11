// src/engine/core/ecs/systems/AISystem.hpp
#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/AIComponent.hpp"
#include "engine/core/Types.hpp"

namespace engine::ECS {


class AISystem : public System {
public:
    AISystem() = default;
    virtual ~AISystem() = default;

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

protected:
    
    Vector2 GetEntityPosition(EntityID entity) const;
    
    void SetEntityVelocity(EntityID entity, float vx, float vy);
    void MoveTowards(EntityID entity, const Vector2& target, float speed);
    void StopMovement(EntityID entity);
    float GetDistance(EntityID from, EntityID to) const;
    Vector2 GetDirection(const Vector2& from, const Vector2& to) const;
    void SetAIState(EntityID entity, AIState newState);
    
    virtual void ProcessAI(EntityID entity, AIComponent& ai, float deltaTime) = 0;
    virtual void OnStateChanged(EntityID entity, AIState oldState, AIState newState) {}

private:
    AIComponent* GetAIComponent(EntityID entity);
};

} // namespace engine::ECS