#pragma once

#include "EventType.hpp"
#include <SDL3/SDL.h>
#include <cstdint>

namespace engine::event {

struct KeyEventData {
    SDL_Keycode keycode;
    SDL_Keymod mod;
    bool repeat;
    uint64_t timestamp;
};

struct MouseEventData {
    int x, y;
    int deltaX, deltaY;
    Uint32 buttonMask;
    SDL_MouseID mouseId;
    uint64_t timestamp;
};

struct MouseButtonEventData {
    int x, y;
    Uint8 button;
    Uint8 clicks;
    SDL_MouseID mouseId;
    uint64_t timestamp;
};

struct MouseWheelData {
    float x, y;
    SDL_MouseWheelDirection direction;
    uint64_t timestamp;
};

} // namespace engine::event 