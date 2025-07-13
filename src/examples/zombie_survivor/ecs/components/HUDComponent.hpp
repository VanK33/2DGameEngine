// src/examples/zombie_survivor/ecs/components/HUDComponent.hpp
#pragma once

#include <SDL3/SDL.h>
#include <string>
#include "engine/core/Types.hpp"

namespace ZombieSurvivor::Component {

enum class HUDElementType {
    HEALTH_BAR = 0,
    AMMO_COUNTER,
    EXPERIENCE_BAR,
    KILL_COUNTER,
    SURVIVAL_TIME,
    CROSSHAIR,
    MINIMAP,
    STATUS_TEXT,
    WEAPON_INFO
};

enum class HUDPosition {
    TOP_LEFT = 0,
    TOP_CENTER,
    TOP_RIGHT,
    CENTER_LEFT,
    CENTER,
    CENTER_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT,
    CUSTOM // Use custom position
};

struct HUDComponent {
    HUDElementType type = HUDElementType::HEALTH_BAR;
    HUDPosition position = HUDPosition::TOP_LEFT;
    
    // Screen position (used when position == CUSTOM)
    SDL_Rect bounds = {10, 10, 200, 20};
    
    // Display properties
    bool visible = true;
    int renderLayer = 20; // UI layer from your RenderLayer enum
    
    // Data binding - what game data this HUD element displays
    uint32_t targetEntityId = 0; // Entity to get data from
    std::string componentProperty = ""; // Property name to display (e.g., "health", "currentAmmo")
    
    // Visual styling
    SDL_Color backgroundColor = {0, 0, 0, 150}; // Semi-transparent black background
    SDL_Color foregroundColor = {255, 255, 255, 255}; // White foreground
    SDL_Color criticalColor = {255, 0, 0, 255};   // Red for critical values (low health/ammo)
    
    // Text properties (for text-based HUD elements)
    std::string text = "";
    std::string textFormat = ""; // Format string (e.g., "Health: {0}/{1}")
    int fontSize = 14;
    
    // Bar properties (for bar-based HUD elements like health/experience)
    float currentValue = 0.0f;
    float maxValue = 100.0f;
    bool showPercentage = false;
    bool showNumbers = true; // Show "50/100" style numbers
    
    // Critical threshold (when to use criticalColor)
    float criticalThreshold = 0.25f; // 25% or below
    
    // Animation properties
    bool animateChanges = true;
    float animationSpeed = 5.0f; // Units per second
    float displayValue = 0.0f;   // Animated display value (different from currentValue during animation)
    
    // Update frequency control
    float updateInterval = 0.1f; // Update every 0.1 seconds
    float timeSinceUpdate = 0.0f;
    
    // Crosshair specific properties
    int crosshairSize = 20;
    int crosshairThickness = 2;
    SDL_Color crosshairColor = {255, 255, 255, 180};
    
    // Auto-hide properties
    bool autoHide = false;
    float hideDelay = 3.0f;      // Seconds of inactivity before hiding
    float timeSinceActivity = 0.0f;
};

} // namespace ZombieSurvivor::Component