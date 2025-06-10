#pragma once

#include "Event.hpp"
#include "EventListener.hpp"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <memory>
#include <mutex>

namespace game {
namespace events {

class EventManager {
public:
    EventManager() = default;
    ~EventManager() = default;

    // Delete copy/move to avoid accidental copies
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    /**
     * Subscribe a listener to a specific event type.
     */
    void subscribe(EventType type, EventListener* listener);

    /**
     * Unsubscribe a listener from a specific event type.
     */
    void unsubscribe(EventType type, EventListener* listener);

    /**
     * Publish an event (deferred execution, queued).
     */
    void publish(std::shared_ptr<Event> event);

    /**
     * Process all queued events.
     * Should be called once per frame from the main loop.
     */
    void update();

    /**
     * Clear all listeners and queued events.
     */
    void clear();

    /**
     * Get listener count for a given type.
     */
    size_t getListenerCount(EventType type) const;

    /**
     * Get number of events waiting in the queue.
     */
    size_t getQueueSize() const;

private:
    std::unordered_map<EventType, std::unordered_set<EventListener*>> listeners_;
    std::queue<std::shared_ptr<Event>> eventQueue_;

    mutable std::mutex listenersMutex_;
    mutable std::mutex queueMutex_;
};

} // namespace events
} // namespace game
