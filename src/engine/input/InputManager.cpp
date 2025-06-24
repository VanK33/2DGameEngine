// src/input/InputManager.cpp

#include "InputManager.hpp"
#include <chrono>
#include <iostream>

namespace engine::input {

void InputManager::SetEventManager(engine::event::EventManager* eventManager) {
    eventManager_ = eventManager;
    std::cout << "[InputManager] Event manager set" << std::endl;
}

void InputManager::HandleEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
            if (!event.key.repeat) {
                keyDown_[event.key.key] = true;
                keyHeld_[event.key.key] = true;
                PublishKeyEvent(engine::event::EventType::KEY_DOWN, event);
            }
            break;

        case SDL_EVENT_KEY_UP:
            keyHeld_[event.key.key] = false;
            keyUp_[event.key.key] = true;
            PublishKeyEvent(engine::event::EventType::KEY_UP, event);
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            mouseButtonDown_[event.button.button] = true;
            PublishMouseButtonEvent(engine::event::EventType::MOUSE_CLICK, event);
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            mouseButtonUp_[event.button.button] = true;
            // 可以发布 MOUSE_BUTTON_UP 事件（如果EventType中有的话）
            break;

        case SDL_EVENT_MOUSE_MOTION:
            mouseX_ = event.motion.x;
            mouseY_ = event.motion.y;
            PublishMouseMotionEvent(event);
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            PublishMouseWheelEvent(event);
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
}

// 保持向后兼容的查询接口
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

// 新增鼠标查询接口
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

// 事件发布辅助方法
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