// src/examples/zombie_survivor/ecs/systems/HUDRenderSystem.cpp

#include "HUDRenderSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/Tag.hpp"
#include "examples/zombie_survivor/ecs/components/WeaponComponent.hpp"
#include "examples/zombie_survivor/ecs/components/AmmoComponent.hpp"
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
    
    uint32_t visualEntityId = entityFactory.CreateEntity(GenerateVisualName(hud->type));
    
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
    
    componentManager.AddComponent<engine::ECS::Transform2D>(visualEntityId,
        engine::ECS::Transform2D{
            static_cast<float>(screenPos.x),
            static_cast<float>(screenPos.y),
            0.0f,
            1.0f,
            1.0f
        });
    
    // Create simple health bar (similar to ammo bars)
    float healthPercentage = (hud->maxValue > 0) ? (hud->currentValue / hud->maxValue) : 0.0f;
    int healthBarWidth = static_cast<int>(healthPercentage * 120); // 120px wide like ammo bars
    
    SDL_Color healthColor = {255, 100, 100, 255}; // Fixed red color for health
    
    // Create simple health bar sprite (similar style to ammo bars)
    componentManager.AddComponent<engine::ECS::Sprite2D>(visualEntityId,
        engine::ECS::Sprite2D{
            "pixel.png",
            {0, 0, healthBarWidth, 6}, // 6px high like ammo bars
            hud->visible,
            healthColor,
            ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::UI)
        });
    
    std::cout << "[HUDRenderSystem] Created health bar visual entity (simple style like ammo bars)" << std::endl;
    
    return visualEntityId;
}

uint32_t HUDRenderSystem::CreateAmmoCounterVisual(const Component::HUDComponent* hud) {
    auto* world = GetWorld();
    if (!world) return 0;
    
    auto& entityFactory = world->GetEntityFactory();
    auto& componentManager = world->GetComponentManager();
    
    uint32_t visualEntityId = entityFactory.CreateEntity(GenerateVisualName(hud->type));
    
    std::cout << "[HUDRenderSystem] Creating ammo counter with bounds: (" 
              << hud->bounds.x << "," << hud->bounds.y << "," << hud->bounds.w << "," << hud->bounds.h << ")" << std::endl;
    std::cout << "[HUDRenderSystem] Ammo counter position type: " << static_cast<int>(hud->position) << std::endl;
    
    SDL_Rect screenPos;
    
    if (hud->position == Component::HUDPosition::CUSTOM) {
        screenPos = CalculateCustomPosition(hud->bounds);
        std::cout << "[HUDRenderSystem] Ammo counter using CUSTOM position from bounds" << std::endl;
    } else {
        screenPos = CalculateScreenPosition(hud->position, hud->bounds.w, hud->bounds.h);
        std::cout << "[HUDRenderSystem] Ammo counter using calculated position for preset position" << std::endl;
    }
    
    std::cout << "[HUDRenderSystem] Ammo counter final screen position: (" 
              << screenPos.x << "," << screenPos.y << "," << screenPos.w << "," << screenPos.h << ")" << std::endl;
    
    componentManager.AddComponent<engine::ECS::Transform2D>(visualEntityId,
        engine::ECS::Transform2D{
            static_cast<float>(screenPos.x),
            static_cast<float>(screenPos.y),
            0.0f,
            1.0f,
            1.0f
        });
    
    // Create magazine ammo indicator (top bar) 
    // Need to get magazine capacity from player's weapon component
    auto& compManager = world->GetComponentManager();
    int magazineCapacity = 12; // Default pistol capacity
    
    // Try to get actual magazine capacity from player's weapon
    if (hud->targetEntityId != 0) {
        auto* weapon = compManager.GetComponent<Component::WeaponComponent>(hud->targetEntityId);
        if (weapon) {
            magazineCapacity = weapon->magazineCapacity;
        }
    }
    
    // Magazine indicator: currentAmmo / magazineCapacity
    float magPercentage = (magazineCapacity > 0) ? (hud->currentValue / magazineCapacity) : 0.0f;
    int magBarWidth = static_cast<int>(magPercentage * 120); // 120px wide for better visibility
    
    SDL_Color magColor = {220, 220, 220, 255}; // Fixed light gray color for magazine
    
    // For right-aligned ammo bars, adjust position to grow leftward from right edge
    float rightAlignedX = screenPos.x - magBarWidth; // Start from right edge, move left by bar width
    
    componentManager.AddComponent<engine::ECS::Transform2D>(visualEntityId,
        engine::ECS::Transform2D{
            rightAlignedX, // Right-aligned position
            static_cast<float>(screenPos.y),
            0.0f,
            1.0f,
            1.0f
        });
    
    // Create magazine indicator bar (this visualEntityId represents the magazine bar)
    componentManager.AddComponent<engine::ECS::Sprite2D>(visualEntityId,
        engine::ECS::Sprite2D{
            "pixel.png",
            {0, 0, magBarWidth, 6}, // 6px high magazine bar (increased height)
            hud->visible,
            magColor,
            ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::UI)
        });
    
    // Create reserve ammo indicator (bottom bar) - separate entity
    uint32_t reserveEntityId = entityFactory.CreateEntity(GenerateVisualName(hud->type, "Reserve"));
    
    componentManager.AddComponent<engine::ECS::Transform2D>(reserveEntityId,
        engine::ECS::Transform2D{
            static_cast<float>(screenPos.x),
            static_cast<float>(screenPos.y + 8), // 8px below magazine bar (more spacing)
            0.0f,
            1.0f,
            1.0f
        });
    
    // Reserve ammo indicator: totalAmmo / maxTotalAmmo
    float reservePercentage = (hud->maxValue > 0) ? (hud->maxValue / 300.0f) : 0.0f; // maxValue is totalAmmo, 300 is maxTotalAmmo
    int reserveBarWidth = static_cast<int>(reservePercentage * 120); // 120px wide for reserve (same as magazine)
    
    SDL_Color reserveColor = {150, 180, 255, 255}; // Fixed light blue color for reserve
    
    // Right-align reserve bar too
    float reserveRightAlignedX = screenPos.x - reserveBarWidth;
    
    componentManager.AddComponent<engine::ECS::Transform2D>(reserveEntityId,
        engine::ECS::Transform2D{
            reserveRightAlignedX, // Right-aligned position for reserve bar
            static_cast<float>(screenPos.y + 8), // 8px below magazine bar (more spacing)
            0.0f,
            1.0f,
            1.0f
        });
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(reserveEntityId,
        engine::ECS::Sprite2D{
            "pixel.png",
            {0, 0, reserveBarWidth, 6}, // 6px high reserve bar (increased height)
            hud->visible,
            reserveColor,
            ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::UI)
        });
    
    // Add tag to reserve entity so we can find it during updates
    componentManager.AddComponent<engine::ECS::Tag>(reserveEntityId, 
        engine::ECS::Tag{GenerateVisualName(hud->type, "Reserve")});
    
    // Create reload progress bar (third bar) - separate entity, initially hidden
    uint32_t reloadEntityId = entityFactory.CreateEntity(GenerateVisualName(hud->type, "Reload"));
    
    // Right-align reload bar too (start from right edge initially with 0 width)
    componentManager.AddComponent<engine::ECS::Transform2D>(reloadEntityId,
        engine::ECS::Transform2D{
            static_cast<float>(screenPos.x), // Will be adjusted during reload updates
            static_cast<float>(screenPos.y + 16), // 16px below magazine bar (below reserve bar)
            0.0f,
            1.0f,
            1.0f
        });
    
    // Initially hide the reload bar - it will show only when reloading
    componentManager.AddComponent<engine::ECS::Sprite2D>(reloadEntityId,
        engine::ECS::Sprite2D{
            "pixel.png",
            {0, 0, 0, 6}, // Start with 0 width, 6px high
            false, // Initially hidden
            {255, 200, 100, 255}, // Orange color for reload progress
            ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::UI)
        });
    
    // Add tag to reload entity so we can find it during updates
    componentManager.AddComponent<engine::ECS::Tag>(reloadEntityId, 
        engine::ECS::Tag{GenerateVisualName(hud->type, "Reload")});
    
    // Store both reserve and reload entities for cleanup later
    visualEntities_.push_back(reserveEntityId);
    visualEntities_.push_back(reloadEntityId);
    
    std::cout << "[HUDRenderSystem] Created ammo counter with magazine bar (entity " << visualEntityId 
              << "), reserve bar (entity " << reserveEntityId << "), and reload bar (entity " 
              << reloadEntityId << ")" << std::endl;
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
    
    // Create simple experience bar (similar to ammo bars)
    float expPercentage = (hud->maxValue > 0) ? (hud->currentValue / hud->maxValue) : 0.0f;
    int expBarWidth = static_cast<int>(expPercentage * 120); // 120px wide like ammo bars
    
    SDL_Color expColor = {100, 200, 255, 255}; // Fixed blue color for experience
    
    // Only show experience bar if there's actually experience to show
    bool showExpBar = (expBarWidth > 0);
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(visualEntityId,
        engine::ECS::Sprite2D{
            "pixel.png",
            {0, 0, expBarWidth, 6}, // 6px high like ammo bars
            (hud->visible && showExpBar), // Only visible if there's experience to show
            expColor,
            ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::UI)
        });
    
    std::cout << "[HUDRenderSystem] Created experience bar visual entity (simple style like ammo bars)" << std::endl;
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
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* healthSprite = componentManager.GetComponent<engine::ECS::Sprite2D>(visualEntityId);
    if (!healthSprite) return;
    
    healthSprite->visible = hud->visible;
    
    // Update health bar: currentHealth / maxHealth
    float healthPercentage = (hud->maxValue > 0) ? (hud->currentValue / hud->maxValue) : 0.0f;
    int healthBarWidth = static_cast<int>(healthPercentage * 120); // 120px wide like ammo bars
    healthSprite->sourceRect.w = healthBarWidth;
    
    // Fixed color - no color changes
    SDL_Color healthColor = {255, 100, 100, 255}; // Red for health
    healthSprite->tint = healthColor;
}

void HUDRenderSystem::UpdateAmmoCounterVisual(uint32_t visualEntityId, const Component::HUDComponent* hud) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* magSprite = componentManager.GetComponent<engine::ECS::Sprite2D>(visualEntityId);
    if (!magSprite) return;
    
    magSprite->visible = hud->visible;
    
    // Get magazine capacity from player's weapon
    int magazineCapacity = 12; // Default pistol capacity
    if (hud->targetEntityId != 0) {
        auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(hud->targetEntityId);
        if (weapon) {
            magazineCapacity = weapon->magazineCapacity;
        }
    }
    
    // Update magazine bar: currentAmmo / magazineCapacity
    float magPercentage = (magazineCapacity > 0) ? (hud->currentValue / magazineCapacity) : 0.0f;
    int magBarWidth = static_cast<int>(magPercentage * 120); // 120px wide for better visibility
    magSprite->sourceRect.w = magBarWidth;
    
    // Update position for right-alignment (grow leftward from right edge)
    auto* magTransform = componentManager.GetComponent<engine::ECS::Transform2D>(visualEntityId);
    if (magTransform) {
        // For ammo bars, bounds.x represents the right edge (1181), so grow leftward
        float rightEdgeX = static_cast<float>(hud->bounds.x);
        magTransform->x = rightEdgeX - magBarWidth; // Position bar to grow leftward
    }
    
    // Fixed color - no color changes
    SDL_Color magColor = {220, 220, 220, 255}; // Light gray for magazine
    magSprite->tint = magColor;
    
    // Update reserve bar (look for the next entity ID, which should be the reserve bar)
    // This is a simple approach - we created the reserve entity right after the magazine entity
    uint32_t reserveEntityId = visualEntityId + 1;
    auto* reserveSprite = componentManager.GetComponent<engine::ECS::Sprite2D>(reserveEntityId);
    if (reserveSprite) {
        reserveSprite->visible = hud->visible;
        
        // Update reserve bar: totalAmmo / maxTotalAmmo  
        float reservePercentage = (hud->maxValue > 0) ? (hud->maxValue / 300.0f) : 0.0f;
        int reserveBarWidth = static_cast<int>(reservePercentage * 120); // 120px wide for better visibility
        reserveSprite->sourceRect.w = reserveBarWidth;
        
        // Update position for right-alignment (grow leftward from right edge)
        auto* reserveTransform = componentManager.GetComponent<engine::ECS::Transform2D>(reserveEntityId);
        if (reserveTransform) {
            float rightEdgeX = static_cast<float>(hud->bounds.x);
            reserveTransform->x = rightEdgeX - reserveBarWidth; // Position bar to grow leftward
        }
        
        // Fixed color - no color changes
        SDL_Color reserveColor = {150, 180, 255, 255}; // Light blue for reserve
        reserveSprite->tint = reserveColor;
    }
    
    // Update reload progress bar (third bar - look for reload entity)
    uint32_t reloadEntityId = visualEntityId + 2; // Reserve is +1, reload is +2
    auto* reloadSprite = componentManager.GetComponent<engine::ECS::Sprite2D>(reloadEntityId);
    if (reloadSprite) {
        // Check if player is reloading by looking at AmmoComponent
        bool isReloading = false;
        float reloadProgress = 0.0f;
        
        if (hud->targetEntityId != 0) {
            auto* ammo = componentManager.GetComponent<Component::AmmoComponent>(hud->targetEntityId);
            if (ammo) {
                isReloading = ammo->isReloading;
                reloadProgress = ammo->reloadProgress;
            }
        }
        
        if (isReloading) {
            // Show reload progress bar
            reloadSprite->visible = true;
            int reloadBarWidth = static_cast<int>(reloadProgress * 120); // Same width as other bars
            reloadSprite->sourceRect.w = reloadBarWidth;
            
            // Update position for right-alignment (grow leftward from right edge)
            auto* reloadTransform = componentManager.GetComponent<engine::ECS::Transform2D>(reloadEntityId);
            if (reloadTransform) {
                float rightEdgeX = static_cast<float>(hud->bounds.x);
                reloadTransform->x = rightEdgeX - reloadBarWidth; // Position bar to grow leftward
            }
            
            reloadSprite->tint = {255, 200, 100, 255}; // Orange color for reload
        } else {
            // Hide reload progress bar when not reloading
            reloadSprite->visible = false;
        }
    }
}

void HUDRenderSystem::UpdateExperienceBarVisual(uint32_t visualEntityId, const Component::HUDComponent* hud) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* expSprite = componentManager.GetComponent<engine::ECS::Sprite2D>(visualEntityId);
    if (!expSprite) return;
    
    // Update experience bar: currentExp / maxExp
    float expPercentage = (hud->maxValue > 0) ? (hud->currentValue / hud->maxValue) : 0.0f;
    int expBarWidth = static_cast<int>(expPercentage * 120); // 120px wide like ammo bars
    expSprite->sourceRect.w = expBarWidth;
    
    // Only show experience bar if there's actually experience to show
    bool showExpBar = (expBarWidth > 0);
    expSprite->visible = (hud->visible && showExpBar);
    
    // Fixed color - no color changes
    SDL_Color expColor = {100, 200, 255, 255}; // Blue for experience
    expSprite->tint = expColor;
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