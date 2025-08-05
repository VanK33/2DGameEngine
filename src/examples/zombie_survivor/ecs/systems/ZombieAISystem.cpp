// src/examples/zombie_survivor/ecs/systems/ZombieAISystem.cpp

#include "ZombieAISystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "engine/core/ecs/components/Tag.hpp"
#include "engine/core/ecs/components/SpriteStateComponent.hpp"
#include "examples/zombie_survivor/ecs/components/TargetComponent.hpp"
#include "examples/zombie_survivor/ecs/components/HealthComponent.hpp"
#include "engine/core/Types.hpp"  // Add this for Vector2
#include <iostream>
#include <algorithm>

namespace ZombieSurvivor::System {

using EntityID = engine::ECS::EntityID;  // Add this for EntityID
using Vector2 = engine::Vector2;          // Add this for Vector2

void ZombieAISystem::Init() {
    std::cout << "[ZombieAISystem] Initialized" << std::endl;
}

void ZombieAISystem::Update(float deltaTime) {
    engine::ECS::AISystem::Update(deltaTime);
}

void ZombieAISystem::Shutdown() {
    std::cout << "[ZombieAISystem] Shutdown" << std::endl;
}

void ZombieAISystem::ProcessAI(EntityID entity, engine::ECS::AIComponent& ai, float deltaTime) {
    ChaseTarget(entity, ai);
    UpdateZombieSpriteState(entity, ai);
}

void ZombieAISystem::ChaseTarget(EntityID zombieEntity, engine::ECS::AIComponent& ai) {
    auto& componentManager = GetWorld()->GetComponentManager();
    auto* target = componentManager.GetComponent<ZombieSurvivor::Component::TargetComponent>(zombieEntity);
    
    if (!target || target->targetEntity == 0) {
        FindNewTarget(zombieEntity);
        return;
    }
    
    if (!GetWorld()->HasEntity(target->targetEntity)) {
        target->targetEntity = 0;
        return;
    }
    
    Vector2 zombiePos = GetEntityPosition(zombieEntity);
    Vector2 targetPos = GetEntityPosition(target->targetEntity);
    float distance = GetDistance(zombieEntity, target->targetEntity);
    
    MoveTowards(zombieEntity, targetPos, ai.speed);
    
    // Debug output - log movement every 120 frames (~2 seconds at 60fps)
    static int frameCounter = 0;
    if (++frameCounter % 120 == 0) {
        auto* velocity = componentManager.GetComponent<engine::ECS::Velocity2D>(zombieEntity);
        std::cout << "[ZombieAISystem] Zombie " << zombieEntity 
                  << " pos(" << zombiePos.x << "," << zombiePos.y << ")"
                  << " -> target(" << targetPos.x << "," << targetPos.y << ")"
                  << " distance=" << distance
                  << " velocity=(" << (velocity ? velocity->vx : 0) << "," << (velocity ? velocity->vy : 0) << ")"
                  << " speed=" << ai.speed << std::endl;
    }
}

void ZombieAISystem::FindNewTarget(EntityID zombieEntity) {
    auto& componentManager = GetWorld()->GetComponentManager();
    auto entitiesWithTag = componentManager.GetEntitiesWithComponent<engine::ECS::Tag>();
    
    std::vector<EntityID> players;
    for (EntityID entity : entitiesWithTag) {
        auto* tag = componentManager.GetComponent<engine::ECS::Tag>(entity);
        if (tag && tag->tag == "player") {
            players.push_back(entity);
        }
    }
    
    if (!players.empty()) {
        EntityID nearestPlayer = FindNearestEntity(zombieEntity, players);
        SetTarget(zombieEntity, nearestPlayer, ZombieSurvivor::Component::TargetType::PLAYER);
        std::cout << "[ZombieAISystem] Zombie " << zombieEntity << " found new target: " << nearestPlayer << std::endl;
    }
}

void ZombieAISystem::SetTarget(EntityID zombieEntity, EntityID targetEntity, ZombieSurvivor::Component::TargetType type) {
    auto& componentManager = GetWorld()->GetComponentManager();
    auto* target = componentManager.GetComponent<ZombieSurvivor::Component::TargetComponent>(zombieEntity);
    if (target) {
        target->targetEntity = targetEntity;
        target->targetType = type;
        target->lastTargetUpdate = SDL_GetTicks() / 1000.0f;
    }
}

EntityID ZombieAISystem::FindNearestEntity(EntityID fromEntity, const std::vector<EntityID>& candidates) {
    if (candidates.empty()) return 0;
    
    EntityID nearest = candidates[0];
    float minDistance = GetDistance(fromEntity, nearest);
    
    for (EntityID candidate : candidates) {
        float distance = GetDistance(fromEntity, candidate);
        if (distance < minDistance) {
            minDistance = distance;
            nearest = candidate;
        }
    }
    
    return nearest;
}

void ZombieAISystem::OnStateChanged(EntityID entity, engine::ECS::AIState oldState, engine::ECS::AIState newState) {
    std::cout << "[ZombieAISystem] Zombie " << entity << " state changed from " 
              << static_cast<int>(oldState) << " to " << static_cast<int>(newState) << std::endl;
}

void ZombieAISystem::UpdateZombieSpriteState(EntityID zombieEntity, engine::ECS::AIComponent& ai) {
    auto& componentManager = GetWorld()->GetComponentManager();
    auto* spriteState = componentManager.GetComponent<engine::ECS::SpriteStateComponent>(zombieEntity);
    auto* health = componentManager.GetComponent<ZombieSurvivor::Component::HealthComponent>(zombieEntity);
    auto* velocity = componentManager.GetComponent<engine::ECS::Velocity2D>(zombieEntity);
    
    if (!spriteState) return;
    
    // Check for death state first (highest priority)
    if (health && health->health <= 0.0f && !health->isAlive) {
        spriteState->currentState = engine::ECS::SpriteStateComponent::State::DEAD;
        return;
    }
    
    // Check if zombie is moving
    bool isMoving = false;
    if (velocity) {
        float speed = std::sqrt(velocity->vx * velocity->vx + velocity->vy * velocity->vy);
        isMoving = speed > 10.0f; // Movement threshold
    }
    
    // Update state based on AI behavior and movement
    if (isMoving) {
        spriteState->currentState = engine::ECS::SpriteStateComponent::State::WALKING;
    } else {
        spriteState->currentState = engine::ECS::SpriteStateComponent::State::IDLE;
    }
    
    // TODO: Add ATTACKING state when in combat range
    // TODO: Add HURT state when taking damage (requires damage event system)
}

} // namespace ZombieSurvivor::System