#pragma once

#include "Event.hpp"
#include <memory>

namespace engine::event {

/**
 * Interface for any system or object that wants to listen to events.
 * Implementers must define how to handle incoming events.
 */
class EventListener {
public:
    virtual ~EventListener() = default;

    /**
     * Called by EventManager when a subscribed event is dispatched.
     * @param event The event being received
     */
    virtual void onEvent(const std::shared_ptr<Event>& event) = 0;
};

} // namespace engine::event