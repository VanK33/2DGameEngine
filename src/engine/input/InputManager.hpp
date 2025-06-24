// src/engine/input/InputManager.hpp

#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include "engine/core/event/EventManager.hpp"
#include "engine/core/event/events/InputEvents.hpp"

namespace engine::input {

class InputManager {
public:
    InputManager() = default;
    ~InputManager() = default;

    void SetEventManager(engine::event::EventManager* eventManager);

    void HandleEvent(const SDL_Event& event);
    void Update();  // triggered at the beginning of each frame

    bool IsKeyDown(SDL_Keycode key) const;     // check if key is pressed
    bool IsKeyHeld(SDL_Keycode key) const;     // check if key is held
    bool IsKeyUp(SDL_Keycode key) const;       // check if key is released 

    bool IsMouseButtonDown(Uint8 button) const;
    bool IsMouseButtonUp(Uint8 button) const;
    void GetMousePosition(int& x, int& y) const;

private:
    std::unordered_map<SDL_Keycode, bool> keyDown_;
    std::unordered_map<SDL_Keycode, bool> keyHeld_;
    std::unordered_map<SDL_Keycode, bool> keyUp_;

    std::unordered_map<Uint8, bool> mouseButtonDown_;
    std::unordered_map<Uint8, bool> mouseButtonUp_;
    int mouseX_ = 0, mouseY_ = 0;

    engine::event::EventManager* eventManager_ = nullptr;

    void PublishKeyEvent(engine::event::EventType type, const SDL_Event& event);
    void PublishMouseButtonEvent(engine::event::EventType type, const SDL_Event& event);
    void PublishMouseMotionEvent(const SDL_Event& event);
    void PublishMouseWheelEvent(const SDL_Event& event);

    uint64_t GetCurrentTimestamp() const;
};

} // namespace engine::input