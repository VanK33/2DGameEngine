// src/input/InputManager.cpp

#include "InputManager.hpp"
#include <chrono>
#include <iostream>
#include <cstdlib>  // for abs
#include <algorithm> // for std::all_of, std::any_of
#include <cmath>

namespace engine::input {

void InputManager::SetEventManager(engine::event::EventManager* eventManager) {
    eventManager_ = eventManager;
    std::cout << "[InputManager] Event manager set" << std::endl;
}

void InputManager::HandleEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
            keyDown_[event.key.key] = true;
            keyHeld_[event.key.key] = true;
            PublishKeyEvent(engine::event::EventType::KEY_DOWN, event);
            break;
        case SDL_EVENT_KEY_UP:
            keyHeld_[event.key.key] = false;
            keyUp_[event.key.key] = true;
            PublishKeyEvent(engine::event::EventType::KEY_UP, event);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            mouseButtonDown_[event.button.button] = true;
            mouseButtonHeld_[event.button.button] = true;
            PublishMouseButtonEvent(engine::event::EventType::MOUSE_CLICK, event);
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            mouseButtonHeld_[event.button.button] = false;
            mouseButtonUp_[event.button.button] = true;
            break;
        case SDL_EVENT_MOUSE_MOTION:
            mouseDeltaX_ = event.motion.xrel;
            mouseDeltaY_ = event.motion.yrel;
            mouseX_ = event.motion.x;
            mouseY_ = event.motion.y;
            PublishMouseMotionEvent(event);
            break;
        case SDL_EVENT_FINGER_DOWN:
            std::cout << "[InputManager] Finger down event detected" << std::endl;
            {
                SDL_Event mouseEvent = {};
                mouseEvent.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
                mouseEvent.button.x = static_cast<int>(event.tfinger.x * 800);
                mouseEvent.button.y = static_cast<int>(event.tfinger.y * 600);
                mouseEvent.button.button = SDL_BUTTON_LEFT;
                PublishMouseButtonEvent(engine::event::EventType::MOUSE_CLICK, mouseEvent);
            }
            break;
        case SDL_EVENT_FINGER_MOTION:
            std::cout << "[InputManager] Finger motion event detected" << std::endl;
            {
                SDL_Event mouseEvent = {};
                mouseEvent.type = SDL_EVENT_MOUSE_MOTION;
                mouseEvent.motion.x = static_cast<int>(event.tfinger.x * 800);
                mouseEvent.motion.y = static_cast<int>(event.tfinger.y * 600);
                mouseEvent.motion.xrel = static_cast<int>(event.tfinger.dx * 800);
                mouseEvent.motion.yrel = static_cast<int>(event.tfinger.dy * 600);
                PublishMouseMotionEvent(mouseEvent);
            }
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            PublishMouseWheelEvent(event);
            std::cout << "[InputManager] Publishing MOUSE_WHEEL event" << std::endl;
            break;
        default:
            break;
    }
}

void InputManager::Update() {
    keyDown_.clear();
    keyUp_.clear();
    mouseButtonDown_.clear();
    mouseButtonUp_.clear();
    // Reset mouse delta at the end of Update
    mouseDeltaX_ = 0;
    mouseDeltaY_ = 0;
}

bool InputManager::IsKeyDown(SDL_Keycode key) const {
    auto it = keyDown_.find(key);
    return it != keyDown_.end() && it->second;
}

bool InputManager::IsKeyHeld(SDL_Keycode key) const {
    auto it = keyHeld_.find(key);
    return it != keyHeld_.end() && it->second;
}

bool InputManager::IsKeyUp(SDL_Keycode key) const {
    auto it = keyUp_.find(key);
    return it != keyUp_.end() && it->second;
}

bool InputManager::IsMouseButtonDown(Uint8 button) const {
    auto it = mouseButtonDown_.find(button);
    return it != mouseButtonDown_.end() && it->second;
}

bool InputManager::IsMouseButtonUp(Uint8 button) const {
    auto it = mouseButtonUp_.find(button);
    return it != mouseButtonUp_.end() && it->second;
}

void InputManager::GetMousePosition(int& x, int& y) const {
    x = mouseX_;
    y = mouseY_;
}

bool InputManager::IsAnyKeyDown(std::span<const SDL_Keycode> keys) const {
    #ifdef DEBUG
        assert(!keys.empty() && "Warning: Checking empty key combination");
    #endif
    if (keys.empty()) return false;
    return std::any_of(keys.begin(), keys.end(), 
        [this](SDL_Keycode key) { return IsKeyDown(key); });
}

bool InputManager::IsAnyKeyHeld(std::span<const SDL_Keycode> keys) const {
    if (keys.empty()) return false;
    return std::any_of(keys.begin(), keys.end(), 
        [this](SDL_Keycode key) { return IsKeyHeld(key); });
}

bool InputManager::IsAnyKeyUp(std::span<const SDL_Keycode> keys) const {
    if (keys.empty()) return false;
    return std::any_of(keys.begin(), keys.end(), 
        [this](SDL_Keycode key) { return IsKeyUp(key); });
}

bool InputManager::IsAllKeysDown(std::span<const SDL_Keycode> keys) const {
    if (keys.empty()) return true;
    return std::all_of(keys.begin(), keys.end(), 
        [this](SDL_Keycode key) { return IsKeyDown(key); });
}

bool InputManager::IsAllKeysHeld(std::span<const SDL_Keycode> keys) const {
    if (keys.empty()) return true;
    return std::all_of(keys.begin(), keys.end(), 
        [this](SDL_Keycode key) { return IsKeyHeld(key); });
}

bool InputManager::IsMouseButtonHeld(Uint8 button) const {
    auto it = mouseButtonHeld_.find(button);
    return it != mouseButtonHeld_.end() && it->second;
}

bool InputManager::IsAnyMouseButtonDown(std::span<const Uint8> buttons) const {
    if (buttons.empty()) return false;
    return std::any_of(buttons.begin(), buttons.end(),
        [this](Uint8 button) { return IsMouseButtonDown(button); });
}

bool InputManager::IsAnyMouseButtonHeld(std::span<const Uint8> buttons) const {
    if (buttons.empty()) return false;
    return std::any_of(buttons.begin(), buttons.end(),
        [this](Uint8 button) { return IsMouseButtonHeld(button); });
}

bool InputManager::IsAnyMouseButtonUp(std::span<const Uint8> buttons) const {
    if (buttons.empty()) return false;
    return std::any_of(buttons.begin(), buttons.end(),
        [this](Uint8 button) { return IsMouseButtonUp(button); });
}

MousePosition InputManager::GetMousePosition() const {
    return MousePosition{mouseX_, mouseY_};
}

MouseDelta InputManager::GetMouseDelta() const {
    return MouseDelta{mouseDeltaX_, mouseDeltaY_};
}

void InputManager::GetMouseDelta(int& dx, int& dy) const {
    dx = mouseDeltaX_;
    dy = mouseDeltaY_;
}

glm::vec2 InputManager::GetNormalizedMouseDelta() const {
    float length = std::sqrt(mouseDeltaX_ * mouseDeltaX_ + mouseDeltaY_ * mouseDeltaY_);
    if (length < 0.0001f) return glm::vec2(0.0f);
    return glm::vec2(mouseDeltaX_ / length, mouseDeltaY_ / length);
}

#ifdef DEBUG
void InputManager::DebugPrintActiveInputs() const {
    std::cout << "=== Active Inputs ===" << std::endl;
    
    std::cout << "Held Keys: ";
    for (const auto& [key, held] : keyHeld_) {
        if (held) std::cout << SDL_GetKeyName(key) << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Held Mouse Buttons: ";
    for (const auto& [button, held] : mouseButtonHeld_) {
        if (held) std::cout << static_cast<int>(button) << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Mouse Position: (" << mouseX_ << ", " << mouseY_ << ")" << std::endl;
    std::cout << "Mouse Delta: (" << mouseDeltaX_ << ", " << mouseDeltaY_ << ")" << std::endl;
    std::cout << "===================" << std::endl;
}
#endif

void InputManager::PublishKeyEvent(engine::event::EventType type, const SDL_Event& event) {
    if (!eventManager_) return;

    auto keyData = std::make_shared<engine::event::KeyEventData>();
    keyData->keycode = event.key.key;
    keyData->mod = event.key.mod;
    keyData->repeat = event.key.repeat;
    keyData->timestamp = GetCurrentTimestamp();

    auto engineEvent = std::make_shared<engine::event::Event>(
        type,
        std::static_pointer_cast<void>(keyData)
    );

    eventManager_->Publish(engineEvent);
}

void InputManager::PublishMouseButtonEvent(engine::event::EventType type, const SDL_Event& event) {
    if (!eventManager_) return;

    auto mouseData = std::make_shared<engine::event::MouseButtonEventData>();
    mouseData->x = event.button.x;
    mouseData->y = event.button.y;
    mouseData->button = event.button.button;
    mouseData->clicks = event.button.clicks;
    mouseData->mouseId = event.button.which;
    mouseData->timestamp = GetCurrentTimestamp();

    auto engineEvent = std::make_shared<engine::event::Event>(
        type,
        std::static_pointer_cast<void>(mouseData)
    );

    eventManager_->Publish(engineEvent);
}

void InputManager::PublishMouseMotionEvent(const SDL_Event& event) {
    if (!eventManager_) return;

    auto mouseData = std::make_shared<engine::event::MouseEventData>();
    mouseData->x = event.motion.x;
    mouseData->y = event.motion.y;
    mouseData->deltaX = event.motion.xrel;
    mouseData->deltaY = event.motion.yrel;
    mouseData->buttonMask = event.motion.state;
    mouseData->mouseId = event.motion.which;
    mouseData->timestamp = GetCurrentTimestamp();

    auto engineEvent = std::make_shared<engine::event::Event>(
        engine::event::EventType::MOUSE_MOVE,
        std::static_pointer_cast<void>(mouseData)
    );

    eventManager_->Publish(engineEvent);
}

void InputManager::PublishMouseWheelEvent(const SDL_Event& event) {
    if (!eventManager_) return;

    auto wheelData = std::make_shared<engine::event::MouseWheelData>();
    wheelData->x = event.wheel.x;
    wheelData->y = event.wheel.y;
    wheelData->direction = event.wheel.direction;
    wheelData->timestamp = GetCurrentTimestamp();

    auto engineEvent = std::make_shared<engine::event::Event>(
        engine::event::EventType::MOUSE_WHEEL,
        std::static_pointer_cast<void>(wheelData)
    );

    eventManager_->Publish(engineEvent);
}

uint64_t InputManager::GetCurrentTimestamp() const {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();
}

} // namespace engine::input