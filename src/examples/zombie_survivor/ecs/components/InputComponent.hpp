// src/examples/zombie_survivor/ecs/components/InputComponent.hpp
#pragma once

#include "engine/core/Types.hpp"

namespace ZombieSurvivor::Component {
struct InputComponent {
    engine::Vector2 moveInput{0.0f, 0.0f};
    engine::Vector2 mousePosition{0.0f, 0.0f};
    bool shootButtonPressed = false;
    bool reloadButtonPressed = false;
    int weaponSwitchPressed = -1;
};
} //namespace ZombieSurvivor::Component
