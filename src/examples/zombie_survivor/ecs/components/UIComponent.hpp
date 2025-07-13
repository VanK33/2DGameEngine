// src/examples/zombie_survivor/ecs/components/UIComponent.hpp
#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <functional>
#include "engine/core/Types.hpp"

namespace ZombieSurvivor::Component {

enum class UIElementType {
    BUTTON = 0,
    LABEL,
    PANEL,
    IMAGE,
    PROGRESS_BAR,
    TEXT_INPUT
};

enum class UIState {
    NORMAL = 0,
    HOVERED,
    PRESSED,
    DISABLED
};

struct UIComponent {
    UIElementType type = UIElementType::BUTTON;
    UIState state = UIState::NORMAL;
    
    // Screen position and size (in pixels)
    SDL_Rect bounds = {0, 0, 100, 30};
    
    // Visual properties
    bool visible = true;
    bool interactive = true;
    int renderLayer = 20; // UI layer from your RenderLayer enum
    
    // Text properties
    std::string text = "";
    SDL_Color textColor = {255, 255, 255, 255}; // White text
    int fontSize = 16;
    
    // Background properties  
    SDL_Color backgroundColor = {64, 64, 64, 200}; // Semi-transparent gray
    SDL_Color hoverColor = {96, 96, 96, 200};      // Lighter gray on hover
    SDL_Color pressedColor = {32, 32, 32, 200};    // Darker gray when pressed
    
    // Progress bar specific (when type == PROGRESS_BAR)
    float progress = 0.0f;        // 0.0 to 1.0
    SDL_Color progressColor = {0, 255, 0, 255}; // Green progress fill
    
    // Interaction callbacks (optional - for more complex UI)
    std::function<void()> onClickCallback = nullptr;
    std::function<void()> onHoverCallback = nullptr;
    
    // UI element ID for identification
    std::string elementId = "";
    
    // Anchor properties for responsive positioning
    bool anchorToScreenEdge = false;
    engine::Vector2 anchorOffset{0.0f, 0.0f}; // Offset from anchor point
};

} // namespace ZombieSurvivor::Component