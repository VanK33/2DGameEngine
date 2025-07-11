// src/examples/zombie_survivor/ecs/components/TargetComponent.hpp

#pragma once

#include "engine/core/Types.hpp"

namespace ZombieSurvivor::Component {

using engine::EntityID;

enum class TargetType {
    NONE,
    PLAYER,
    ENEMY,
    ITEM
};

struct TargetComponent {
    EntityID targetEntity = 0;
    TargetType targetType = TargetType::NONE;
    float targetPriority = 1.0f;
    float lastTargetUpdate = 0.0f;
};

} // namesapce ZombieSurvivor::Component