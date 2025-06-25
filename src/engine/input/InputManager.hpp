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

// 便捷的数据结构
struct MousePosition {
    int x, y;
    operator glm::vec2() const { return glm::vec2(static_cast<float>(x), static_cast<float>(y)); }
};

struct MouseDelta {
    int dx, dy;
    operator glm::vec2() const { return glm::vec2(static_cast<float>(dx), static_cast<float>(dy)); }
};

// 常用的按键组合
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

    // 原有的单键查询接口
    bool IsKeyDown(SDL_Keycode key) const;     
    bool IsKeyHeld(SDL_Keycode key) const;     
    bool IsKeyUp(SDL_Keycode key) const;       

    // 组合键查询接口
    bool IsAnyKeyDown(std::span<const SDL_Keycode> keys) const;
    bool IsAnyKeyHeld(std::span<const SDL_Keycode> keys) const;
    bool IsAnyKeyUp(std::span<const SDL_Keycode> keys) const;
    bool IsAllKeysDown(std::span<const SDL_Keycode> keys) const;
    bool IsAllKeysHeld(std::span<const SDL_Keycode> keys) const;

    // 鼠标按键接口
    bool IsMouseButtonDown(Uint8 button) const;
    bool IsMouseButtonHeld(Uint8 button) const;  // 新增
    bool IsMouseButtonUp(Uint8 button) const;
    
    bool IsAnyMouseButtonDown(std::span<const Uint8> buttons) const;
    bool IsAnyMouseButtonHeld(std::span<const Uint8> buttons) const;
    bool IsAnyMouseButtonUp(std::span<const Uint8> buttons) const;

    // 鼠标位置接口
    MousePosition GetMousePosition() const;
    void GetMousePosition(int& x, int& y) const;  // 保持向后兼容
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
    std::unordered_map<Uint8, bool> mouseButtonHeld_;  // 新增
    std::unordered_map<Uint8, bool> mouseButtonUp_;
    
    int mouseX_ = 0, mouseY_ = 0;
    int mouseDeltaX_ = 0, mouseDeltaY_ = 0;  // 新增

    engine::event::EventManager* eventManager_ = nullptr;

    void PublishKeyEvent(engine::event::EventType type, const SDL_Event& event);
    void PublishMouseButtonEvent(engine::event::EventType type, const SDL_Event& event);
    void PublishMouseMotionEvent(const SDL_Event& event);
    void PublishMouseWheelEvent(const SDL_Event& event);

    uint64_t GetCurrentTimestamp() const;
};

} // namespace engine::input