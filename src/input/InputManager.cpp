// src/input/InputManager.cpp

#include "InputManager.hpp"

namespace input {

void InputManager::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
        keyDown_[event.key.key] = true;
        keyHeld_[event.key.key] = true;
    }
    
    if (event.type == SDL_EVENT_KEY_UP) {
        keyHeld_[event.key.key] = false;
        keyUp_[event.key.key] = true;
    }
}

void InputManager::Update() {
    keyDown_.clear();
    keyUp_.clear();
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

}