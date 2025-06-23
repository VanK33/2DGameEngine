// src/engine/core/event/events/PhysicsEvents.hpp

#pragma once

#include "engine/core/event/EventType.hpp"
#include <SDL3/SDL.h>
#include <string>

namespace engine::event {

enum class PhysicsEventType {
    COLLISION_STARTED,
    COLLISION_ENDED,
    TRIGGER_ENTERED,
    TRIGGER_EXITED,
    ENTITY_MOVED,
    ENTITY_STOPPED
};

struct CollisionData {
    uint32_t entityA, entityB;
    SDL_FRect overlap;
    bool isTrigger;
    std::string layerA, layerB;
    float impactForce;
};

struct TriggerData {
    uint32_t triggerEntity, otherEntity;
    SDL_FRect triggerBounds;
    std::string triggerType;
};

struct MovementData {
    uint32_t entityId;
    float oldX, oldY;
    float newX, newY;
    float velocityX, velocityY;
    bool isMoving;
};

} // namespace engine::event 