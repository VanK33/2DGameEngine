// src/engine/core/ecs/systems/AISystem.cpp

#include "AISystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include <cmath>
#include <iostream>

namespace engine::ECS {

void AISystem::Init() {
    std::cout << "[AISystem] AI system initialized" << std::endl;
}

void AISystem::Update(float deltaTime) {
    if (!world_) return;
    
    auto& componentManager = world_->GetComponentManager();
    auto entities = componentManager.GetEntitiesWithComponent<AIComponent>();
    
    for (auto entity : entities) {
        auto* ai = componentManager.GetComponent<AIComponent>(entity);
        if (!ai) continue;
        
        switch (ai->state) {
            case AIState::ACTIVE:
                ai->updateTimer += deltaTime;
                
                if (ai->updateTimer >= ai->updateInterval) {
                    ProcessAI(entity, *ai, ai->updateTimer);
                    ai->updateTimer = 0.0f;
                }
                break;
                
            case AIState::INACTIVE:
            case AIState::DISABLED:
                StopMovement(entity);
                break;
        }
    }
}

void AISystem::Shutdown() {
    std::cout << "[AISystem] AI system shutdown" << std::endl;
}


Vector2 AISystem::GetEntityPosition(EntityID entity) const {
    if (!world_) return {0, 0};
    
    auto& componentManager = world_->GetComponentManager();
    auto* transform = componentManager.GetComponent<Transform2D>(entity);
    
    if (transform) {
        return {transform->x, transform->y};
    }
    
    return {0, 0};
}

void AISystem::SetEntityVelocity(EntityID entity, float vx, float vy) {
    if (!world_) return;
    
    auto& componentManager = world_->GetComponentManager();
    auto* velocity = componentManager.GetComponent<Velocity2D>(entity);
    
    if (velocity) {
        velocity->vx = vx;
        velocity->vy = vy;
    }
}

void AISystem::MoveTowards(EntityID entity, const Vector2& target, float speed) {
    Vector2 position = GetEntityPosition(entity);
    Vector2 direction = GetDirection(position, target);
    
    if (direction.Length() > 0.01f) {
        Vector2 normalized = direction.Normalized();
        SetEntityVelocity(entity, normalized.x * speed, normalized.y * speed);
    } else {
        StopMovement(entity);
    }
}

void AISystem::StopMovement(EntityID entity) {
    SetEntityVelocity(entity, 0, 0);
}

float AISystem::GetDistance(EntityID from, EntityID to) const {
    Vector2 pos1 = GetEntityPosition(from);
    Vector2 pos2 = GetEntityPosition(to);
    return (pos2 - pos1).Length();
}

Vector2 AISystem::GetDirection(const Vector2& from, const Vector2& to) const {
    return to - from;
}

void AISystem::SetAIState(EntityID entity, AIState newState) {
    auto* ai = GetAIComponent(entity);
    if (!ai) return;
    
    if (ai->state != newState) {
        AIState oldState = ai->state;
        ai->state = newState;
        OnStateChanged(entity, oldState, newState);
    }
}

AIComponent* AISystem::GetAIComponent(EntityID entity) {
    if (!world_) return nullptr;
    return world_->GetComponentManager().GetComponent<AIComponent>(entity);
}

void AISystem::OnStateChanged(EntityID entity, AIState oldState, AIState newState) {}

} // namespace engine::ECS