// src/examples/zombie_survivor/ecs/UIFactory.cpp

#include "UIFactory.hpp"
#include <iostream>

namespace ZombieSurvivor::ECS {

UIFactory::UIFactory(engine::ECS::World* world) : world_(world) {
    if (!ValidateWorld()) {
        std::cerr << "[UIFactory] Error: Invalid world provided to UIFactory" << std::endl;
    }
}

uint32_t UIFactory::CreateHealthHUD(uint32_t targetPlayerId, Component::HUDPosition position) {
    if (!ValidateTargetEntity(targetPlayerId)) return 0;
    
    SDL_Rect bounds = CalculatePosition(position, 200, 20);
    uint32_t hudEntityId = CreateBaseHUDEntity("HealthHUD", Component::HUDElementType::HEALTH_BAR, 
                                               position, bounds, targetPlayerId);
    
    auto& componentManager = world_->GetComponentManager();
    auto* hud = componentManager.GetComponent<Component::HUDComponent>(hudEntityId);
    if (hud) {
        hud->foregroundColor = {0, 255, 0, 255};        // Green
        hud->criticalColor = {255, 0, 0, 255};          // Red
        hud->criticalThreshold = 0.25f;                 // 25% health
        hud->textFormat = "Health: {0}%";
        hud->showNumbers = true;
        hud->animateChanges = true;
        hud->animationSpeed = 5.0f;
    }
    
    std::cout << "[UIFactory] Created health HUD for player " << targetPlayerId << std::endl;
    return hudEntityId;
}

uint32_t UIFactory::CreateAmmoHUD(uint32_t targetPlayerId, Component::HUDPosition position) {
    if (!ValidateTargetEntity(targetPlayerId)) return 0;
    
    SDL_Rect bounds = CalculatePosition(position, 200, 40);
    uint32_t hudEntityId = CreateBaseHUDEntity("AmmoHUD", Component::HUDElementType::AMMO_COUNTER,
                                               position, bounds, targetPlayerId);
    
    auto& componentManager = world_->GetComponentManager();
    auto* hud = componentManager.GetComponent<Component::HUDComponent>(hudEntityId);
    if (hud) {
        hud->foregroundColor = {255, 255, 255, 255};    // White
        hud->criticalColor = {255, 165, 0, 255};        // Orange
        hud->criticalThreshold = 0.25f;                 // 25% ammo
        hud->textFormat = "Ammo: {0}/{1}";
        hud->showNumbers = true;
        hud->animateChanges = true;
        hud->animationSpeed = 8.0f;
    }
    
    std::cout << "[UIFactory] Created ammo HUD for player " << targetPlayerId << std::endl;
    return hudEntityId;
}

uint32_t UIFactory::CreateExperienceHUD(uint32_t targetPlayerId, Component::HUDPosition position) {
    if (!ValidateTargetEntity(targetPlayerId)) return 0;
    
    SDL_Rect bounds = CalculatePosition(position, 200, 15);
    uint32_t hudEntityId = CreateBaseHUDEntity("ExperienceHUD", Component::HUDElementType::EXPERIENCE_BAR,
                                               position, bounds, targetPlayerId);
    
    auto& componentManager = world_->GetComponentManager();
    auto* hud = componentManager.GetComponent<Component::HUDComponent>(hudEntityId);
    if (hud) {
        hud->foregroundColor = {0, 191, 255, 255};      // Blue
        hud->criticalColor = {255, 215, 0, 255};        // Gold
        hud->textFormat = "XP: {0}%";
        hud->showPercentage = true;
        hud->showNumbers = false;
        hud->animateChanges = true;
        hud->animationSpeed = 10.0f;
    }
    
    std::cout << "[UIFactory] Created experience HUD for player " << targetPlayerId << std::endl;
    return hudEntityId;
}

uint32_t UIFactory::CreateKillCounterHUD(uint32_t targetPlayerId, Component::HUDPosition position) {
    if (!ValidateTargetEntity(targetPlayerId)) return 0;
    
    SDL_Rect bounds = CalculatePosition(position, 180, 25);
    uint32_t hudEntityId = CreateBaseHUDEntity("KillCounterHUD", Component::HUDElementType::KILL_COUNTER,
                                               position, bounds, targetPlayerId);
    
    auto& componentManager = world_->GetComponentManager();
    auto* hud = componentManager.GetComponent<Component::HUDComponent>(hudEntityId);
    if (hud) {
        hud->foregroundColor = {255, 255, 255, 255};    // White
        hud->criticalColor = {255, 215, 0, 255};        // Gold
        hud->textFormat = "Kills: {0}";
        hud->showNumbers = true;
        hud->animateChanges = false;
        hud->updateInterval = 0.2f;
    }
    
    std::cout << "[UIFactory] Created kill counter HUD for player " << targetPlayerId << std::endl;
    return hudEntityId;
}

uint32_t UIFactory::CreateSurvivalTimerHUD(Component::HUDPosition position) {
    SDL_Rect bounds = CalculatePosition(position, 100, 25);
    uint32_t hudEntityId = CreateBaseHUDEntity("SurvivalTimerHUD", Component::HUDElementType::SURVIVAL_TIME,
                                               position, bounds, 0);
    
    auto& componentManager = world_->GetComponentManager();
    auto* hud = componentManager.GetComponent<Component::HUDComponent>(hudEntityId);
    if (hud) {
        hud->foregroundColor = {255, 255, 255, 255};    // White
        hud->textFormat = "Time: {0}";
        hud->showNumbers = false;
        hud->animateChanges = false;
        hud->updateInterval = 1.0f;                     // Update every second
    }
    
    std::cout << "[UIFactory] Created survival timer HUD" << std::endl;
    return hudEntityId;
}

uint32_t UIFactory::CreateCrosshairHUD(Component::HUDPosition position) {
    SDL_Rect bounds = {screenWidth_ / 2 - 10, screenHeight_ / 2 - 10, 20, 20};
    uint32_t hudEntityId = CreateBaseHUDEntity("CrosshairHUD", Component::HUDElementType::CROSSHAIR,
                                               position, bounds, 0);
    
    auto& componentManager = world_->GetComponentManager();
    auto* hud = componentManager.GetComponent<Component::HUDComponent>(hudEntityId);
    if (hud) {
        hud->crosshairSize = 20;
        hud->crosshairThickness = 2;
        hud->crosshairColor = {255, 255, 255, 180};     // Semi-transparent white
        hud->animateChanges = false;
    }
    
    std::cout << "[UIFactory] Created crosshair HUD" << std::endl;
    return hudEntityId;
}

uint32_t UIFactory::CreateButton(const std::string& text, SDL_Rect bounds, std::function<void()> callback) {
    uint32_t uiEntityId = CreateBaseUIEntity("Button_" + text, Component::UIElementType::BUTTON, bounds);
    
    auto& componentManager = world_->GetComponentManager();
    auto* ui = componentManager.GetComponent<Component::UIComponent>(uiEntityId);
    if (ui) {
        ui->text = text;
        ui->interactive = true;
        ui->onClickCallback = callback;
        ui->backgroundColor = {64, 64, 64, 200};        // Gray
        ui->hoverColor = {96, 96, 96, 200};             // Lighter gray
        ui->pressedColor = {32, 32, 32, 200};           // Darker gray
        ui->textColor = {255, 255, 255, 255};           // White text
        ui->fontSize = 16;
    }
    
    std::cout << "[UIFactory] Created button: " << text << std::endl;
    return uiEntityId;
}

uint32_t UIFactory::CreateLabel(const std::string& text, SDL_Rect bounds) {
    uint32_t uiEntityId = CreateBaseUIEntity("Label_" + text, Component::UIElementType::LABEL, bounds);
    
    auto& componentManager = world_->GetComponentManager();
    auto* ui = componentManager.GetComponent<Component::UIComponent>(uiEntityId);
    if (ui) {
        ui->text = text;
        ui->interactive = false;
        ui->backgroundColor = {0, 0, 0, 0};             // Transparent
        ui->textColor = {255, 255, 255, 255};           // White text
        ui->fontSize = 14;
    }
    
    std::cout << "[UIFactory] Created label: " << text << std::endl;
    return uiEntityId;
}

uint32_t UIFactory::CreatePanel(SDL_Rect bounds, SDL_Color backgroundColor) {
    uint32_t uiEntityId = CreateBaseUIEntity("Panel", Component::UIElementType::PANEL, bounds);
    
    auto& componentManager = world_->GetComponentManager();
    auto* ui = componentManager.GetComponent<Component::UIComponent>(uiEntityId);
    if (ui) {
        ui->interactive = false;
        ui->backgroundColor = backgroundColor;
        ui->text = "";
    }
    
    std::cout << "[UIFactory] Created panel" << std::endl;
    return uiEntityId;
}

uint32_t UIFactory::CreateProgressBar(SDL_Rect bounds, float progress) {
    uint32_t uiEntityId = CreateBaseUIEntity("ProgressBar", Component::UIElementType::PROGRESS_BAR, bounds);
    
    auto& componentManager = world_->GetComponentManager();
    auto* ui = componentManager.GetComponent<Component::UIComponent>(uiEntityId);
    if (ui) {
        ui->interactive = false;
        ui->progress = progress;
        ui->backgroundColor = {64, 64, 64, 200};        // Gray background
        ui->progressColor = {0, 255, 0, 255};           // Green progress
    }
    
    std::cout << "[UIFactory] Created progress bar" << std::endl;
    return uiEntityId;
}

void UIFactory::SetScreenSize(int width, int height) {
    screenWidth_ = width;
    screenHeight_ = height;
}

SDL_Rect UIFactory::CalculatePosition(Component::HUDPosition position, int width, int height) const {
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
            // Custom position should be handled separately
            break;
    }
    
    return rect;
}

uint32_t UIFactory::CreateBaseHUDEntity(const std::string& name, Component::HUDElementType type,
                                        Component::HUDPosition position, SDL_Rect bounds, uint32_t targetEntityId) {
    if (!ValidateWorld()) return 0;
    
    auto& entityFactory = world_->GetEntityFactory();
    auto& componentManager = world_->GetComponentManager();
    
    uint32_t entityId = entityFactory.CreateEntity(name);
    Component::HUDComponent hud = CreateDefaultHUDComponent(type, position, bounds, targetEntityId);
    componentManager.AddComponent<Component::HUDComponent>(entityId, hud);
    
    return entityId;
}

uint32_t UIFactory::CreateBaseUIEntity(const std::string& name, Component::UIElementType type, SDL_Rect bounds) {
    if (!ValidateWorld()) return 0;
    
    auto& entityFactory = world_->GetEntityFactory();
    auto& componentManager = world_->GetComponentManager();
    
    uint32_t entityId = entityFactory.CreateEntity(name);
    Component::UIComponent ui = CreateDefaultUIComponent(type, bounds);
    componentManager.AddComponent<Component::UIComponent>(entityId, ui);
    
    return entityId;
}

Component::HUDComponent UIFactory::CreateDefaultHUDComponent(Component::HUDElementType type, 
                                                             Component::HUDPosition position, SDL_Rect bounds, uint32_t targetEntityId) {
    return Component::HUDComponent{
        type,
        position,
        bounds,
        true,                                           // visible
        ToInt(RenderLayer::UI),                         // renderLayer - using unified enum
        targetEntityId,
        "",                                             // componentProperty
        {0, 0, 0, 150},                                 // backgroundColor
        {255, 255, 255, 255},                           // foregroundColor
        {255, 0, 0, 255},                               // criticalColor
        "",                                             // text
        "",                                             // textFormat
        14,                                             // fontSize
        0.0f,                                           // currentValue
        100.0f,                                         // maxValue
        false,                                          // showPercentage
        true,                                           // showNumbers
        0.25f,                                          // criticalThreshold
        true,                                           // animateChanges
        5.0f,                                           // animationSpeed
        0.0f,                                           // displayValue
        0.1f,                                           // updateInterval
        0.0f,                                           // timeSinceUpdate
        20,                                             // crosshairSize
        2,                                              // crosshairThickness
        {255, 255, 255, 180},                           // crosshairColor
        false,                                          // autoHide
        3.0f,                                           // hideDelay
        0.0f                                            // timeSinceActivity
    };
}

Component::UIComponent UIFactory::CreateDefaultUIComponent(Component::UIElementType type, SDL_Rect bounds) {
    return Component::UIComponent{
        type,
        Component::UIState::NORMAL,
        bounds,
        true,                                           // visible
        true,                                           // interactive
        ToInt(RenderLayer::UI),                         // renderLayer - using unified enum
        "",                                             // text
        {255, 255, 255, 255},                           // textColor
        16,                                             // fontSize
        {64, 64, 64, 200},                              // backgroundColor
        {96, 96, 96, 200},                              // hoverColor
        {32, 32, 32, 200},                              // pressedColor
        0.0f,                                           // progress
        {0, 255, 0, 255},                               // progressColor
        nullptr,                                        // onClickCallback
        nullptr,                                        // onHoverCallback
        "",                                             // elementId
        false,                                          // anchorToScreenEdge
        {0.0f, 0.0f}                                    // anchorOffset
    };
}

bool UIFactory::ValidateWorld() const {
    return world_ != nullptr;
}

bool UIFactory::ValidateTargetEntity(uint32_t targetEntityId) const {
    if (!ValidateWorld()) return false;
    
    if (targetEntityId == 0) {
        std::cout << "[UIFactory] Warning: Target entity ID is 0" << std::endl;
        return true; // Allow 0 for some HUD elements like timer
    }
    
    return world_->HasEntity(targetEntityId);
}

} // namespace ZombieSurvivor::ECS