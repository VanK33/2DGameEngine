// src/examples/zombie_survivor/ecs/systems/UpgradeSystem.cpp

#include "UpgradeSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "examples/zombie_survivor/ecs/components/UpgradeComponent.hpp"
#include "examples/zombie_survivor/ecs/components/HealthComponent.hpp"
#include "examples/zombie_survivor/ecs/components/MovementComponent.hpp"
#include "examples/zombie_survivor/ecs/components/WeaponComponent.hpp"
#include "examples/zombie_survivor/events/GameEventTypes.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include <iostream>
#include <algorithm>

namespace ZombieSurvivor::System {

void UpgradeSystem::Init() {
    auto* world = GetWorld();
    if (!world) return;

    auto& eventManager = world->GetEventManager();
    eventManager.Subscribe(engine::event::EventType::CUSTOM, this);

    rng_.seed(std::random_device{}());

    std::cout << "[UpgradeSystem Initialized" << std::endl;
}

void UpgradeSystem::Update(float deltaTime) {
    // Simple system, primarly event-driven
}

void UpgradeSystem::Shutdown() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    eventManager.Unsubscribe(engine::event::EventType::CUSTOM, this);
}

void UpgradeSystem::onEvent(const std::shared_ptr<engine::event::Event>& event) {
    if (event && event->GetType() == engine::event::EventType::CUSTOM) {
        HandleGameEvent(event);
    }
}

void UpgradeSystem::HandleGameEvent(const std::shared_ptr<engine::event::Event>& event) {
    auto gameEvent = std::dynamic_pointer_cast<Events::GameEvent>(event);
    if (!gameEvent) return;

    if (gameEvent->GetGameEventType() == Events::GameEventType::PLAYER_LEVEL_UP) {
        HandleLevelUpEvent(gameEvent->GetData());
    }

}

void UpgradeSystem::HandleLevelUpEvent(const std::shared_ptr<void>& eventData) {
    auto levelUpData = std::static_pointer_cast<Events::PlayerLevelUpData>(eventData);
    if (!levelUpData) return;

    uint32_t playerId = levelUpData->playerId;

    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();
    auto* upgrade = componentManager.GetComponent<Component::UpgradeComponent>(playerId);

    if (upgrade) {
        upgrade->pendingUpgrade = true;
        upgrade->currentOptions = GenerateUpgradeOptions(3);

        std::cout << "[UpgradeSystem] Player " << playerId 
        << " can choose from 3 upgrade options" << std::endl;
    }
}

std::vector<Component::UpgradeType> UpgradeSystem::GenerateUpgradeOptions(int numOptions) {
    std::vector<Component::UpgradeType> allUpgrades = {
        Component::UpgradeType::DAMAGE_BOOST,
        Component::UpgradeType::MOVEMENT_SPEED_BOOST,
        Component::UpgradeType::MAX_HEALTH_BOOST,
        Component::UpgradeType::FIRE_RATE_BOOST,
        Component::UpgradeType::MAGAZINE_SIZE_BOOST
    };
    
    std::shuffle(allUpgrades.begin(), allUpgrades.end(), rng_);
    
    int actualOptions = std::min(numOptions, static_cast<int>(allUpgrades.size()));
    return std::vector<Component::UpgradeType>(allUpgrades.begin(), allUpgrades.begin() + actualOptions);
}

void UpgradeSystem::SelectUpgrade(uint32_t entityId, Component::UpgradeType upgradeType) {
    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();
    auto* upgrade = componentManager.GetComponent<Component::UpgradeComponent>(entityId);

    if (!upgrade || !upgrade->pendingUpgrade) return;

    upgrade->ApplyUpgrade(upgradeType);
    ApplyUpgradeEffect(entityId, upgradeType);

    upgrade->pendingUpgrade = false;
    upgrade->currentOptions.clear();
    
    std::cout << "[UpgradeSystem] Applied " << static_cast<int>(upgradeType) 
              << " upgrade to entity " << entityId << std::endl;
}

void UpgradeSystem::ApplyUpgradeEffect(uint32_t entityId, Component::UpgradeType upgradeType) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    
    switch (upgradeType) {
        case Component::UpgradeType::DAMAGE_BOOST: {
            auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(entityId);
            if (weapon) {
                weapon->damage += 5.0f;
                std::cout << "[UpgradeSystem] Damage increased to " << weapon->damage << std::endl;
            }
            break;
        }
        case Component::UpgradeType::MOVEMENT_SPEED_BOOST: {
            auto* movement = componentManager.GetComponent<Component::MovementComponent>(entityId);
            if (movement) {
                movement->speed += 25.0f;
                std::cout << "[UpgradeSystem] Speed increased to " << movement->speed << std::endl;
            }
            break;
        }
        case Component::UpgradeType::MAX_HEALTH_BOOST: {
            auto* health = componentManager.GetComponent<Component::HealthComponent>(entityId);
            if (health) {
                health->maxHealth += 25.0f;
                health->health += 25.0f;
                std::cout << "[UpgradeSystem] Max health increased to " << health->maxHealth << std::endl;
            }
            break;
        }
        case Component::UpgradeType::FIRE_RATE_BOOST: {
            auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(entityId);
            if (weapon) {
                weapon->fireRate += 1.0f;
                std::cout << "[UpgradeSystem] Fire rate increased to " << weapon->fireRate << std::endl;
            }
            break;
        }
        case Component::UpgradeType::MAGAZINE_SIZE_BOOST: {
            auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(entityId);
            if (weapon) {
                weapon->magazineCapacity += 3;
                std::cout << "[UpgradeSystem] Magazine size increased to " << weapon->magazineCapacity << std::endl;
            }
            break;
        }
    }
}

} // namespace ZombieSurvivor::System