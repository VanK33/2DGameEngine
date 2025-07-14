// src/examples/zombie_survivor/ecs/systems/UIInputSystem.cpp

#include "UIInputSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "engine/core/event/events/InputEvents.hpp"
#include <iostream>
#include <algorithm>

namespace ZombieSurvivor::System {

void UIInputSystem::Init() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    eventManager.Subscribe(engine::event::EventType::MOUSE_CLICK, this);
    eventManager.Subscribe(engine::event::EventType::MOUSE_MOVE, this);
    eventManager.Subscribe(engine::event::EventType::KEY_DOWN, this);
    eventManager.Subscribe(engine::event::EventType::KEY_UP, this);
    
    std::cout << "[UIInputSystem] Initialized and subscribed to input events" << std::endl;
}

void UIInputSystem::Update(float deltaTime) {
    // Reset frame-specific flags
    mouseButtonPressed_ = false;
    mouseButtonReleased_ = false;
    hadClickThisFrame_ = false;
    lastClickedElementId_ = 0;
    
    ProcessMouseInput();
    ProcessKeyboardInput();
    UpdateUIStates();
}

void UIInputSystem::Shutdown() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    eventManager.Unsubscribe(engine::event::EventType::MOUSE_CLICK, this);
    eventManager.Unsubscribe(engine::event::EventType::MOUSE_MOVE, this);
    eventManager.Unsubscribe(engine::event::EventType::KEY_DOWN, this);
    eventManager.Unsubscribe(engine::event::EventType::KEY_UP, this);
    
    std::cout << "[UIInputSystem] Shutdown and unsubscribed from input events" << std::endl;
}

void UIInputSystem::onEvent(const std::shared_ptr<engine::event::Event>& event) {
    if (!event) return;
    
    switch (event->GetType()) {
        case engine::event::EventType::MOUSE_CLICK:
        case engine::event::EventType::MOUSE_MOVE:
        case engine::event::EventType::KEY_DOWN:
        case engine::event::EventType::KEY_UP:
            HandleInputEvent(event);
            break;
        default:
            break;
    }
}

void UIInputSystem::SetFocusedElement(uint32_t uiEntityId) {
    if (focusedElementId_ != 0) {
        UpdateElementState(focusedElementId_, Component::UIState::NORMAL);
    }
    
    focusedElementId_ = uiEntityId;
    
    if (uiEntityId != 0) {
        auto* ui = GetUIComponent(uiEntityId);
        if (ui && ui->interactive) {
            std::cout << "[UIInputSystem] Set focus to element " << uiEntityId << std::endl;
        }
    }
}

void UIInputSystem::ClearFocus() {
    SetFocusedElement(0);
}

bool UIInputSystem::IsElementHovered(uint32_t uiEntityId) const {
    return hoveredElementId_ == uiEntityId;
}

bool UIInputSystem::IsElementPressed(uint32_t uiEntityId) const {
    return pressedElementId_ == uiEntityId;
}

bool UIInputSystem::WasElementClicked(uint32_t uiEntityId) const {
    return lastClickedElementId_ == uiEntityId;
}

void UIInputSystem::SetMousePosition(int x, int y) {
    mouseX_ = x;
    mouseY_ = y;
}

void UIInputSystem::GetMousePosition(int& x, int& y) const {
    x = mouseX_;
    y = mouseY_;
}

void UIInputSystem::ProcessMouseInput() {
    uint32_t elementUnderMouse = FindElementUnderMouse();
    
    // Handle hover changes
    if (elementUnderMouse != hoveredElementId_) {
        // Clear previous hover
        if (hoveredElementId_ != 0) {
            UpdateElementState(hoveredElementId_, Component::UIState::NORMAL);
        }
        
        // Set new hover
        hoveredElementId_ = elementUnderMouse;
        if (hoveredElementId_ != 0) {
            HandleMouseHover(hoveredElementId_);
        }
    }
    
    // Handle mouse button state changes
    if (mouseButtonPressed_) {
        if (elementUnderMouse != 0) {
            HandleMousePress(elementUnderMouse);
        }
    }
    
    if (mouseButtonReleased_) {
        if (pressedElementId_ != 0) {
            HandleMouseRelease(pressedElementId_);
            
            // Check for click (release on same element as press)
            if (pressedElementId_ == elementUnderMouse) {
                HandleMouseClick(pressedElementId_);
            }
        }
    }
}

void UIInputSystem::ProcessKeyboardInput() {
    // Handle keyboard input for focused element
    // Implementation depends on specific UI element types that need keyboard input
    // For now, this is a placeholder for future text input elements
}

void UIInputSystem::UpdateUIStates() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto uiEntities = componentManager.GetEntitiesWithComponent<Component::UIComponent>();
    
    for (uint32_t entityId : uiEntities) {
        auto* ui = componentManager.GetComponent<Component::UIComponent>(entityId);
        if (!ui || !ui->visible || !ui->interactive) continue;
        
        // Update visual state based on interaction state
        Component::UIState targetState = Component::UIState::NORMAL;
        
        if (entityId == pressedElementId_) {
            targetState = Component::UIState::PRESSED;
        } else if (entityId == hoveredElementId_) {
            targetState = Component::UIState::HOVERED;
        } else if (!ui->interactive) {
            targetState = Component::UIState::DISABLED;
        }
        
        if (ui->state != targetState) {
            ui->state = targetState;
        }
    }
}

uint32_t UIInputSystem::FindElementUnderMouse() const {
    auto* world = GetWorld();
    if (!world) return 0;
    
    auto& componentManager = world->GetComponentManager();
    auto uiEntities = componentManager.GetEntitiesWithComponent<Component::UIComponent>();
    
    // Process in reverse order to handle UI layering (last drawn = top layer)
    for (auto it = uiEntities.rbegin(); it != uiEntities.rend(); ++it) {
        uint32_t entityId = *it;
        auto* ui = componentManager.GetComponent<Component::UIComponent>(entityId);
        
        if (!ui || !ui->visible || !ui->interactive) continue;
        
        if (IsPointInElement(mouseX_, mouseY_, ui)) {
            return entityId;
        }
    }
    
    return 0;
}

bool UIInputSystem::IsPointInElement(int x, int y, const Component::UIComponent* ui) const {
    if (!ui) return false;
    
    SDL_Rect bounds = GetElementScreenBounds(ui);
    return (x >= bounds.x && x < bounds.x + bounds.w &&
            y >= bounds.y && y < bounds.y + bounds.h);
}

void UIInputSystem::HandleMouseHover(uint32_t elementId) {
    UpdateElementState(elementId, Component::UIState::HOVERED);
    ExecuteHoverCallback(elementId);
    
    DebugLogInteraction("Hover", elementId);
}

void UIInputSystem::HandleMousePress(uint32_t elementId) {
    pressedElementId_ = elementId;
    UpdateElementState(elementId, Component::UIState::PRESSED);
    SetFocusedElement(elementId);
    
    DebugLogInteraction("Press", elementId);
}

void UIInputSystem::HandleMouseRelease(uint32_t elementId) {
    UpdateElementState(elementId, Component::UIState::HOVERED);
    pressedElementId_ = 0;
    
    DebugLogInteraction("Release", elementId);
}

void UIInputSystem::HandleMouseClick(uint32_t elementId) {
    lastClickedElementId_ = elementId;
    hadClickThisFrame_ = true;
    ExecuteClickCallback(elementId);
    
    DebugLogInteraction("Click", elementId);
}

void UIInputSystem::UpdateElementState(uint32_t elementId, Component::UIState newState) {
    auto* ui = GetUIComponent(elementId);
    if (ui) {
        ui->state = newState;
    }
}

void UIInputSystem::ResetElementStates() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
        auto uiEntities = componentManager.GetEntitiesWithComponent<Component::UIComponent>();

    for (uint32_t entityId : uiEntities) {
        auto* ui = componentManager.GetComponent<Component::UIComponent>(entityId);
        if (ui && ui->interactive) {
            ui->state = Component::UIState::NORMAL;
        }
    }
}

void UIInputSystem::HandleInputEvent(const std::shared_ptr<engine::event::Event>& event) {
    switch (event->GetType()) {
        case engine::event::EventType::MOUSE_MOVE: {
            auto mouseData = std::static_pointer_cast<engine::event::MouseEventData>(event->GetData());
            if (mouseData) {
                OnMouseMove(mouseData->x, mouseData->y);
            }
            break;
        }
        case engine::event::EventType::MOUSE_CLICK: {
            auto mouseData = std::static_pointer_cast<engine::event::MouseButtonEventData>(event->GetData());
            if (mouseData) {
                OnMouseButtonDown(mouseData->x, mouseData->y, mouseData->button);
                // Note: This simplified handling treats all mouse clicks as button down events
                // For more sophisticated UI interactions, you might need separate events
            }
            break;
        }
        case engine::event::EventType::KEY_DOWN: {
            auto keyData = std::static_pointer_cast<engine::event::KeyEventData>(event->GetData());
            if (keyData) {
                OnKeyDown(keyData->keycode);
            }
            break;
        }
        case engine::event::EventType::KEY_UP: {
            auto keyData = std::static_pointer_cast<engine::event::KeyEventData>(event->GetData());
            if (keyData) {
                OnKeyUp(keyData->keycode);
            }
            break;
        }
        default:
            break;
    }
}

void UIInputSystem::OnMouseButtonDown(int x, int y, uint8_t button) {
    if (button == SDL_BUTTON_LEFT) {
        mouseX_ = x;
        mouseY_ = y;
        mouseButtonDown_ = true;
        mouseButtonPressed_ = true;
    }
}

void UIInputSystem::OnMouseButtonUp(int x, int y, uint8_t button) {
    if (button == SDL_BUTTON_LEFT) {
        mouseX_ = x;
        mouseY_ = y;
        mouseButtonDown_ = false;
        mouseButtonReleased_ = true;
    }
}

void UIInputSystem::OnMouseMove(int x, int y) {
    mouseX_ = x;
    mouseY_ = y;
}

void UIInputSystem::OnKeyDown(SDL_Keycode key) {
    // Handle keyboard input for focused UI element
    // Implementation depends on specific UI element needs
}

void UIInputSystem::OnKeyUp(SDL_Keycode key) {
    // Handle keyboard input for focused UI element
    // Implementation depends on specific UI element needs
}

void UIInputSystem::ExecuteClickCallback(uint32_t elementId) {
    auto* ui = GetUIComponent(elementId);
    if (ui && ui->onClickCallback) {
        ui->onClickCallback();
    }
}

void UIInputSystem::ExecuteHoverCallback(uint32_t elementId) {
    auto* ui = GetUIComponent(elementId);
    if (ui && ui->onHoverCallback) {
        ui->onHoverCallback();
    }
}

bool UIInputSystem::IsValidUIElement(uint32_t elementId) const {
    auto* world = GetWorld();
    if (!world) return false;
    
    return world->HasEntity(elementId);
}

Component::UIComponent* UIInputSystem::GetUIComponent(uint32_t elementId) const {
    auto* world = GetWorld();
    if (!world) return nullptr;
    
    auto& componentManager = world->GetComponentManager();
    return componentManager.GetComponent<Component::UIComponent>(elementId);
}

SDL_Rect UIInputSystem::GetElementScreenBounds(const Component::UIComponent* ui) const {
    if (!ui) return {0, 0, 0, 0};
    
    // For now, return bounds as-is
    // In a more complex system, this would handle anchoring and screen-relative positioning
    return ui->bounds;
}

void UIInputSystem::DebugLogInteraction(const std::string& action, uint32_t elementId) const {
    #ifdef DEBUG
    auto* ui = GetUIComponent(elementId);
    std::string elementName = ui ? ui->elementId : "Unknown";
    std::cout << "[UIInputSystem] " << action << " on element " << elementId 
              << " (" << elementName << ")" << std::endl;
    #endif
}

} // namespace ZombieSurvivor::System