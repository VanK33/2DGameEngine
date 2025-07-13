// src/examples/zombie_survivor/ecs/systems/UIInputSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/event/EventListener.hpp"
#include "engine/core/event/Event.hpp"
#include "examples/zombie_survivor/ecs/components/UIComponent.hpp"
#include <memory>

namespace ZombieSurvivor::System {

class UIInputSystem : public engine::ECS::System, public engine::event::EventListener {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "UIInputSystem"; }
    
    void onEvent(const std::shared_ptr<engine::event::Event>& event) override;
    
    // UI state management
    void SetFocusedElement(uint32_t uiEntityId);
    void ClearFocus();
    uint32_t GetFocusedElement() const { return focusedElementId_; }
    
    // Mouse interaction queries
    bool IsElementHovered(uint32_t uiEntityId) const;
    bool IsElementPressed(uint32_t uiEntityId) const;
    bool WasElementClicked(uint32_t uiEntityId) const;
    
    // Input state management
    void SetMousePosition(int x, int y);
    void GetMousePosition(int& x, int& y) const;
    
private:
    // Input state
    int mouseX_ = 0;
    int mouseY_ = 0;
    bool mouseButtonDown_ = false;
    bool mouseButtonPressed_ = false;  // This frame
    bool mouseButtonReleased_ = false; // This frame
    
    // UI interaction state
    uint32_t hoveredElementId_ = 0;
    uint32_t pressedElementId_ = 0;
    uint32_t focusedElementId_ = 0;
    uint32_t lastClickedElementId_ = 0;
    
    // Frame tracking for click detection
    bool hadClickThisFrame_ = false;
    
    // Input processing methods
    void ProcessMouseInput();
    void ProcessKeyboardInput();
    void UpdateUIStates();
    
    // Mouse interaction methods
    uint32_t FindElementUnderMouse() const;
    bool IsPointInElement(int x, int y, const Component::UIComponent* ui) const;
    void HandleMouseHover(uint32_t elementId);
    void HandleMousePress(uint32_t elementId);
    void HandleMouseRelease(uint32_t elementId);
    void HandleMouseClick(uint32_t elementId);
    
    // UI state update methods
    void UpdateElementState(uint32_t elementId, Component::UIState newState);
    void ResetElementStates();
    
    // Event handling
    void HandleInputEvent(const std::shared_ptr<engine::event::Event>& event);
    void OnMouseButtonDown(int x, int y, uint8_t button);
    void OnMouseButtonUp(int x, int y, uint8_t button);
    void OnMouseMove(int x, int y);
    void OnKeyDown(SDL_Keycode key);
    void OnKeyUp(SDL_Keycode key);
    
    // Callback execution
    void ExecuteClickCallback(uint32_t elementId);
    void ExecuteHoverCallback(uint32_t elementId);
    
    // Validation and utility
    bool IsValidUIElement(uint32_t elementId) const;
    Component::UIComponent* GetUIComponent(uint32_t elementId) const;
    SDL_Rect GetElementScreenBounds(const Component::UIComponent* ui) const;
    
    // Debug methods
    void DebugLogInteraction(const std::string& action, uint32_t elementId) const;
};

} // namespace ZombieSurvivor::System