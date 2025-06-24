#pragma once

#include "EventType.hpp"
#include <memory>
#include <chrono>

namespace engine::event {


enum class EventPriority {
    CRITICAL = 0,   //  System level events
    HIGH = 1,       //  Game logic events
    MEDIUM = 2,     //  User input events, default priority
    LOW = 3         //  Debug events, UI events
};


class Event {
public:
    Event(EventType type, std::shared_ptr<void> data = nullptr)
        : type(type), data(std::move(data)), timestamp_(currentTimeMillis()) {}

    EventType getType() const { return type; }
    std::shared_ptr<void> getData() const { return data; }
    uint64_t getTimestamp() const { return timestamp_; }

    EventPriority getPriority() const { return priority_; }
    void setPriority(EventPriority priority) { priority_ = priority; }

private:
    EventType type;
    std::shared_ptr<void> data;
    uint64_t timestamp_;
    EventPriority priority_;

    static uint64_t currentTimeMillis() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
        ).count();
    }
};

} // namespace engine::event
