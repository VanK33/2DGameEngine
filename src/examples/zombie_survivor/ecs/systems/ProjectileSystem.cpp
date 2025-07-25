// src/examples/zombie_survivor/ecs/systems/ProjectileSystem.cpp

#include "ProjectileSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "engine/core/ecs/components/Collider2D.hpp"
#include "engine/core/ecs/components/Sprite2D.hpp"
#include "engine/core/ecs/components/Tag.hpp"
#include "engine/core/ecs/components/PhysicsMode.hpp"
#include "examples/zombie_survivor/ecs/components/ProjectileComponent.hpp"
#include "examples/zombie_survivor/events/ProjectileEventUtils.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

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
        // Remove oldest projectile to make room for new one
        engine::EntityID oldestProjectile = FindOldestProjectile();
        if (oldestProjectile != 0) {
            DestroyProjectile(oldestProjectile);
            std::cout << "[ProjectileSystem] Removed oldest projectile to make room for new one" << std::endl;
        } else {
            std::cout << "[ProjectileSystem] Max projectiles reached, ignoring create request" << std::endl;
            return;
        }
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
                  << " for shooter " << data->shooterId 
                  << " at (" << std::fixed << std::setprecision(1) << data->startPosition.x << ", " << data->startPosition.y << ")"
                  << " - Expected range: " << (data->speed * data->lifetime) << "px"
                  << " (speed=" << data->speed << "px/s, lifetime=" << std::setprecision(3) << data->lifetime << "s)" << std::endl;
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
        projectile->targetsHit++;
        
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
    
    // Add PhysicsModeComponent for PhysicsSystem to process movement
    componentManager.AddComponent<engine::ECS::PhysicsModeComponent>(projectileId,
        engine::ECS::PhysicsModeComponent{
            engine::ECS::PhysicsMode::TOP_DOWN,  // 2D physics mode
            0.0f, 0.0f, 0.0f,                    // no gravity
            false,                               // disable gravity
            false,                               // disable friction for projectiles
            1.0f                                 // friction factor (unused when friction disabled)
        });
    
    componentManager.AddComponent<engine::ECS::Collider2D>(projectileId,
        engine::ECS::Collider2D{{0, 0, 4, 4}, false, "projectile"});
    
    // Add Sprite2D component to make projectile visible
    componentManager.AddComponent<engine::ECS::Sprite2D>(projectileId,
        engine::ECS::Sprite2D{
            "pixel.png",                    // texture path
            {0, 0, 8, 8},                   // sourceRect - bigger 8x8 bullet for visibility
            true,                           // visible
            {255, 255, 0, 255},             // tint - bright yellow bullet
            15                              // renderLayer - higher than ENTITIES (10) and WEAPON (12)
        });
    
    Component::ProjectileComponent projectile;
    projectile.damage = data.damage;
    projectile.speed = data.speed;
    projectile.maxLifetime = data.lifetime;
    projectile.direction = data.direction;
    projectile.velocity = velocity;
    projectile.startPosition = data.startPosition;  // Record start position for distance tracking
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
    
    auto projectiles = componentManager.GetEntitiesWithComponents<
        Component::ProjectileComponent,
        engine::ECS::Velocity2D,
        engine::ECS::Transform2D
    >();
    
    for (auto projectileId : projectiles) {
        auto* projectile = componentManager.GetComponent<Component::ProjectileComponent>(projectileId);
        auto* velocity = componentManager.GetComponent<engine::ECS::Velocity2D>(projectileId);
        auto* transform = componentManager.GetComponent<engine::ECS::Transform2D>(projectileId);
        
        if (projectile && velocity && transform && !projectile->shouldDestroy) {
            // Calculate distance moved this frame
            float speed = std::sqrt(velocity->vx * velocity->vx + velocity->vy * velocity->vy);
            float distanceThisFrame = speed * deltaTime;
            projectile->distanceTraveled += distanceThisFrame;
            
            // Calculate actual straight-line distance from start position
            float actualDistance = std::sqrt(
                (transform->x - projectile->startPosition.x) * (transform->x - projectile->startPosition.x) +
                (transform->y - projectile->startPosition.y) * (transform->y - projectile->startPosition.y)
            );
            
            // Debug output every 0.1 seconds (roughly every 6 frames at 60fps)
            if (static_cast<int>(projectile->currentLifetime * 10) != static_cast<int>((projectile->currentLifetime - deltaTime) * 10)) {
                std::cout << "[DEBUG] Projectile " << projectileId 
                          << " - Time: " << std::fixed << std::setprecision(3) << projectile->currentLifetime
                          << "s, Traveled: " << std::setprecision(1) << projectile->distanceTraveled 
                          << "px, Actual: " << actualDistance 
                          << "px, Pos: (" << transform->x << ", " << transform->y << ")"
                          << ", Start: (" << projectile->startPosition.x << ", " << projectile->startPosition.y << ")" << std::endl;
            }
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
                          << " EXPIRED after " << std::fixed << std::setprecision(3) << projectile->currentLifetime 
                          << "s (max: " << projectile->maxLifetime << "s)"
                          << " - Distance traveled: " << std::setprecision(1) << projectile->distanceTraveled << "px" << std::endl;
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
                          << " HIT BOUNDARY at (" << std::fixed << std::setprecision(1) << transform->x << ", " << transform->y << ")"
                          << " after " << std::setprecision(3) << projectile->currentLifetime << "s"
                          << " - Distance: " << std::setprecision(1) << projectile->distanceTraveled << "px"
                          << " [BOUNDARY BOUNDS: " << worldBounds_.x << "x" << worldBounds_.y << "]" << std::endl;
            }
        }
    }
}

void ProjectileSystem::CleanupExpiredProjectiles() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto& entityFactory = world->GetEntityFactory();
    
    // 使用迭代器安全地遍历并修改activeProjectiles_集合
    std::vector<engine::EntityID> toRemove;
    
    for (auto projectileId : activeProjectiles_) {
        auto* projectile = componentManager.GetComponent<Component::ProjectileComponent>(projectileId);
        
        if (projectile && projectile->shouldDestroy) {
            // Debug: Check which components exist before cleanup
            auto* transform = componentManager.GetComponent<engine::ECS::Transform2D>(projectileId);
            auto* sprite = componentManager.GetComponent<engine::ECS::Sprite2D>(projectileId);
            auto* velocity = componentManager.GetComponent<engine::ECS::Velocity2D>(projectileId);
            
            std::cout << "[ProjectileSystem] BEFORE cleanup - Projectile " << projectileId 
                      << " Components: Transform=" << (transform ? "YES" : "NO")
                      << ", Sprite=" << (sprite ? "YES" : "NO")
                      << ", Velocity=" << (velocity ? "YES" : "NO") << std::endl;
            
            // Use ComponentManager's unified cleanup - removes ALL components
            componentManager.RemoveAllComponents(projectileId);
            
            // Then destroy entity ID from EntityFactory
            entityFactory.DestroyEntity(projectileId);
            toRemove.push_back(projectileId);
            
            // Debug: Verify all components are gone
            transform = componentManager.GetComponent<engine::ECS::Transform2D>(projectileId);
            sprite = componentManager.GetComponent<engine::ECS::Sprite2D>(projectileId);
            velocity = componentManager.GetComponent<engine::ECS::Velocity2D>(projectileId);
            
            std::cout << "[ProjectileSystem] AFTER unified cleanup - Projectile " << projectileId 
                      << " Components: Transform=" << (transform ? "YES" : "NO")
                      << ", Sprite=" << (sprite ? "YES" : "NO")
                      << ", Velocity=" << (velocity ? "YES" : "NO") << std::endl;
            
            std::cout << "[ProjectileSystem] Cleaned up projectile " << projectileId << std::endl;
        }
    }
    
    // 从activeProjectiles_中移除已清理的projectile
    for (auto projectileId : toRemove) {
        activeProjectiles_.erase(projectileId);
    }
}

engine::EntityID ProjectileSystem::FindOldestProjectile() {
    auto* world = GetWorld();
    if (!world) return 0;
    
    auto& componentManager = world->GetComponentManager();
    engine::EntityID oldestId = 0;
    float longestLifetime = 0.0f;
    
    for (engine::EntityID projectileId : activeProjectiles_) {
        auto* projectile = componentManager.GetComponent<Component::ProjectileComponent>(projectileId);
        if (projectile && projectile->currentLifetime > longestLifetime) {
            longestLifetime = projectile->currentLifetime;
            oldestId = projectileId;
        }
    }
    
    return oldestId;
}

void ProjectileSystem::DestroyProjectile(engine::EntityID projectileId) {
    activeProjectiles_.erase(projectileId);
    
    auto* world = GetWorld();
    if (world) {
        // Use unified cleanup for consistency
        world->GetComponentManager().RemoveAllComponents(projectileId);
        world->GetEntityFactory().DestroyEntity(projectileId);
    }
}

} // namespace ZombieSurvivor::System
