// src/engine/core/event/events/PhysicsEvents.hpp

#pragma once

#include "engine/core/event/Event.hpp"
#include "engine/core/event/EventType.hpp"
#include "engine/core/Types.hpp"
#include <SDL3/SDL.h>
#include <string>

namespace engine::event {

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

class CollisionEvent : public Event {
public:
    CollisionEvent(EntityID entityA, EntityID entityB) 
        : Event(EventType::COLLISION_STARTED),
          entityA_(entityA), 
          entityB_(entityB) {}

    EntityID GetEntityA() const { return entityA_; }
    EntityID GetEntityB() const { return entityB_; }

private:
    EntityID entityA_;
    EntityID entityB_;
};


} // namespace engine::event 