#pragma once

#include "EventType.hpp"
#include <memory>
#include <chrono>

namespace game {
namespace events {

/**
 * Represents a generic event in the system.
 * Each event has a type, optional payload data, and a timestamp of creation.
 */
class Event {
public:
    /**
     * Constructs a new event with optional payload.
     * @param type The type of the event
     * @param data Optional payload associated with the event
     */
    Event(EventType type, std::shared_ptr<void> data = nullptr)
        : type(type), data(std::move(data)), timestamp(currentTimeMillis()) {}

    /**
     * Get the event type.
     */
    EventType getType() const { return type; }

    /**
     * Get the associated payload data (may be null).
     */
    std::shared_ptr<void> getData() const { return data; }

    /**
     * Get the event creation timestamp in milliseconds since epoch.
     */
    long getTimestamp() const { return timestamp; }

private:
    EventType type;
    std::shared_ptr<void> data;
    long timestamp;

    static long currentTimeMillis() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
        ).count();
    }
};

} // namespace events
} // namespace game
