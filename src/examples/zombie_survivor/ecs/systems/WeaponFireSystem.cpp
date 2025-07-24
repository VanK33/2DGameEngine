// src/examples/zombie_survivor/ecs/systems/WeaponFireSystem.cpp

#include "WeaponFireSystem.hpp"
#include "WeaponSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include "examples/zombie_survivor/ecs/components/AimingComponent.hpp"
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
    
    std::cout << "[WeaponFireSystem] Received GameEvent type: " << static_cast<int>(gameEvent->GetGameEventType()) << std::endl;
    
    switch (gameEvent->GetGameEventType()) {
        case Events::GameEventType::FIRE_INPUT: {
            auto data = std::static_pointer_cast<Events::FireInputData>(gameEvent->GetData());
            if (data && data->pressed) {
                HandleFireInput(data->playerId);
            }
            break;
        }
        case Events::GameEventType::AMMO_CONSUMED: {
            HandleAmmoConsumed(gameEvent->GetData());
            break;
        }
    }
}

void WeaponFireSystem::HandleFireInput(uint32_t playerId) {
    std::cout << "[WeaponFireSystem] Received FIRE_INPUT event for player " << playerId << std::endl;
    if (!CanFire(playerId)) {
        std::cout << "[WeaponFireSystem] Player " << playerId << " cannot fire" << std::endl;
        return;
    }
    
    // Get weapon's ammo type
    Component::AmmoType ammoType = GetWeaponAmmoType(playerId);
    if (ammoType == Component::AmmoType::NONE) {
        std::cout << "[WeaponFireSystem] Player " << playerId << " has no valid weapon" << std::endl;
        return;
    }
    
    // Request ammo consumption
    PublishAmmoConsumeRequest(playerId, ammoType);
    
    std::cout << "[WeaponFireSystem] Player " << playerId << " fire request sent for ammo type " 
              << static_cast<int>(ammoType) << std::endl;
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

void WeaponFireSystem::PublishAmmoConsumeRequest(uint32_t playerId, Component::AmmoType ammoType) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    auto consumeData = std::make_shared<Events::AmmoConsumeRequestData>();
    consumeData->playerId = playerId;
    consumeData->ammoType = ammoType;
    consumeData->amount = 1;  // Consume 1 bullet per shot
    
    auto consumeEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::AMMO_CONSUME_REQUEST,
        std::static_pointer_cast<void>(consumeData)
    );
    eventManager.Publish(consumeEvent);
}

void WeaponFireSystem::HandleAmmoConsumed(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::AmmoConsumedData>(eventData);
    if (!data) return;
    
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(data->entityId);
    if (!weapon) return;
    
    CreateProjectile(data->entityId, weapon->currentAmmoType);
    
    std::cout << "[WeaponFireSystem] Ammo consumed, creating projectile for player " 
              << data->entityId << std::endl;
}

void WeaponFireSystem::CreateProjectile(uint32_t playerId, Component::AmmoType ammoType) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto& eventManager = world->GetEventManager();
    
    // Find the weapon entity for this player
    engine::EntityID weaponEntityId = FindWeaponEntityForPlayer(playerId);
    if (weaponEntityId == 0) {
        std::cout << "[WeaponFireSystem] No weapon found for player " << playerId << std::endl;
        return;
    }
    
    // Use weapon entity components instead of player components
    auto* weaponTransform = componentManager.GetComponent<engine::ECS::Transform2D>(weaponEntityId);
    auto* weaponAiming = componentManager.GetComponent<Component::AimingComponent>(weaponEntityId);
    auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(weaponEntityId);
    
    if (!weaponTransform || !weapon) {
        std::cout << "[WeaponFireSystem] Missing weapon components for entity " << weaponEntityId << std::endl;
        return;
    }
    
    auto projectileConfig = ZombieSurvivor::Config::ProjectileConfigManager::GetConfig(ammoType);
    
    // Use weapon's aiming direction
    engine::Vector2 direction{1.0f, 0.0f};
    if (weaponAiming) {
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
    
    std::cout << "[WeaponFireSystem] Finding weapon for player " << playerId << std::endl;
    
    // Find weapon entity that follows this player
    auto entities = componentManager.GetEntitiesWithComponent<Component::FollowComponent>();
    std::cout << "[WeaponFireSystem] Found " << entities.size() << " entities with FollowComponent" << std::endl;
    
    for (const auto& entityId : entities) {
        auto* follow = componentManager.GetComponent<Component::FollowComponent>(entityId);
        if (follow) {
            std::cout << "[WeaponFireSystem] Entity " << entityId << " follows " << follow->targetEntityId << std::endl;
            if (follow->targetEntityId == playerId) {
                // This entity follows the player - check if it's a weapon
                auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(entityId);
                std::cout << "[WeaponFireSystem] Entity " << entityId << " follows player " << playerId 
                          << ", has weapon: " << (weapon ? "yes" : "no") << std::endl;
                if (weapon) {
                    std::cout << "[WeaponFireSystem] Found weapon entity " << entityId << " for player " << playerId << std::endl;
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
    
    // Get weapon transform
    auto* weaponTransform = componentManager.GetComponent<engine::ECS::Transform2D>(weaponEntityId);
    if (!weaponTransform) {
        return {0.0f, 0.0f};
    }
    
    // Weapon specifications:
    // - Pivot point: {1.0f, 0.5f} (right edge center)
    // - Dimensions: 40x12 pixels
    // - Tip is at the LEFT edge (opposite to pivot)
    
    // Calculate tip position: move 20 pixels from weapon center in weapon facing direction
    float tipDistance = 20.0f;  // Half weapon length to get tip from center
    float tipX = weaponTransform->x + tipDistance * std::cos(weaponTransform->rotation);
    float tipY = weaponTransform->y + tipDistance * std::sin(weaponTransform->rotation);
    
    return engine::Vector2{tipX, tipY};
}

} // namespace ZombieSurvivor::System