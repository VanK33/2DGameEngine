// src/examples/zombie_survivor/ecs/UIFactory.hpp

#pragma once

#include "engine/core/ecs/World.hpp"
#include "examples/zombie_survivor/ecs/components/UIComponent.hpp"
#include "examples/zombie_survivor/ecs/components/HUDComponent.hpp"
#include "examples/zombie_survivor/ecs/RenderLayer.hpp"
#include <memory>

namespace ZombieSurvivor::ECS {

class UIFactory {
public:
    explicit UIFactory(engine::ECS::World* world);
    ~UIFactory() = default;
    
    // HUD element creation methods
    uint32_t CreateHealthHUD(uint32_t targetPlayerId, Component::HUDPosition position = Component::HUDPosition::TOP_LEFT);
    uint32_t CreateAmmoHUD(uint32_t targetPlayerId, Component::HUDPosition position = Component::HUDPosition::BOTTOM_RIGHT);
    uint32_t CreateExperienceHUD(uint32_t targetPlayerId, Component::HUDPosition position = Component::HUDPosition::BOTTOM_CENTER);
    uint32_t CreateKillCounterHUD(uint32_t targetPlayerId, Component::HUDPosition position = Component::HUDPosition::TOP_RIGHT);
    uint32_t CreateSurvivalTimerHUD(Component::HUDPosition position = Component::HUDPosition::TOP_CENTER);
    uint32_t CreateCrosshairHUD(Component::HUDPosition position = Component::HUDPosition::CENTER);
    
    // UI element creation methods  
    uint32_t CreateButton(const std::string& text, SDL_Rect bounds, std::function<void()> callback = nullptr);
    uint32_t CreateLabel(const std::string& text, SDL_Rect bounds);
    uint32_t CreatePanel(SDL_Rect bounds, SDL_Color backgroundColor = {64, 64, 64, 200});
    uint32_t CreateProgressBar(SDL_Rect bounds, float progress = 0.0f);
    
    // Utility methods
    void SetScreenSize(int width, int height);
    SDL_Rect CalculatePosition(Component::HUDPosition position, int width, int height) const;
    
private:
    engine::ECS::World* world_;
    int screenWidth_ = 800;
    int screenHeight_ = 600;
    
    // Helper methods for common setup
    uint32_t CreateBaseHUDEntity(const std::string& name, Component::HUDElementType type, 
                                 Component::HUDPosition position, SDL_Rect bounds, uint32_t targetEntityId = 0);
    uint32_t CreateBaseUIEntity(const std::string& name, Component::UIElementType type, SDL_Rect bounds);
    
    // Default styling methods
    Component::HUDComponent CreateDefaultHUDComponent(Component::HUDElementType type, 
                                                      Component::HUDPosition position, SDL_Rect bounds, uint32_t targetEntityId);
    Component::UIComponent CreateDefaultUIComponent(Component::UIElementType type, SDL_Rect bounds);
    
    // Validation
    bool ValidateWorld() const;
    bool ValidateTargetEntity(uint32_t targetEntityId) const;
};

} // namespace ZombieSurvivor::ECS