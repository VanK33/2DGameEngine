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
    // 监听碰撞事件
    if (auto* world = GetWorld()) {
        auto& eventManager = world->GetEventManager(); // 使用引用
        eventManager.Subscribe(engine::event::EventType::COLLISION_STARTED, this);
    }
}

void PhysicsSystem::Update(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto entities = componentManager.GetEntitiesWithComponents<Transform2D, Velocity2D>();
    
    for (EntityID entityId : entities) {
        auto* transform = componentManager.GetComponent<Transform2D>(entityId);
        auto* velocity = componentManager.GetComponent<Velocity2D>(entityId);
        auto* physicsMode = componentManager.GetComponent<PhysicsModeComponent>(entityId);
        
        if (transform && velocity) {
            // 保存当前位置
            lastPositions_[entityId] = {transform->x, transform->y};
            
            float oldX = transform->x;
            float oldY = transform->y;
            
            if (physicsMode && physicsMode->enableGravity) {
                ApplyGravity(velocity, physicsMode, deltaTime);
            }
            
            transform->x += velocity->vx * deltaTime;
            transform->y += velocity->vy * deltaTime;
            
            CheckBoundaries(transform, velocity);
            
            // TODO: 与CollisionSystem集成
            // 暂时注释掉碰撞检测
            // if (HasCollision(transform)) {
            //     HandleCollision(transform, velocity, oldX, oldY);
            // }
            
            LimitVelocity(velocity);
            
            if (!physicsMode || physicsMode->enableFriction) {
                ApplyFriction(velocity, physicsMode ? physicsMode->frictionFactor : 0.98f);
            }
        }
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

void PhysicsSystem::ApplyFriction(Velocity2D* velocity, float frictionFactor) {
    velocity->vx *= frictionFactor;
    velocity->vy *= frictionFactor;
    
    // 性能优化：如果速度太小，直接设为0
    const float minSpeed = 0.1f;
    if (std::abs(velocity->vx) < minSpeed) velocity->vx = 0;
    if (std::abs(velocity->vy) < minSpeed) velocity->vy = 0;
}

void PhysicsSystem::CheckBoundaries(Transform2D* transform, Velocity2D* velocity) {
    // 简单的屏幕边界检测
    const float screenWidth = 800.0f;   // 应该从配置获取
    const float screenHeight = 600.0f;
    
    if (transform->x < 0) {
        transform->x = 0;
        velocity->vx = 0;  // 停止移动
    } else if (transform->x > screenWidth) {
        transform->x = screenWidth;
        velocity->vx = 0;
    }
    
    if (transform->y < 0) {
        transform->y = 0;
        velocity->vy = 0;
    } else if (transform->y > screenHeight) {
        transform->y = screenHeight;
        velocity->vy = 0;
    }
}

bool PhysicsSystem::HasCollision(Transform2D* transform) {
    // 这里应该与碰撞系统集成
    // 暂时返回false，等碰撞系统实现后再完善
    return false;
}

void PhysicsSystem::HandleCollision(Transform2D* transform, Velocity2D* velocity, float oldX, float oldY) {
    transform->x = oldX;
    transform->y = oldY;
    
    velocity->vx = 0;
    velocity->vy = 0;
}

void PhysicsSystem::onEvent(const std::shared_ptr<engine::event::Event>& event) {
    if (event->getType() == engine::event::EventType::COLLISION_STARTED) {
        HandleCollisionEvent(*event);
    }
}

void PhysicsSystem::HandleCollisionEvent(const engine::event::Event& event) {
    auto collisionData = std::static_pointer_cast<engine::event::CollisionData>(event.getData());
    HandleCollisionResponse(collisionData->entityA, collisionData->entityB);
}

void PhysicsSystem::HandleCollisionResponse(EntityID entityA, EntityID entityB) {
    // TODO: 实现碰撞响应逻辑
    // 比如：回退位置、停止移动等
}

} // namespace engine::ECS