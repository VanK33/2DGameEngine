// src/examples/zombie_survivor/ecs/systems/WeaponSystem.cpp

#include "WeaponSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include "examples/zombie_survivor/ecs/components/WeaponComponent.hpp"
#include <iostream>
#include <algorithm>

namespace ZombieSurvivor::System {

void WeaponSystem::Init() {
    auto* world = GetWorld();
    if (!world) return;

    auto& eventManager = world->GetEventManager();
    eventManager.Subscribe(engine::event::EventType::CUSTOM, this);

    std::cout << "[WeaponSystem] Initialized" << std::endl;
}

void WeaponSystem::Update(float deltaTime) {
    UpdateWeaponCooldowns(deltaTime);
    UpdateWeaponReloads(deltaTime);
}

void WeaponSystem::Shutdown() {
    auto* world = GetWorld();
    if (!world) return;

    auto& eventManager = world->GetEventManager();
    eventManager.Unsubscribe(engine::event::EventType::CUSTOM, this);

    playerWeaponStates_.clear();
    std::cout << "[WeaponSystem] Shutdown" << std::endl;
}

void WeaponSystem::onEvent(const std::shared_ptr<engine::event::Event>& event) {
    if (event && event->GetType() == engine::event::EventType::CUSTOM) {
        HandleGameEvent(event);
    }
}

bool WeaponSystem::IsWeaponReady(uint32_t playerId) const {
    auto it = playerWeaponStates_.find(playerId);
    if (it == playerWeaponStates_.end()) return true;

    return it->second.cooldown <= 0.0f && !it->second.isReloading;
}

bool WeaponSystem::IsReloading(uint32_t playerId) const {
    auto it = playerWeaponStates_.find(playerId);
    return it != playerWeaponStates_.end() && it->second.isReloading;
}

float WeaponSystem::GetCooldownProgress(uint32_t playerId) const {
    auto it = playerWeaponStates_.find(playerId);
    if (it == playerWeaponStates_.end() || it->second.fireRate <= 0.0f) return 0.0f;
    
    float maxCooldown = 1.0f / it->second.fireRate;
    return it->second.cooldown / maxCooldown;
}

float WeaponSystem::GetReloadProgress(uint32_t playerId) const {
    auto it = playerWeaponStates_.find(playerId);
    if (it == playerWeaponStates_.end() || !it->second.isReloading) return 0.0f;
    
    return std::min(1.0f, it->second.reloadTimer / it->second.reloadTime);
}

void WeaponSystem::UpdateWeaponCooldowns(float deltaTime) {
    for (auto& [playerId, state] : playerWeaponStates_) {
        if (state.cooldown > 0.0f) {
            state.cooldown -= deltaTime;
        }
    }
}

void WeaponSystem::UpdateWeaponReloads(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    for (auto& [playerId, state] : playerWeaponStates_) {
        if (state.isReloading) {
            state.reloadTimer += deltaTime;
            
            if (state.reloadTimer >= state.reloadTime) {
                // Complete reload
                state.isReloading = false;
                state.reloadTimer = 0.0f;
                
                // Notify reload completion
                OnReloadCompleted(playerId);
                
                std::cout << "[WeaponSystem] Player " << playerId << " completed reload" << std::endl;
            }
        }
    }
}

void WeaponSystem::HandleGameEvent(const std::shared_ptr<engine::event::Event>& event) {
    auto gameEvent = std::dynamic_pointer_cast<Events::GameEvent>(event);
    if (!gameEvent) return;
    
    switch (gameEvent->GetGameEventType()) {
        case Events::GameEventType::WEAPON_FIRED: {
            auto data = std::static_pointer_cast<Events::WeaponFiredData>(gameEvent->GetData());
            if (data) {
                OnWeaponFired(data->entityId);
            }
            break;
        }
        case Events::GameEventType::RELOAD_INPUT: {
            auto data = std::static_pointer_cast<Events::ReloadInputData>(gameEvent->GetData());
            if (data) {
                OnReloadStarted(data->playerId);
            }
            break;
        }
        case Events::GameEventType::WEAPON_SWITCHED: {
            auto data = std::static_pointer_cast<Events::WeaponSwitchedData>(gameEvent->GetData());
            if (data) {
                OnWeaponSwitched(data->playerId, data->fireRate, data->reloadTime);
            }
            break;
        }
    }
}

void WeaponSystem::OnWeaponFired(uint32_t playerId) {
    auto& state = GetOrCreateWeaponState(playerId);
    
    // Set cooldown based on fire rate
    state.cooldown = 1.0f / state.fireRate;
    
    std::cout << "[WeaponSystem] Player " << playerId << " fired, cooldown: " 
              << state.cooldown << "s" << std::endl;
}

void WeaponSystem::OnReloadStarted(uint32_t playerId) {
    auto& state = GetOrCreateWeaponState(playerId);
    
    // Check if already reloading
    if (state.isReloading) {
        std::cout << "[WeaponSystem] Player " << playerId << " already reloading" << std::endl;
        return;
    }
    
    // Start reload
    state.isReloading = true;
    state.reloadTimer = 0.0f;
    
    // Publish reload started event
    auto* world = GetWorld();
    if (world) {
        auto& eventManager = world->GetEventManager();
        
        auto reloadData = std::make_shared<Events::ReloadData>();
        reloadData->entityId = playerId;
        reloadData->reloadTime = state.reloadTime;
        
        auto reloadEvent = std::make_shared<Events::GameEvent>(
            Events::GameEventType::RELOAD_STARTED,
            std::static_pointer_cast<void>(reloadData)
        );
        eventManager.Publish(reloadEvent);
    }
    
    std::cout << "[WeaponSystem] Player " << playerId << " started reload, time: " 
              << state.reloadTime << "s" << std::endl;
}

void WeaponSystem::OnReloadCompleted(uint32_t playerId) {
    // Publish reload completed event
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    auto reloadData = std::make_shared<Events::ReloadData>();
    reloadData->entityId = playerId;
    
    auto reloadEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::RELOAD_COMPLETED,
        std::static_pointer_cast<void>(reloadData)
    );
    eventManager.Publish(reloadEvent);
}

void WeaponSystem::OnWeaponSwitched(uint32_t playerId, float fireRate, float reloadTime) {
    auto& state = GetOrCreateWeaponState(playerId);
    
    // Update weapon stats
    state.fireRate = fireRate;
    state.reloadTime = reloadTime;
    
    // Reset states when switching weapon
    state.cooldown = 0.0f;
    state.isReloading = false;
    state.reloadTimer = 0.0f;
    
    std::cout << "[WeaponSystem] Player " << playerId << " switched weapon, "
              << "fireRate: " << fireRate << ", reloadTime: " << reloadTime << std::endl;
}

WeaponSystem::WeaponState& WeaponSystem::GetOrCreateWeaponState(uint32_t playerId) {
    auto it = playerWeaponStates_.find(playerId);
    if (it == playerWeaponStates_.end()) {
        // Get default values from weapon component if available
        WeaponState defaultState;
        
        auto* world = GetWorld();
        if (world) {
            auto& componentManager = world->GetComponentManager();
            auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(playerId);
            if (weapon) {
                defaultState.fireRate = weapon->fireRate;
                defaultState.reloadTime = weapon->reloadTime;
            }
        }
        
        auto [newIt, inserted] = playerWeaponStates_.emplace(playerId, defaultState);
        return newIt->second;
    }
    return it->second;
}

float WeaponSystem::GetWeaponFireRate(uint32_t playerId) const {
    auto it = playerWeaponStates_.find(playerId);
    return it != playerWeaponStates_.end() ? it->second.fireRate : 2.0f;
}

float WeaponSystem::GetWeaponReloadTime(uint32_t playerId) const {
    auto it = playerWeaponStates_.find(playerId);
    return it != playerWeaponStates_.end() ? it->second.reloadTime : 1.5f;
}

} // namespace ZombieSurvivor::System