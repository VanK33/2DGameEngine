// src/examples/zombie_survivor/ecs/systems/HUDDataSystem.cpp

#include "HUDDataSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "examples/zombie_survivor/ecs/components/HealthComponent.hpp"
#include "examples/zombie_survivor/ecs/components/AmmoComponent.hpp"
#include "examples/zombie_survivor/ecs/components/ExperienceComponent.hpp"
#include "examples/zombie_survivor/ecs/components/PlayerStatsComponent.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include "examples/zombie_survivor/events/GameEventTypes.hpp"
#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace ZombieSurvivor::System {

void HUDDataSystem::Init() {
    auto* world = GetWorld();
    if (!world) return;
    
    // Initialize UIFactory
    uiFactory_ = std::make_unique<ZombieSurvivor::ECS::UIFactory>(world);
    
    auto& eventManager = world->GetEventManager();
    eventManager.Subscribe(engine::event::EventType::CUSTOM, this);
    
    gameStartTime_ = GetCurrentTime();
    
    std::cout << "[HUDDataSystem] Initialized with UIFactory and subscribed to game events" << std::endl;
}

void HUDDataSystem::Update(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto hudEntities = componentManager.GetEntitiesWithComponent<Component::HUDComponent>();
    
    for (uint32_t hudEntityId : hudEntities) {
        auto* hud = componentManager.GetComponent<Component::HUDComponent>(hudEntityId);
        if (!hud || !hud->visible) continue;
        
        hud->timeSinceUpdate += deltaTime;
        if (hud->timeSinceUpdate >= hud->updateInterval) {
            hud->timeSinceUpdate = 0.0f;
            
            switch (hud->type) {
                case Component::HUDElementType::HEALTH_BAR:
                    UpdateHealthData(hudEntityId, hud->targetEntityId);
                    break;
                case Component::HUDElementType::AMMO_COUNTER:
                    UpdateAmmoData(hudEntityId, hud->targetEntityId);
                    break;
                case Component::HUDElementType::EXPERIENCE_BAR:
                    UpdateExperienceData(hudEntityId, hud->targetEntityId);
                    break;
                case Component::HUDElementType::KILL_COUNTER:
                    UpdateKillCounterData(hudEntityId, hud->targetEntityId);
                    break;
                case Component::HUDElementType::SURVIVAL_TIME:
                    UpdateSurvivalTimerData(hudEntityId);
                    break;
                default:
                    break;
            }
        }
        
        if (hud->animateChanges) {
            AnimateValueChange(hud, hud->currentValue, deltaTime);
        }
    }
}

void HUDDataSystem::Shutdown() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    eventManager.Unsubscribe(engine::event::EventType::CUSTOM, this);
    
    std::cout << "[HUDDataSystem] Shutdown and unsubscribed from events" << std::endl;
}

void HUDDataSystem::onEvent(const std::shared_ptr<engine::event::Event>& event) {
    if (!event) return;
    
    switch (event->GetType()) {
        case engine::event::EventType::CUSTOM:
            HandleGameEvent(event);
            break;
        default:
            break;
    }
}

uint32_t HUDDataSystem::CreateHealthHUD(uint32_t targetPlayerId, Component::HUDPosition position) {
    if (!uiFactory_) {
        std::cerr << "[HUDDataSystem] Error: UIFactory not initialized" << std::endl;
        return 0;
    }
    
    return uiFactory_->CreateHealthHUD(targetPlayerId, position);
}

uint32_t HUDDataSystem::CreateAmmoHUD(uint32_t targetPlayerId, Component::HUDPosition position) {
    if (!uiFactory_) {
        std::cerr << "[HUDDataSystem] Error: UIFactory not initialized" << std::endl;
        return 0;
    }
    
    return uiFactory_->CreateAmmoHUD(targetPlayerId, position);
}

uint32_t HUDDataSystem::CreateExperienceHUD(uint32_t targetPlayerId, Component::HUDPosition position) {
    if (!uiFactory_) {
        std::cerr << "[HUDDataSystem] Error: UIFactory not initialized" << std::endl;
        return 0;
    }
    
    return uiFactory_->CreateExperienceHUD(targetPlayerId, position);
}

uint32_t HUDDataSystem::CreateKillCounterHUD(uint32_t targetPlayerId, Component::HUDPosition position) {
    if (!uiFactory_) {
        std::cerr << "[HUDDataSystem] Error: UIFactory not initialized" << std::endl;
        return 0;
    }
    
    return uiFactory_->CreateKillCounterHUD(targetPlayerId, position);
}

uint32_t HUDDataSystem::CreateSurvivalTimerHUD(Component::HUDPosition position) {
    if (!uiFactory_) {
        std::cerr << "[HUDDataSystem] Error: UIFactory not initialized" << std::endl;
        return 0;
    }
    
    return uiFactory_->CreateSurvivalTimerHUD(position);
}

uint32_t HUDDataSystem::CreateCrosshairHUD(Component::HUDPosition position) {
    if (!uiFactory_) {
        std::cerr << "[HUDDataSystem] Error: UIFactory not initialized" << std::endl;
        return 0;
    }
    
    return uiFactory_->CreateCrosshairHUD(position);
}

void HUDDataSystem::UpdateHealthData(uint32_t hudEntityId, uint32_t playerId) {
    if (!ValidatePlayerId(playerId, "UpdateHealthData")) return;
    
    auto* hud = GetHUDComponent(hudEntityId);
    if (!hud) return;
    
    float healthPercentage = GetHealthPercentage(playerId);
    hud->currentValue = healthPercentage;
    hud->text = FormatTime(healthPercentage); // Will be formatted by render system
}

void HUDDataSystem::UpdateAmmoData(uint32_t hudEntityId, uint32_t playerId) {
    if (!ValidatePlayerId(playerId, "UpdateAmmoData")) return;
    
    auto* hud = GetHUDComponent(hudEntityId);
    if (!hud) return;
    
    int currentAmmo = GetCurrentAmmo(playerId);
    int maxAmmo = GetMaxAmmo(playerId);
    
    hud->currentValue = static_cast<float>(currentAmmo);
    hud->maxValue = static_cast<float>(maxAmmo);
    hud->text = FormatAmmoText(currentAmmo, maxAmmo);
}

void HUDDataSystem::UpdateExperienceData(uint32_t hudEntityId, uint32_t playerId) {
    if (!ValidatePlayerId(playerId, "UpdateExperienceData")) return;
    
    auto* hud = GetHUDComponent(hudEntityId);
    if (!hud) return;
    
    float expPercentage = GetExperiencePercentage(playerId);
    hud->currentValue = expPercentage;
    hud->text = "XP: " + std::to_string(static_cast<int>(expPercentage)) + "%";
}

void HUDDataSystem::UpdateKillCounterData(uint32_t hudEntityId, uint32_t playerId) {
    if (!ValidatePlayerId(playerId, "UpdateKillCounterData")) return;
    
    auto* hud = GetHUDComponent(hudEntityId);
    if (!hud) return;
    
    int killCount = GetKillCount(playerId);
    hud->currentValue = static_cast<float>(killCount);
    hud->text = "Kills: " + std::to_string(killCount);
}

void HUDDataSystem::UpdateSurvivalTimerData(uint32_t hudEntityId) {
    auto* hud = GetHUDComponent(hudEntityId);
    if (!hud) return;
    
    float currentTime = GetCurrentTime();
    float survivalTime = currentTime - gameStartTime_;
    hud->currentValue = survivalTime;
    hud->text = "Time: " + FormatTime(survivalTime);
}

float HUDDataSystem::GetHealthPercentage(uint32_t playerId) const {
    auto* world = GetWorld();
    if (!world) return 0.0f;
    
    auto& componentManager = world->GetComponentManager();
    auto* health = componentManager.GetComponent<Component::HealthComponent>(playerId);
    
    if (!health || health->maxHealth <= 0.0f) return 0.0f;
    return (health->health / health->maxHealth) * 100.0f;
}

int HUDDataSystem::GetCurrentAmmo(uint32_t playerId) const {
    auto* world = GetWorld();
    if (!world) return 0;
    
    auto& componentManager = world->GetComponentManager();
    auto* ammo = componentManager.GetComponent<Component::AmmoComponent>(playerId);
    
    return ammo ? ammo->currentAmmo : 0;
}

int HUDDataSystem::GetMaxAmmo(uint32_t playerId) const {
    auto* world = GetWorld();
    if (!world) return 0;
    
    auto& componentManager = world->GetComponentManager();
    auto* ammo = componentManager.GetComponent<Component::AmmoComponent>(playerId);
    
    return ammo ? ammo->totalAmmo : 0;
}

float HUDDataSystem::GetExperiencePercentage(uint32_t playerId) const {
    auto* world = GetWorld();
    if (!world) return 0.0f;
    
    auto& componentManager = world->GetComponentManager();
    auto* exp = componentManager.GetComponent<Component::ExperienceComponent>(playerId);
    
    if (!exp || exp->experienceToNext <= 0) return 0.0f;
    return (static_cast<float>(exp->experience) / exp->experienceToNext) * 100.0f;
}

int HUDDataSystem::GetKillCount(uint32_t playerId) const {
    auto* world = GetWorld();
    if (!world) return 0;
    
    auto& componentManager = world->GetComponentManager();
    auto* stats = componentManager.GetComponent<Component::PlayerStatsComponent>(playerId);
    
    return stats ? stats->killCount : 0;
}

float HUDDataSystem::GetCurrentTime() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<float>(duration).count();
}

void HUDDataSystem::AnimateValueChange(Component::HUDComponent* hud, float targetValue, float deltaTime) {
    if (!hud) return;
    
    float difference = targetValue - hud->displayValue;
    if (std::abs(difference) < 0.1f) {
        hud->displayValue = targetValue;
        return;
    }
    
    float changeAmount = hud->animationSpeed * deltaTime;
    if (difference > 0) {
        hud->displayValue += std::min(changeAmount, difference);
    } else {
        hud->displayValue -= std::min(changeAmount, -difference);
    }
}

void HUDDataSystem::HandleGameEvent(const std::shared_ptr<engine::event::Event>& event) {
    // Handle specific game events that affect HUD data
    // Implementation would depend on your specific event types
}

void HUDDataSystem::OnHealthChanged(uint32_t playerId) {
    // Refresh health HUD immediately when health changes
    std::cout << "[HUDDataSystem] Health changed for player " << playerId << std::endl;
}

void HUDDataSystem::OnAmmoChanged(uint32_t playerId) {
    // Refresh ammo HUD immediately when ammo changes
    std::cout << "[HUDDataSystem] Ammo changed for player " << playerId << std::endl;
}

void HUDDataSystem::OnExperienceChanged(uint32_t playerId) {
    // Refresh experience HUD immediately when experience changes
    std::cout << "[HUDDataSystem] Experience changed for player " << playerId << std::endl;
}

void HUDDataSystem::OnEnemyKilled(uint32_t playerId) {
    // Refresh kill counter HUD when enemy is killed
    std::cout << "[HUDDataSystem] Enemy killed by player " << playerId << std::endl;
}

std::string HUDDataSystem::FormatTime(float timeInSeconds) const {
    int minutes = static_cast<int>(timeInSeconds) / 60;
    int seconds = static_cast<int>(timeInSeconds) % 60;
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes << ":" 
        << std::setfill('0') << std::setw(2) << seconds;
    return oss.str();
}

std::string HUDDataSystem::FormatAmmoText(int current, int max) const {
    return std::to_string(current) + "/" + std::to_string(max);
}

bool HUDDataSystem::ValidatePlayerId(uint32_t playerId, const std::string& operation) const {
    if (playerId == 0) {
        std::cout << "[HUDDataSystem] Invalid player ID in " << operation << std::endl;
        return false;
    }
    return true;
}

Component::HUDComponent* HUDDataSystem::GetHUDComponent(uint32_t hudEntityId) const {
    auto* world = GetWorld();
    if (!world) return nullptr;
    
    auto& componentManager = world->GetComponentManager();
    return componentManager.GetComponent<Component::HUDComponent>(hudEntityId);
}

} // namespace ZombieSurvivor::System