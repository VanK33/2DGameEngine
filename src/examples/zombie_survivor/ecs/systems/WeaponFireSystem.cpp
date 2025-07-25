// src/examples/zombie_survivor/ecs/systems/WeaponFireSystem.cpp

#include "WeaponFireSystem.hpp"
#include "WeaponSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include "examples/zombie_survivor/ecs/components/AimingComponent.hpp"
#include "examples/zombie_survivor/ecs/components/InputComponent.hpp"
#include "examples/zombie_survivor/ecs/components/FollowComponent.hpp"
#include "examples/zombie_survivor/configs/ProjectileConfig.hpp"
#include <iostream>
#include <cmath>

namespace ZombieSurvivor::System {

void WeaponFireSystem::Init() {
    std::cout << "[WeaponFireSystem] Init() called" << std::endl;
    auto* world = GetWorld();
    if (!world) {
        std::cout << "[WeaponFireSystem] ERROR: No world available during Init!" << std::endl;
        return;
    }

    auto& eventManager = world->GetEventManager();
    eventManager.Subscribe(engine::event::EventType::CUSTOM, this);
    
    std::cout << "[WeaponFireSystem] Initialized and subscribed to CUSTOM events" << std::endl;
}

void WeaponFireSystem::Update(float deltaTime) {
    // This system is event-driven, no update needed
}

void WeaponFireSystem::Shutdown() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    eventManager.Unsubscribe(engine::event::EventType::CUSTOM, this);
    
    std::cout << "[WeaponFireSystem] Shutdown" << std::endl;
}

void WeaponFireSystem::onEvent(const std::shared_ptr<engine::event::Event>& event) {
    if (event && event->GetType() == engine::event::EventType::CUSTOM) {
        HandleGameEvent(event);
    }
}

void WeaponFireSystem::HandleGameEvent(const std::shared_ptr<engine::event::Event>& event) {
    auto gameEvent = std::dynamic_pointer_cast<Events::GameEvent>(event);
    if (!gameEvent) return;
    
    
    switch (gameEvent->GetGameEventType()) {
        case Events::GameEventType::FIRE_INPUT: {
            auto data = std::static_pointer_cast<Events::FireInputData>(gameEvent->GetData());
            if (data && data->pressed) {
                HandleFireInput(data->playerId);
            }
            break;
        }
    }
}

void WeaponFireSystem::HandleFireInput(uint32_t playerId) {
    
    engine::EntityID weaponEntityId = FindWeaponEntityForPlayer(playerId);
    if (weaponEntityId == 0) {
        return;
    }
    
    // Check if weapon is ready (cooldown, reloading) 
    if (!CanFire(playerId)) {
        std::cout << "[WeaponFireSystem] Player " << playerId << " weapon not ready (cooldown/reloading)" << std::endl;
        return;
    }
    
    // Get weapon's ammo type
    Component::AmmoType ammoType = GetWeaponAmmoType(playerId);
    if (ammoType == Component::AmmoType::NONE) {
        std::cout << "[WeaponFireSystem] Player " << playerId << " weapon has invalid ammo type" << std::endl;
        return;
    }
    
    // Try to consume ammo directly
    if (TryConsumeAmmo(playerId, ammoType, 1)) {
        // Ammo consumed successfully, create projectile
        CreateProjectile(playerId, ammoType);
        
        // Notify WeaponSystem for cooldown
        PublishWeaponFiredEvent(playerId);
        
        std::cout << "[WeaponFireSystem] Successfully fired projectile for player " << playerId << std::endl;
    } else {
        std::cout << "[WeaponFireSystem] Failed to consume ammo for player " << playerId << std::endl;
    }
}

bool WeaponFireSystem::CanFire(uint32_t playerId) const {
    auto* world = GetWorld();
    if (!world) return false;
    
    // Check with WeaponSystem if weapon is ready (not on cooldown or reloading)
    auto* weaponSystem = static_cast<WeaponSystem*>(
        world->GetSystemManager().GetSystem("WeaponSystem")
    );
    if (!weaponSystem) return false;
    
    return weaponSystem->IsWeaponReady(playerId);
}

Component::AmmoType WeaponFireSystem::GetWeaponAmmoType(uint32_t playerId) const {
    auto* world = GetWorld();
    if (!world) return Component::AmmoType::NONE;
    
    auto& componentManager = world->GetComponentManager();
    
    // Find the weapon entity for this player
    engine::EntityID weaponEntityId = FindWeaponEntityForPlayer(playerId);
    if (weaponEntityId == 0) return Component::AmmoType::NONE;
    
    auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(weaponEntityId);
    if (!weapon) return Component::AmmoType::NONE;
    
    return weapon->currentAmmoType;
}

bool WeaponFireSystem::TryConsumeAmmo(uint32_t playerId, Component::AmmoType ammoType, int amount) {
    auto* world = GetWorld();
    if (!world) return false;
    
    auto& componentManager = world->GetComponentManager();
    auto* ammo = componentManager.GetComponent<Component::AmmoComponent>(playerId);
    if (!ammo || ammo->currentAmmo < amount) {
        return false;
    }
    
    // Consume ammo directly
    ammo->currentAmmo -= amount;
    
    std::cout << "[WeaponFireSystem] Consumed " << amount << " ammo for player " << playerId 
              << ", remaining: " << ammo->currentAmmo << "/" << ammo->totalAmmo << std::endl;
    
    return true;
}

void WeaponFireSystem::PublishWeaponFiredEvent(uint32_t playerId) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    auto firedData = std::make_shared<Events::WeaponFiredData>();
    firedData->entityId = playerId;
    
    auto firedEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::WEAPON_FIRED,
        std::static_pointer_cast<void>(firedData)
    );
    eventManager.Publish(firedEvent);
}


void WeaponFireSystem::CreateProjectile(uint32_t playerId, Component::AmmoType ammoType) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto& eventManager = world->GetEventManager();
    
    // Find the weapon entity for this player
    engine::EntityID weaponEntityId = FindWeaponEntityForPlayer(playerId);
    if (weaponEntityId == 0) {
        return;
    }
    
    // Use weapon entity components instead of player components
    auto* weaponTransform = componentManager.GetComponent<engine::ECS::Transform2D>(weaponEntityId);
    auto* weaponAiming = componentManager.GetComponent<Component::AimingComponent>(weaponEntityId);
    auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(weaponEntityId);
    
    if (!weaponTransform || !weapon) {
        return;
    }
    
    auto projectileConfig = ZombieSurvivor::Config::ProjectileConfigManager::GetConfig(ammoType);
    
    // Calculate direction directly using SDL coordinates (no Y-flip)
    engine::Vector2 direction{1.0f, 0.0f};
    
    // Try to get mouse position from weapon's InputComponent (copied by WeaponFollowSystem)
    auto* weaponInput = componentManager.GetComponent<Component::InputComponent>(weaponEntityId);
    if (weaponInput) {
        // Calculate direction from weapon position to mouse position (SDL coordinates)
        engine::Vector2 weaponPos{weaponTransform->x, weaponTransform->y};
        engine::Vector2 mousePos = weaponInput->mousePosition;
        
        // Direct SDL coordinate direction calculation (no Y-flip)
        direction = engine::Vector2{mousePos.x - weaponPos.x, mousePos.y - weaponPos.y};
        
        // Normalize direction vector
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length > 0.001f) {
            direction.x /= length;
            direction.y /= length;
        } else {
            direction = engine::Vector2{1.0f, 0.0f}; // fallback to right
        }
        
    } else if (weaponAiming) {
        // Fallback to existing aiming direction (with Y-flip)
        direction = weaponAiming->aimDirection;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length > 0.0f) {
            direction.x /= length;
            direction.y /= length;
        }
    }
    
    // Calculate weapon tip position for projectile spawn
    engine::Vector2 tipPosition = CalculateWeaponTipPosition(weaponEntityId);
    
    // 创建子弹事件
    auto projectileData = std::make_shared<Events::CreateProjectileData>();
    projectileData->shooterId = playerId;
    projectileData->startPosition = tipPosition;
    projectileData->direction = direction;
    projectileData->damage = projectileConfig.damage;
    projectileData->speed = projectileConfig.speed;
    projectileData->lifetime = projectileConfig.lifetime;
    projectileData->type = projectileConfig.type;
    projectileData->weaponType = weapon->type;
    projectileData->penetration = projectileConfig.penetration;
    
    auto projectileEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::CREATE_PROJECTILE,
        std::static_pointer_cast<void>(projectileData)
    );
    eventManager.Publish(projectileEvent);
    
    std::cout << "[WeaponFireSystem] Created " << static_cast<int>(ammoType) 
              << " projectile for player " << playerId 
              << " (damage=" << projectileConfig.damage 
              << ", speed=" << projectileConfig.speed << ")" << std::endl;
}

engine::EntityID WeaponFireSystem::FindWeaponEntityForPlayer(uint32_t playerId) const {
    auto* world = GetWorld();
    if (!world) return 0;
    
    auto& componentManager = world->GetComponentManager();
    
    // Find weapon entity that follows this player
    auto entities = componentManager.GetEntitiesWithComponent<Component::FollowComponent>();
    
    for (const auto& entityId : entities) {
        auto* follow = componentManager.GetComponent<Component::FollowComponent>(entityId);
        if (follow) {
            if (follow->targetEntityId == playerId) {
                // This entity follows the player - check if it's a weapon
                auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(entityId);
                if (weapon) {
                    return entityId;
                }
            }
        }
    }
    
    std::cout << "[WeaponFireSystem] No weapon found for player " << playerId << std::endl;
    return 0; // No weapon found
}

engine::Vector2 WeaponFireSystem::CalculateWeaponTipPosition(engine::EntityID weaponEntityId) const {
    auto* world = GetWorld();
    if (!world) return {0.0f, 0.0f};
    
    auto& componentManager = world->GetComponentManager();
    
    // Get weapon transform and input (for consistent coordinate system)
    auto* weaponTransform = componentManager.GetComponent<engine::ECS::Transform2D>(weaponEntityId);
    auto* weaponInput = componentManager.GetComponent<Component::InputComponent>(weaponEntityId);
    
    if (!weaponTransform) {
        return {0.0f, 0.0f};
    }
    
    // Calculate tip position using the same direction as bullets (SDL coordinates)
    float tipDistance = 20.0f;  // Half weapon length to get tip from center
    engine::Vector2 weaponPos{weaponTransform->x, weaponTransform->y};
    
    if (weaponInput) {
        // Use same SDL coordinate direction calculation as bullets
        engine::Vector2 mousePos = weaponInput->mousePosition;
        engine::Vector2 direction{mousePos.x - weaponPos.x, mousePos.y - weaponPos.y};
        
        // Normalize direction
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length > 0.001f) {
            direction.x /= length;
            direction.y /= length;
        } else {
            direction = engine::Vector2{1.0f, 0.0f}; // fallback to right
        }
        
        // Calculate tip position using normalized direction
        float tipX = weaponPos.x + tipDistance * direction.x;
        float tipY = weaponPos.y + tipDistance * direction.y;
        
        return engine::Vector2{tipX, tipY};
    } else {
        // Fallback: use transform rotation (old method)
        float tipX = weaponTransform->x + tipDistance * std::cos(weaponTransform->rotation);
        float tipY = weaponTransform->y + tipDistance * std::sin(weaponTransform->rotation);
        return engine::Vector2{tipX, tipY};
    }
}

} // namespace ZombieSurvivor::System