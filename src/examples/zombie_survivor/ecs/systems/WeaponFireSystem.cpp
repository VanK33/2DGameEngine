// src/examples/zombie_survivor/ecs/systems/WeaponFireSystem.cpp

#include "WeaponFireSystem.hpp"
#include "WeaponSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include <iostream>

namespace ZombieSurvivor::System {

void WeaponFireSystem::Init() {
    auto* world = GetWorld();
    if (!world) return;

    auto& eventManager = world->GetEventManager();
    eventManager.Subscribe(engine::event::EventType::CUSTOM, this);
    
    std::cout << "[WeaponFireSystem] Initialized" << std::endl;
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
    auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(playerId);
    
    if (!weapon) return Component::AmmoType::NONE;
    
    // Map weapon type to ammo type
    // This is a simple mapping - you might want to store ammo type in WeaponComponent
    switch (weapon->type) {
        case Component::WeaponType::PISTOL:
            return Component::AmmoType::PISTOL;
        case Component::WeaponType::RIFLE:
            return Component::AmmoType::RIFLE;
        case Component::WeaponType::SMG:
            return Component::AmmoType::SMG;
        default:
            return Component::AmmoType::NONE;
    }
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

} // namespace ZombieSurvivor::System