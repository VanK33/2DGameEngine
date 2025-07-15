// src/examples/zombie_survivor/ecs/systems/ZombieAISystem.hpp

#pragma once

#include "engine/core/ecs/systems/AISystem.hpp"
#include "engine/core/ecs/components/AIComponent.hpp"
#include "examples/zombie_survivor/ecs/components/TargetComponent.hpp"
#include "engine/core/Types.hpp"
#include <vector>

namespace ZombieSurvivor::System {

using EntityID = engine::EntityID;
using Vector2 = engine::Vector2;

class ZombieAISystem : public engine::ECS::AISystem {
public:
    ZombieAISystem() = default;
    ~ZombieAISystem() = default;

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "ZombieAISystem"; }

protected:
    void ProcessAI(EntityID entity, engine::ECS::AIComponent& ai, float deltaTime) override;
    
    void OnStateChanged(EntityID entity, engine::ECS::AIState oldState, engine::ECS::AIState newState) override;

private:
    void ChaseTarget(EntityID zombieEntity, engine::ECS::AIComponent& ai);
    
    void FindNewTarget(EntityID zombieEntity);
    
    void SetTarget(EntityID zombieEntity, EntityID targetEntity, ZombieSurvivor::Component::TargetType type);
    
    EntityID FindNearestEntity(EntityID fromEntity, const std::vector<EntityID>& candidates);
};

} // namespace ZombieSurvivor::System
