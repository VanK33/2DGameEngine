// src/engine/input/InputManager.hpp

#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <span>
#include <array>
#include <glm/vec2.hpp>
#include <cassert>
#include "engine/core/event/EventManager.hpp"
#include "engine/core/event/events/InputEvents.hpp"

namespace engine::input {

// Convenient data structures
struct MousePosition {
    int x, y;
    operator glm::vec2() const { return glm::vec2(static_cast<float>(x), static_cast<float>(y)); }
};

struct MouseDelta {
    int dx, dy;
    operator glm::vec2() const { return glm::vec2(static_cast<float>(dx), static_cast<float>(dy)); }
};

// Common key combinations
namespace KeyCombos {
    inline static const std::array<SDL_Keycode, 2> LEFT_KEYS = {SDLK_A, SDLK_LEFT};
    inline static const std::array<SDL_Keycode, 2> RIGHT_KEYS = {SDLK_D, SDLK_RIGHT};
    inline static const std::array<SDL_Keycode, 2> UP_KEYS = {SDLK_W, SDLK_UP};
    inline static const std::array<SDL_Keycode, 2> DOWN_KEYS = {SDLK_S, SDLK_DOWN};
}

class InputManager {
public:
    InputManager() = default;
    ~InputManager() = default;

    void SetEventManager(engine::event::EventManager* eventManager);

    void HandleEvent(const SDL_Event& event);
    void Update();  // triggered at the beginning of each frame

    // Original single key query interface
    bool IsKeyDown(SDL_Keycode key) const;     
    bool IsKeyHeld(SDL_Keycode key) const;     
    bool IsKeyUp(SDL_Keycode key) const;       

    // Combined key query interface
    bool IsAnyKeyDown(std::span<const SDL_Keycode> keys) const;
    bool IsAnyKeyHeld(std::span<const SDL_Keycode> keys) const;
    bool IsAnyKeyUp(std::span<const SDL_Keycode> keys) const;
    bool IsAllKeysDown(std::span<const SDL_Keycode> keys) const;
    bool IsAllKeysHeld(std::span<const SDL_Keycode> keys) const;

    // Mouse button interface
    bool IsMouseButtonDown(Uint8 button) const;
    bool IsMouseButtonHeld(Uint8 button) const;  // New addition
    bool IsMouseButtonUp(Uint8 button) const;
    
    bool IsAnyMouseButtonDown(std::span<const Uint8> buttons) const;
    bool IsAnyMouseButtonHeld(std::span<const Uint8> buttons) const;
    bool IsAnyMouseButtonUp(std::span<const Uint8> buttons) const;

    // Mouse position interface
    MousePosition GetMousePosition() const;
    void GetMousePosition(int& x, int& y) const;  // Maintain backward compatibility
    MouseDelta GetMouseDelta() const;
    void GetMouseDelta(int& dx, int& dy) const;
    glm::vec2 GetNormalizedMouseDelta() const;

#ifdef DEBUG
    void DebugPrintActiveInputs() const;
#endif

private:
    std::unordered_map<SDL_Keycode, bool> keyDown_;
    std::unordered_map<SDL_Keycode, bool> keyHeld_;
    std::unordered_map<SDL_Keycode, bool> keyUp_;

    std::unordered_map<Uint8, bool> mouseButtonDown_;
    std::unordered_map<Uint8, bool> mouseButtonHeld_;  // New addition
    std::unordered_map<Uint8, bool> mouseButtonUp_;
    
    int mouseX_ = 0, mouseY_ = 0;
    int mouseDeltaX_ = 0, mouseDeltaY_ = 0;  // New addition

    engine::event::EventManager* eventManager_ = nullptr;

    void PublishKeyEvent(engine::event::EventType type, const SDL_Event& event);
    void PublishMouseButtonEvent(engine::event::EventType type, const SDL_Event& event);
    void PublishMouseMotionEvent(const SDL_Event& event);
    void PublishMouseWheelEvent(const SDL_Event& event);

    uint64_t GetCurrentTimestamp() const;
};

} // namespace engine::input