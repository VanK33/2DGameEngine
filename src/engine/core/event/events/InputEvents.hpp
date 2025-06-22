#pragma once

#include "EventType.hpp"
#include <SDL3/SDL.h>

namespace engine::event {

enum class InputEventType {
    KEY_PRESSED,
    KEY_RELEASED,
    KEY_HELD,
    MOUSE_MOVED,
    MOUSE_CLICKED,
    MOUSE_DRAGGED,
    MOUSE_WHEEL
};

struct KeyEventData {
    SDL_Keycode keycode;
    SDL_Keymod mod;
    bool repeat;
    float timestamp;
};

struct MouseEventData {
    int x, y;
    int deltaX, deltaY;
    Uint32 buttonMask;
    SDL_MouseID mouseId;
};

struct MouseWheelData {
    float x, y;
    SDL_MouseWheelDirection direction;
};

} // namespace engine::event 