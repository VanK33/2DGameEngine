#pragma once

#include "engine/core/Types.hpp"

namespace ZombieSurvivor::Component {

struct FollowComponent {
    engine::EntityID targetEntityId;
    engine::Vector2 offset;
    bool followRotation = false;
    float followDistance = 50.0f;
    
    FollowComponent() = default;
    
    FollowComponent(engine::EntityID target, engine::Vector2 followOffset, bool followRot = false, float distance = 50.0f)
        : targetEntityId(target), offset(followOffset), followRotation(followRot), followDistance(distance) {}
};

} // namespace ZombieSurvivor::Component