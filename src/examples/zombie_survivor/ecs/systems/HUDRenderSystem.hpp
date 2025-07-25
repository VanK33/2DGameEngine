// src/examples/zombie_survivor/ecs/systems/HUDRenderSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Sprite2D.hpp"
#include "examples/zombie_survivor/ecs/components/HUDComponent.hpp"
#include "examples/zombie_survivor/ecs/RenderLayer.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <SDL3/SDL.h>

namespace ZombieSurvivor::System {

class HUDRenderSystem : public engine::ECS::System {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "HUDRenderSystem"; }
    
    // Visual entity creation methods
    void CreateHUDVisuals();
    void UpdateHUDVisuals();
    
    // Screen management
    void SetScreenSize(int width, int height);
    void UpdateLayout();
    
private:
    int screenWidth_ = 800;
    int screenHeight_ = 600;
    
    // HUD element tracking
    std::unordered_map<uint32_t, uint32_t> hudToVisualMap_; // HUD entity ID -> Visual entity ID
    std::vector<uint32_t> visualEntities_;
    
    // Visual creation methods
    uint32_t CreateHealthBarVisual(const Component::HUDComponent* hud);
    uint32_t CreateAmmoCounterVisual(const Component::HUDComponent* hud);
    uint32_t CreateExperienceBarVisual(const Component::HUDComponent* hud);
    uint32_t CreateKillCounterVisual(const Component::HUDComponent* hud);
    uint32_t CreateSurvivalTimerVisual(const Component::HUDComponent* hud);
    uint32_t CreateCrosshairVisual(const Component::HUDComponent* hud);
    
    // Visual update methods
    void UpdateHealthBarVisual(uint32_t visualEntityId, const Component::HUDComponent* hud);
    void UpdateAmmoCounterVisual(uint32_t visualEntityId, const Component::HUDComponent* hud);
    void UpdateExperienceBarVisual(uint32_t visualEntityId, const Component::HUDComponent* hud);
    void UpdateTextVisual(uint32_t visualEntityId, const Component::HUDComponent* hud);
    void UpdateBarVisual(uint32_t visualEntityId, const Component::HUDComponent* hud);
    
    // Helper methods
    SDL_Rect CalculateScreenPosition(Component::HUDPosition position, int width, int height) const;
    SDL_Rect CalculateCustomPosition(const SDL_Rect& hudBounds) const;
    SDL_Color GetStatusColor(const Component::HUDComponent* hud, float percentage) const;
    std::string GenerateVisualName(Component::HUDElementType type, const std::string& suffix = "") const;
    
    // Management methods
    void CleanupVisualEntity(uint32_t visualEntityId);
    bool IsHUDEntityValid(uint32_t hudEntityId) const;
    Component::HUDComponent* GetHUDComponent(uint32_t hudEntityId) const;
};

} // namespace ZombieSurvivor::System