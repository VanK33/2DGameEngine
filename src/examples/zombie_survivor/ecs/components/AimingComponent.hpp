// src/examples/zombie_survivor/ecs/compoents/AimingComponent.hpp
#pragma once

#include "engine/core/Types.hpp"

namespace ZombieSurvivor::Component {


struct AimingComponent {
    engine::Vector2 aimDirection{1.0f, 0.0f};
    engine::Vector2 mouseWorldPos{0.0f, 0.0f};

    bool showAimLine = true;
    float maxAimRange = 250.0f;
};

} // namespace ZombieSurvivor::Component