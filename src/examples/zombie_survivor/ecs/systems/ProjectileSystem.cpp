// src/examples/zombie_survivor/ecs/systems/ProjectileSystem.cpp

#include "ProjectileSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "engine/core/ecs/components/Collider2D.hpp"
#include "engine/core/ecs/components/Tag.hpp"
#include "examples/zombie_survivor/ecs/components/ProjectileComponent.hpp"
#include "examples/zombie_survivor/events/ProjectileEventUtils.hpp"
#include <iostream>

namespace ZombieSurvivor::System {

void ProjectileSystem::Init() {
    auto* world = GetWorld();
    if (!world) return;

    auto& eventManager = world->GetEventManager();
    eventManager.Subscribe(engine::event::EventType::CUSTOM, this);

    std::cout << "[ProjectileSystem] Initialized and subscribed to events" << std::endl;
}

void ProjectileSystem::Update(float deltaTime) {
    UpdateProjectileMovement(deltaTime);
    UpdateProjectileLifetime(deltaTime);
    HandleBoundaryChecks();
    CleanupExpiredProjectiles();
}

void ProjectileSystem::Shutdown() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    eventManager.Unsubscribe(engine::event::EventType::CUSTOM, this);
    
    activeProjectiles_.clear();
    std::cout << "[ProjectileSystem] Shutdown complete" << std::endl;
}

void ProjectileSystem::onEvent(const std::shared_ptr<engine::event::Event>& event) {
    if (event && event->GetType() == engine::event::EventType::CUSTOM) {
        HandleGameEvent(event);
    }
}

void ProjectileSystem::HandleGameEvent(const std::shared_ptr<engine::event::Event>& event) {
    auto gameEvent = std::dynamic_pointer_cast<Events::GameEvent>(event);
    if (!gameEvent) return;
    
    switch (gameEvent->GetGameEventType()) {
        case Events::GameEventType::CREATE_PROJECTILE:
            HandleCreateProjectile(gameEvent->GetData());
            break;
        case Events::GameEventType::PROJECTILE_HIT:
            HandleProjectileDestroyed(gameEvent->GetData());
            break;
        default:
            break;
    }
}

void ProjectileSystem::HandleCreateProjectile(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::CreateProjectileData>(eventData);
    if (!data) return;
    
    if (activeProjectiles_.size() >= maxActiveProjectiles_) {
        std::cout << "[ProjectileSystem] Max projectiles reached, ignoring create request" << std::endl;
        return;
    }
    
    engine::EntityID projectileId = CreateProjectileEntity(*data);
    if (projectileId != 0) {
        activeProjectiles_.insert(projectileId);
        
        auto* world = GetWorld();
        if (world) {
            auto& eventManager = world->GetEventManager();
            auto createdData = std::make_shared<Events::ProjectileCreatedData>();
            createdData->projectileId = projectileId;
            createdData->shooterId = data->shooterId;
            createdData->position = data->startPosition;
            createdData->direction = data->direction;
            createdData->type = data->type;
            
            auto event = std::make_shared<Events::GameEvent>(
                Events::GameEventType::PROJECTILE_CREATED,
                std::static_pointer_cast<void>(createdData)
            );
            eventManager.Publish(event);
        }
        
        std::cout << "[ProjectileSystem] Created projectile " << projectileId 
                  << " for shooter " << data->shooterId << std::endl;
    }
}

void ProjectileSystem::HandleProjectileDestroyed(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::ProjectileHitData>(eventData);
    if (!data) return;
    
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* projectile = componentManager.GetComponent<Component::ProjectileComponent>(data->projectileId);
    
    if (projectile) {
        projectile->hasHit = true;
        projectile->shouldDestroy = true;
        
        if (projectile->penetration > 1) {
            projectile->penetration--;
            projectile->shouldDestroy = false;
        }
        
        std::cout << "[ProjectileSystem] Projectile " << data->projectileId 
                  << " hit target, remaining penetration: " << projectile->penetration << std::endl;
    }
}

engine::EntityID ProjectileSystem::CreateProjectileEntity(const ZombieSurvivor::Events::CreateProjectileData& data) {
    auto* world = GetWorld();
    if (!world) return 0;
    
    auto& entityFactory = world->GetEntityFactory();
    auto& componentManager = world->GetComponentManager();
    
    engine::EntityID projectileId = entityFactory.CreateEntity("Projectile");
    
    componentManager.AddComponent<engine::ECS::Transform2D>(projectileId,
        engine::ECS::Transform2D{data.startPosition.x, data.startPosition.y, 0.0f, 1.0f, 1.0f});
    
    engine::Vector2 velocity = data.direction * data.speed;
    componentManager.AddComponent<engine::ECS::Velocity2D>(projectileId,
        engine::ECS::Velocity2D{velocity.x, velocity.y, data.speed});
    
    componentManager.AddComponent<engine::ECS::Collider2D>(projectileId,
        engine::ECS::Collider2D{{0, 0, 4, 4}, false, "projectile"});
    
    Component::ProjectileComponent projectile;
    projectile.damage = data.damage;
    projectile.speed = data.speed;
    projectile.maxLifetime = data.lifetime;
    projectile.direction = data.direction;
    projectile.velocity = velocity;
    projectile.shooterId = data.shooterId;
    projectile.type = data.type;
    projectile.sourceWeaponType = data.weaponType;
    projectile.penetration = data.penetration;
    projectile.spread = data.spread;
    
    componentManager.AddComponent<Component::ProjectileComponent>(projectileId, projectile);
    
    componentManager.AddComponent<engine::ECS::Tag>(projectileId, 
        engine::ECS::Tag{"projectile"});
    
    return projectileId;
}

void ProjectileSystem::UpdateProjectileMovement(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto projectiles = componentManager.GetEntitiesWithComponent<Component::ProjectileComponent>();
    
    for (auto projectileId : projectiles) {
        auto* projectile = componentManager.GetComponent<Component::ProjectileComponent>(projectileId);
        auto* transform = componentManager.GetComponent<engine::ECS::Transform2D>(projectileId);
        auto* velocity = componentManager.GetComponent<engine::ECS::Velocity2D>(projectileId);
        
        if (projectile && transform && velocity && !projectile->shouldDestroy) {
            transform->x += velocity->vx * deltaTime;
            transform->y += velocity->vy * deltaTime;
            
            float distance = projectile->speed * deltaTime;
            projectile->distanceTraveled += distance;
        }
    }
}

void ProjectileSystem::UpdateProjectileLifetime(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto projectiles = componentManager.GetEntitiesWithComponent<Component::ProjectileComponent>();
    
    for (auto projectileId : projectiles) {
        auto* projectile = componentManager.GetComponent<Component::ProjectileComponent>(projectileId);
        if (projectile && !projectile->shouldDestroy) {
            projectile->currentLifetime += deltaTime;
            
            if (projectile->currentLifetime >= projectile->maxLifetime) {
                projectile->shouldDestroy = true;
                std::cout << "[ProjectileSystem] Projectile " << projectileId 
                          << " expired after " << projectile->currentLifetime << "s" << std::endl;
            }
        }
    }
}

void ProjectileSystem::HandleBoundaryChecks() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto projectiles = componentManager.GetEntitiesWithComponent<Component::ProjectileComponent>();
    
    for (auto projectileId : projectiles) {
        auto* projectile = componentManager.GetComponent<Component::ProjectileComponent>(projectileId);
        auto* transform = componentManager.GetComponent<engine::ECS::Transform2D>(projectileId);
        
        if (projectile && transform && !projectile->shouldDestroy) {
            if (transform->x < -worldBounds_.x || transform->x > worldBounds_.x ||
                transform->y < -worldBounds_.y || transform->y > worldBounds_.y) {
                projectile->shouldDestroy = true;
                std::cout << "[ProjectileSystem] Projectile " << projectileId 
                          << " destroyed at boundary (" << transform->x << ", " << transform->y << ")" << std::endl;
            }
        }
    }
}

void ProjectileSystem::CleanupExpiredProjectiles() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto& entityFactory = world->GetEntityFactory();
    auto projectiles = componentManager.GetEntitiesWithComponent<Component::ProjectileComponent>();
    
    for (auto projectileId : projectiles) {
        auto* projectile = componentManager.GetComponent<Component::ProjectileComponent>(projectileId);
        
        if (projectile && projectile->shouldDestroy) {
            entityFactory.DestroyEntity(projectileId);
            activeProjectiles_.erase(projectileId);
            
            std::cout << "[ProjectileSystem] Cleaned up projectile " << projectileId << std::endl;
        }
    }
}

} // namespace ZombieSurvivor::System
