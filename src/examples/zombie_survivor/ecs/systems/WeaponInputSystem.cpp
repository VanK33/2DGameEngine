// src/examples/zombie_survivor/ecs/systems/WeaponInputSystem.cpp

#include "WeaponInputSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include "examples/zombie_survivor/events/GameEventTypes.hpp"
#include <iostream>

namespace ZombieSurvivor::System {

void WeaponInputSystem::Init() {
    std::cout << "[WeaponInputSystem] Initialized" << std::endl;
}

void WeaponInputSystem::Update(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    
    // Get all entities with input components (players)
    auto entities = componentManager.GetEntitiesWithComponent<Component::InputComponent>();
    
    for (const auto& entityId : entities) {
        auto* input = componentManager.GetComponent<Component::InputComponent>(entityId);
        if (!input) continue;
        
        // Skipping non-player entity
        auto* tag = componentManager.GetComponent<engine::ECS::Tag>(entityId);
        if (!tag || tag->tag != "player") {
            continue;
        }
        
        // Process shoot input
        if (input->shootButtonPressed) {
            std::cout << "[WeaponInputSystem] Shoot button pressed for PLAYER " << entityId << std::endl;
            ProcessShootInput(entityId, true);
        }
        
        // Process reload input
        if (input->reloadButtonPressed) {
            ProcessReloadInput(entityId, true);
        }
        
        // Process weapon switch (number keys 1-3)
        if (input->weaponSwitchPressed >= 0) {
            ProcessWeaponSwitch(entityId, input->weaponSwitchPressed);
        }
    }
}

void WeaponInputSystem::Shutdown() {
    std::cout << "[WeaponInputSystem] Shutdown" << std::endl;
}

void WeaponInputSystem::ProcessShootInput(uint32_t playerId, bool shootPressed) {
    if (shootPressed) {
        PublishFireInput(playerId);
    }
}

void WeaponInputSystem::ProcessReloadInput(uint32_t playerId, bool reloadPressed) {
    if (reloadPressed) {
        std::cout << "[WeaponInputSystem] Reload button pressed for player " << playerId << std::endl;
        PublishReloadInput(playerId);
    }
}

void WeaponInputSystem::ProcessWeaponSwitch(uint32_t playerId, int weaponSlot) {
    // Validate weapon slot (0-2 for slots 1-3)
    if (weaponSlot >= 0 && weaponSlot < 3) {
        PublishWeaponSwitchInput(playerId, weaponSlot);
    }
}

void WeaponInputSystem::PublishFireInput(uint32_t playerId) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    auto fireData = std::make_shared<Events::FireInputData>();
    fireData->playerId = playerId;
    fireData->pressed = true;
    
    auto fireEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::FIRE_INPUT,
        std::static_pointer_cast<void>(fireData)
    );
    eventManager.Publish(fireEvent);
    
    // Debug: Published FIRE_INPUT event
}

void WeaponInputSystem::PublishReloadInput(uint32_t playerId) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    auto reloadData = std::make_shared<Events::ReloadInputData>();
    reloadData->playerId = playerId;
    
    auto reloadEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::RELOAD_INPUT,
        std::static_pointer_cast<void>(reloadData)
    );
    eventManager.Publish(reloadEvent);
    
    std::cout << "[WeaponInputSystem] Published RELOAD_INPUT event for player " << playerId 
              << " (type: " << static_cast<int>(Events::GameEventType::RELOAD_INPUT) << ")" << std::endl;
}

void WeaponInputSystem::PublishWeaponSwitchInput(uint32_t playerId, int weaponSlot) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    auto switchData = std::make_shared<Events::WeaponSwitchInputData>();
    switchData->playerId = playerId;
    switchData->weaponSlot = weaponSlot;
    
    auto switchEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::WEAPON_SWITCH_INPUT,
        std::static_pointer_cast<void>(switchData)
    );
    eventManager.Publish(switchEvent);
    
    std::cout << "[WeaponInputSystem] Player " << playerId 
              << " switching to weapon slot " << weaponSlot << std::endl;
}

} // namespace ZombieSurvivor::System