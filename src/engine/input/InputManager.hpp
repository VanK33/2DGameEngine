// src/input/InputManager.hpp

#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
namespace engine::input {

class InputManager {
public:
    void HandleEvent(const SDL_Event& event);
    void Update();  // triggered at the beginning of each frame

    bool IsKeyDown(SDL_Keycode key) const;     // check if key is pressed
    bool IsKeyHeld(SDL_Keycode key) const;     // check if key is held
    bool IsKeyUp(SDL_Keycode key) const;       // check if key is released 

private:
    std::unordered_map<SDL_Keycode, bool> keyDown_;
    std::unordered_map<SDL_Keycode, bool> keyHeld_;
    std::unordered_map<SDL_Keycode, bool> keyUp_;
};

} // namespace engine::input