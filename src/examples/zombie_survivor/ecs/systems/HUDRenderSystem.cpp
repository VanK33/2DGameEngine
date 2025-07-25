// src/examples/zombie_survivor/ecs/systems/HUDRenderSystem.cpp

#include "HUDRenderSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include <iostream>
#include <algorithm>

namespace ZombieSurvivor::System {

void HUDRenderSystem::Init() {
    CreateHUDVisuals();
    std::cout << "[HUDRenderSystem] Initialized with visual entity creation" << std::endl;
}

void HUDRenderSystem::Update(float deltaTime) {
    UpdateHUDVisuals();
}

void HUDRenderSystem::Shutdown() {
    for (uint32_t visualEntityId : visualEntities_) {
        CleanupVisualEntity(visualEntityId);
    }
    visualEntities_.clear();
    hudToVisualMap_.clear();
    hudToForegroundMap_.clear();
    
    std::cout << "[HUDRenderSystem] Shutdown and cleaned up visual entities" << std::endl;
}

void HUDRenderSystem::CreateHUDVisuals() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto hudEntities = componentManager.GetEntitiesWithComponent<Component::HUDComponent>();
    
    for (uint32_t hudEntityId : hudEntities) {
        auto* hud = componentManager.GetComponent<Component::HUDComponent>(hudEntityId);
        if (!hud || !hud->visible) continue;
        
        // Skip if visual already exists
        if (hudToVisualMap_.find(hudEntityId) != hudToVisualMap_.end()) continue;
        
        uint32_t visualEntityId = 0;
        
        switch (hud->type) {
            case Component::HUDElementType::HEALTH_BAR:
                visualEntityId = CreateHealthBarVisual(hud);
                break;
            case Component::HUDElementType::AMMO_COUNTER:
                visualEntityId = CreateAmmoCounterVisual(hud);
                break;
            case Component::HUDElementType::EXPERIENCE_BAR:
                visualEntityId = CreateExperienceBarVisual(hud);
                break;
            case Component::HUDElementType::KILL_COUNTER:
                visualEntityId = CreateKillCounterVisual(hud);
                break;
            case Component::HUDElementType::SURVIVAL_TIME:
                visualEntityId = CreateSurvivalTimerVisual(hud);
                break;
            case Component::HUDElementType::CROSSHAIR:
                visualEntityId = CreateCrosshairVisual(hud);
                break;
            default:
                break;
        }
        
        if (visualEntityId != 0) {
            hudToVisualMap_[hudEntityId] = visualEntityId;
            visualEntities_.push_back(visualEntityId);
            
            // For health bars, also track the foreground entity
            if (hud->type == Component::HUDElementType::HEALTH_BAR) {
                if (lastCreatedForegroundEntity_ != 0) {
                    hudToForegroundMap_[hudEntityId] = lastCreatedForegroundEntity_;
                    lastCreatedForegroundEntity_ = 0; // Reset for next use
                }
            }
        }
    }
}

void HUDRenderSystem::UpdateHUDVisuals() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    
    // Update existing visuals (collect invalid entities first to avoid iterator invalidation)
    std::vector<uint32_t> invalidHudEntities;
    
    for (auto& [hudEntityId, visualEntityId] : hudToVisualMap_) {
        if (!IsHUDEntityValid(hudEntityId)) {
            invalidHudEntities.push_back(hudEntityId);
            continue;
        }
        
        auto* hud = GetHUDComponent(hudEntityId);
        if (!hud) continue;
        
        switch (hud->type) {
            case Component::HUDElementType::HEALTH_BAR:
                UpdateHealthBarVisual(visualEntityId, hud);
                break;
            case Component::HUDElementType::AMMO_COUNTER:
                UpdateAmmoCounterVisual(visualEntityId, hud);
                break;
            case Component::HUDElementType::EXPERIENCE_BAR:
                UpdateExperienceBarVisual(visualEntityId, hud);
                break;
            case Component::HUDElementType::KILL_COUNTER:
            case Component::HUDElementType::SURVIVAL_TIME:
                UpdateTextVisual(visualEntityId, hud);
                break;
            default:
                break;
        }
    }
    
    // Clean up invalid entities
    for (uint32_t hudEntityId : invalidHudEntities) {
        // Clean up background entity
        auto it = hudToVisualMap_.find(hudEntityId);
        if (it != hudToVisualMap_.end()) {
            CleanupVisualEntity(it->second);
            hudToVisualMap_.erase(it);
        }
        
        // Clean up foreground entity if it exists
        auto fgIt = hudToForegroundMap_.find(hudEntityId);
        if (fgIt != hudToForegroundMap_.end()) {
            CleanupVisualEntity(fgIt->second);
            hudToForegroundMap_.erase(fgIt);
        }
    }
    
    // Check for new HUD entities
    CreateHUDVisuals();
}

void HUDRenderSystem::SetScreenSize(int width, int height) {
    screenWidth_ = width;
    screenHeight_ = height;
    UpdateLayout();
}

void HUDRenderSystem::UpdateLayout() {
    // Recalculate positions for all HUD elements when screen size changes
    for (auto& [hudEntityId, visualEntityId] : hudToVisualMap_) {
        auto* hud = GetHUDComponent(hudEntityId);
        if (!hud) continue;
        
        auto* world = GetWorld();
        if (!world) continue;
        
        auto& componentManager = world->GetComponentManager();
        auto* transform = componentManager.GetComponent<engine::ECS::Transform2D>(visualEntityId);
        if (!transform) continue;
        
        SDL_Rect screenPos = CalculateScreenPosition(hud->position, hud->bounds.w, hud->bounds.h);
        transform->x = static_cast<float>(screenPos.x);
        transform->y = static_cast<float>(screenPos.y);
    }
}

uint32_t HUDRenderSystem::CreateHealthBarVisual(const Component::HUDComponent* hud) {
    auto* world = GetWorld();
    if (!world) return 0;
    
    auto& entityFactory = world->GetEntityFactory();
    auto& componentManager = world->GetComponentManager();
    
    uint32_t backgroundEntityId = entityFactory.CreateEntity(GenerateVisualName(hud->type, "Background"));
    
    std::cout << "[HUDRenderSystem] Creating health bar with bounds: (" 
              << hud->bounds.x << "," << hud->bounds.y << "," << hud->bounds.w << "," << hud->bounds.h << ")" << std::endl;
    std::cout << "[HUDRenderSystem] HUD position type: " << static_cast<int>(hud->position) << std::endl;
    
    SDL_Rect screenPos;
    
    if (hud->position == Component::HUDPosition::CUSTOM) {
        screenPos = CalculateCustomPosition(hud->bounds);
        std::cout << "[HUDRenderSystem] Using CUSTOM position from bounds" << std::endl;
    } else {
        screenPos = CalculateScreenPosition(hud->position, hud->bounds.w, hud->bounds.h);
        std::cout << "[HUDRenderSystem] Using calculated position for preset position" << std::endl;
    }
    
    std::cout << "[HUDRenderSystem] Final screen position: (" 
              << screenPos.x << "," << screenPos.y << "," << screenPos.w << "," << screenPos.h << ")" << std::endl;
    
    componentManager.AddComponent<engine::ECS::Transform2D>(backgroundEntityId,
        engine::ECS::Transform2D{
            static_cast<float>(screenPos.x),
            static_cast<float>(screenPos.y),
            0.0f,
            1.0f,
            1.0f
        });
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(backgroundEntityId,
        engine::ECS::Sprite2D{
            "pixel.png",
            {0, 0, hud->bounds.w, hud->bounds.h},
            hud->visible,
            {hud->backgroundColor.r, hud->backgroundColor.g, hud->backgroundColor.b, hud->backgroundColor.a},
            ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::UI)
        });
    
    // Create foreground bar
    uint32_t foregroundEntityId = entityFactory.CreateEntity(GenerateVisualName(hud->type, "Foreground"));
    
    componentManager.AddComponent<engine::ECS::Transform2D>(foregroundEntityId,
        engine::ECS::Transform2D{
            static_cast<float>(screenPos.x),
            static_cast<float>(screenPos.y),
            0.0f,
            1.0f,
            1.0f
        });
    
    int fillWidth = static_cast<int>((hud->currentValue / hud->maxValue) * hud->bounds.w);
    SDL_Color barColor = GetStatusColor(hud, hud->currentValue / hud->maxValue);
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(foregroundEntityId,
        engine::ECS::Sprite2D{
            "pixel.png",
            {0, 0, fillWidth, hud->bounds.h},
            hud->visible,
            {barColor.r, barColor.g, barColor.b, barColor.a},
            ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::UI) + 1
        });
    
    visualEntities_.push_back(foregroundEntityId);
    
    // Store the foreground entity for later updates - we'll set this after the background is mapped
    // Note: We can't set hudToForegroundMap_ here because the hudEntityId mapping isn't established yet
    // We'll need to handle this in the calling code
    
    std::cout << "[HUDRenderSystem] Created health bar visual entities (bg: " << backgroundEntityId 
              << ", fg: " << foregroundEntityId << ")" << std::endl;
              
    // Store the foreground entity ID in a temporary way that we can retrieve later
    // We'll create a special mapping right after this method returns
    lastCreatedForegroundEntity_ = foregroundEntityId;
    
    return backgroundEntityId;
}

uint32_t HUDRenderSystem::CreateAmmoCounterVisual(const Component::HUDComponent* hud) {
    auto* world = GetWorld();
    if (!world) return 0;
    
    auto& entityFactory = world->GetEntityFactory();
    auto& componentManager = world->GetComponentManager();
    
    uint32_t visualEntityId = entityFactory.CreateEntity(GenerateVisualName(hud->type));
    
    SDL_Rect screenPos = CalculateScreenPosition(hud->position, hud->bounds.w, hud->bounds.h);
    
    componentManager.AddComponent<engine::ECS::Transform2D>(visualEntityId,
        engine::ECS::Transform2D{
            static_cast<float>(screenPos.x),
            static_cast<float>(screenPos.y),
            0.0f,
            1.0f,
            1.0f
        });
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(visualEntityId,
        engine::ECS::Sprite2D{
            "pixel.png",
            {0, 0, hud->bounds.w, hud->bounds.h},
            hud->visible,
            {hud->backgroundColor.r, hud->backgroundColor.g, hud->backgroundColor.b, hud->backgroundColor.a},
            ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::UI)
        });
    
    std::cout << "[HUDRenderSystem] Created ammo counter visual entity" << std::endl;
    return visualEntityId;
}

uint32_t HUDRenderSystem::CreateExperienceBarVisual(const Component::HUDComponent* hud) {
    auto* world = GetWorld();
    if (!world) return 0;
    
    auto& entityFactory = world->GetEntityFactory();
    auto& componentManager = world->GetComponentManager();
    
    uint32_t visualEntityId = entityFactory.CreateEntity(GenerateVisualName(hud->type));
    
    std::cout << "[HUDRenderSystem] Creating experience bar with bounds: (" 
              << hud->bounds.x << "," << hud->bounds.y << "," << hud->bounds.w << "," << hud->bounds.h << ")" << std::endl;
    
    SDL_Rect screenPos;
    
    if (hud->position == Component::HUDPosition::CUSTOM) {
        screenPos = CalculateCustomPosition(hud->bounds);
        std::cout << "[HUDRenderSystem] Experience bar using CUSTOM position from bounds" << std::endl;
    } else {
        screenPos = CalculateScreenPosition(hud->position, hud->bounds.w, hud->bounds.h);
        std::cout << "[HUDRenderSystem] Experience bar using calculated position for preset position" << std::endl;
    }
    
    std::cout << "[HUDRenderSystem] Experience bar final screen position: (" 
              << screenPos.x << "," << screenPos.y << "," << screenPos.w << "," << screenPos.h << ")" << std::endl;
    
    componentManager.AddComponent<engine::ECS::Transform2D>(visualEntityId,
        engine::ECS::Transform2D{
            static_cast<float>(screenPos.x),
            static_cast<float>(screenPos.y),
            0.0f,
            1.0f,
            1.0f
        });
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(visualEntityId,
        engine::ECS::Sprite2D{
            "pixel.png",
            {0, 0, hud->bounds.w, hud->bounds.h},
            hud->visible,
            {hud->foregroundColor.r, hud->foregroundColor.g, hud->foregroundColor.b, hud->foregroundColor.a},
            ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::UI)
        });
    
    std::cout << "[HUDRenderSystem] Created experience bar visual entity" << std::endl;
    return visualEntityId;
}

uint32_t HUDRenderSystem::CreateKillCounterVisual(const Component::HUDComponent* hud) {
    auto* world = GetWorld();
    if (!world) return 0;
    
    auto& entityFactory = world->GetEntityFactory();
    auto& componentManager = world->GetComponentManager();
    
    uint32_t visualEntityId = entityFactory.CreateEntity(GenerateVisualName(hud->type));
    
    SDL_Rect screenPos = CalculateScreenPosition(hud->position, hud->bounds.w, hud->bounds.h);
    
    componentManager.AddComponent<engine::ECS::Transform2D>(visualEntityId,
        engine::ECS::Transform2D{
            static_cast<float>(screenPos.x),
            static_cast<float>(screenPos.y),
            0.0f,
            1.0f,
            1.0f
        });
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(visualEntityId,
        engine::ECS::Sprite2D{
            "pixel.png",
            {0, 0, hud->bounds.w, hud->bounds.h},
            hud->visible,
            {hud->backgroundColor.r, hud->backgroundColor.g, hud->backgroundColor.b, hud->backgroundColor.a},
            ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::UI)
        });
    
    std::cout << "[HUDRenderSystem] Created kill counter visual entity" << std::endl;
    return visualEntityId;
}

uint32_t HUDRenderSystem::CreateSurvivalTimerVisual(const Component::HUDComponent* hud) {
    auto* world = GetWorld();
    if (!world) return 0;
    
    auto& entityFactory = world->GetEntityFactory();
    auto& componentManager = world->GetComponentManager();
    
    uint32_t visualEntityId = entityFactory.CreateEntity(GenerateVisualName(hud->type));
    
    SDL_Rect screenPos = CalculateScreenPosition(hud->position, hud->bounds.w, hud->bounds.h);
    
    componentManager.AddComponent<engine::ECS::Transform2D>(visualEntityId,
        engine::ECS::Transform2D{
            static_cast<float>(screenPos.x),
            static_cast<float>(screenPos.y),
            0.0f,
            1.0f,
            1.0f
        });
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(visualEntityId,
        engine::ECS::Sprite2D{
            "pixel.png",
            {0, 0, hud->bounds.w, hud->bounds.h},
            hud->visible,
            {hud->backgroundColor.r, hud->backgroundColor.g, hud->backgroundColor.b, hud->backgroundColor.a},
            ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::UI)
        });
    
    std::cout << "[HUDRenderSystem] Created survival timer visual entity" << std::endl;
    return visualEntityId;
}

uint32_t HUDRenderSystem::CreateCrosshairVisual(const Component::HUDComponent* hud) {
    auto* world = GetWorld();
    if (!world) return 0;
    
    auto& entityFactory = world->GetEntityFactory();
    auto& componentManager = world->GetComponentManager();
    
    uint32_t visualEntityId = entityFactory.CreateEntity(GenerateVisualName(hud->type));
    
    // Center crosshair on screen
    SDL_Rect screenPos = {
        screenWidth_ / 2 - hud->crosshairSize / 2,
        screenHeight_ / 2 - hud->crosshairSize / 2,
        hud->crosshairSize,
        hud->crosshairSize
    };
    
    componentManager.AddComponent<engine::ECS::Transform2D>(visualEntityId,
        engine::ECS::Transform2D{
            static_cast<float>(screenPos.x),
            static_cast<float>(screenPos.y),
            0.0f,
            1.0f,
            1.0f
        });
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(visualEntityId,
        engine::ECS::Sprite2D{
            "pixel.png",
            {0, 0, hud->crosshairSize, hud->crosshairSize},
            hud->visible,
            {hud->crosshairColor.r, hud->crosshairColor.g, hud->crosshairColor.b, hud->crosshairColor.a},
            ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::UI)
        });
    
    std::cout << "[HUDRenderSystem] Created crosshair visual entity" << std::endl;
    return visualEntityId;
}

void HUDRenderSystem::UpdateHealthBarVisual(uint32_t visualEntityId, const Component::HUDComponent* hud) {
    // For health bars, we need to update the foreground entity, not the background
    // Find the HUD entity ID that corresponds to this visual entity
    uint32_t hudEntityId = 0;
    for (const auto& [hudId, bgVisualId] : hudToVisualMap_) {
        if (bgVisualId == visualEntityId) {
            hudEntityId = hudId;
            break;
        }
    }
    
    if (hudEntityId != 0) {
        auto foregroundIt = hudToForegroundMap_.find(hudEntityId);
        if (foregroundIt != hudToForegroundMap_.end()) {
            UpdateBarVisual(foregroundIt->second, hud);
            return;
        }
    }
    
    // Fallback to updating the background entity (shouldn't happen for health bars)
    UpdateBarVisual(visualEntityId, hud);
}

void HUDRenderSystem::UpdateAmmoCounterVisual(uint32_t visualEntityId, const Component::HUDComponent* hud) {
    UpdateTextVisual(visualEntityId, hud);
}

void HUDRenderSystem::UpdateExperienceBarVisual(uint32_t visualEntityId, const Component::HUDComponent* hud) {
    UpdateBarVisual(visualEntityId, hud);
}

void HUDRenderSystem::UpdateTextVisual(uint32_t visualEntityId, const Component::HUDComponent* hud) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* sprite = componentManager.GetComponent<engine::ECS::Sprite2D>(visualEntityId);
    if (!sprite) return;
    
    sprite->visible = hud->visible;
    
    // Update color based on status
    float percentage = (hud->maxValue > 0) ? (hud->currentValue / hud->maxValue) : 1.0f;
    SDL_Color statusColor = GetStatusColor(hud, percentage);
    sprite->tint = statusColor;
}

void HUDRenderSystem::UpdateBarVisual(uint32_t visualEntityId, const Component::HUDComponent* hud) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* sprite = componentManager.GetComponent<engine::ECS::Sprite2D>(visualEntityId);
    if (!sprite) return;
    
    sprite->visible = hud->visible;
    
    // Update bar fill based on current value
    if (hud->maxValue > 0) {
        float percentage = hud->displayValue / hud->maxValue;
        int fillWidth = static_cast<int>(percentage * hud->bounds.w);
        sprite->sourceRect.w = fillWidth;
        
        // Update color based on percentage
        SDL_Color statusColor = GetStatusColor(hud, percentage);
        sprite->tint = statusColor;
    }
}

SDL_Rect HUDRenderSystem::CalculateScreenPosition(Component::HUDPosition position, int width, int height) const {
    SDL_Rect rect = {0, 0, width, height};
    
    switch (position) {
        case Component::HUDPosition::TOP_LEFT:
            rect.x = 10;
            rect.y = 10;
            break;
        case Component::HUDPosition::TOP_CENTER:
            rect.x = (screenWidth_ - width) / 2;
            rect.y = 10;
            break;
        case Component::HUDPosition::TOP_RIGHT:
            rect.x = screenWidth_ - width - 10;
            rect.y = 10;
            break;
        case Component::HUDPosition::CENTER_LEFT:
            rect.x = 10;
            rect.y = (screenHeight_ - height) / 2;
            break;
        case Component::HUDPosition::CENTER:
            rect.x = (screenWidth_ - width) / 2;
            rect.y = (screenHeight_ - height) / 2;
            break;
        case Component::HUDPosition::CENTER_RIGHT:
            rect.x = screenWidth_ - width - 10;
            rect.y = (screenHeight_ - height) / 2;
            break;
        case Component::HUDPosition::BOTTOM_LEFT:
            rect.x = 10;
            rect.y = screenHeight_ - height - 10;
            break;
        case Component::HUDPosition::BOTTOM_CENTER:
            rect.x = (screenWidth_ - width) / 2;
            rect.y = screenHeight_ - height - 10;
            break;
        case Component::HUDPosition::BOTTOM_RIGHT:
            rect.x = screenWidth_ - width - 10;
            rect.y = screenHeight_ - height - 10;
            break;
        case Component::HUDPosition::CUSTOM:
            // Custom position handled separately
            break;
    }
    
    return rect;
}

SDL_Rect HUDRenderSystem::CalculateCustomPosition(const SDL_Rect& hudBounds) const {
    return hudBounds; // Use provided bounds directly
}

SDL_Color HUDRenderSystem::GetStatusColor(const Component::HUDComponent* hud, float percentage) const {
    if (percentage <= hud->criticalThreshold) {
        return hud->criticalColor;
    }
    return hud->foregroundColor;
}

std::string HUDRenderSystem::GenerateVisualName(Component::HUDElementType type, const std::string& suffix) const {
    std::string baseName;
    
    switch (type) {
        case Component::HUDElementType::HEALTH_BAR:
            baseName = "HealthBarVisual";
            break;
        case Component::HUDElementType::AMMO_COUNTER:
            baseName = "AmmoCounterVisual";
            break;
        case Component::HUDElementType::EXPERIENCE_BAR:
            baseName = "ExperienceBarVisual";
            break;
        case Component::HUDElementType::KILL_COUNTER:
            baseName = "KillCounterVisual";
            break;
        case Component::HUDElementType::SURVIVAL_TIME:
            baseName = "SurvivalTimerVisual";
            break;
        case Component::HUDElementType::CROSSHAIR:
            baseName = "CrosshairVisual";
            break;
        default:
            baseName = "HUDVisual";
            break;
    }
    
    return suffix.empty() ? baseName : baseName + "_" + suffix;
}

void HUDRenderSystem::CleanupVisualEntity(uint32_t visualEntityId) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& entityFactory = world->GetEntityFactory();
    entityFactory.DestroyEntity(visualEntityId);
    
    auto it = std::find(visualEntities_.begin(), visualEntities_.end(), visualEntityId);
    if (it != visualEntities_.end()) {
        visualEntities_.erase(it);
    }
}

bool HUDRenderSystem::IsHUDEntityValid(uint32_t hudEntityId) const {
    auto* world = GetWorld();
    if (!world) return false;
    
    return world->HasEntity(hudEntityId);
}

Component::HUDComponent* HUDRenderSystem::GetHUDComponent(uint32_t hudEntityId) const {
    auto* world = GetWorld();
    if (!world) return nullptr;
    
    auto& componentManager = world->GetComponentManager();
    return componentManager.GetComponent<Component::HUDComponent>(hudEntityId);
}

} // namespace ZombieSurvivor::System