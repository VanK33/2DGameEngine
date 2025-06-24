// src/engine/core/event/EventManager.hpp

#pragma once

#include "Event.hpp"
#include "EventListener.hpp"
#include "EventFilter.hpp"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <memory>
#include <mutex>

namespace engine::event {

class EventManager {
public:
    EventManager() = default;
    ~EventManager() = default;

    // Delete copy/move to avoid accidental copies
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    void Subscribe(EventType type, EventListener* listener);
    void Unsubscribe(EventType type, EventListener* listener);
    void Publish(std::shared_ptr<Event> event);
    void Update();
    void Clear();
    size_t GetListenerCount(EventType type) const;
    size_t GetQueueSize() const;

    void PublishWithPriority(std::shared_ptr<Event> event, EventPriority priority);

    void SubscribeWithFilter(EventType type, EventListener* listener, std::unique_ptr<EventFilter> filter);
    void SubscribeToMultipleWithFilter(const std::vector<EventType>& types, EventListener* listener, std::unique_ptr<EventFilter> filter);

    void SubscribeToMultiple(const std::vector<EventType>& types, EventListener* listener);

private:
    std::unordered_map<EventType, std::unordered_set<EventListener*>> listeners_;
    std::queue<std::shared_ptr<Event>> eventQueue_;

    mutable std::mutex listenersMutex_;
    mutable std::mutex queueMutex_;
    
    void ProcessEventsByPriority();
    void ProcessEvent(const std::shared_ptr<Event>& event); 
    std::vector<std::shared_ptr<Event>> GetAndSortEvents();

    std::unordered_map<EventListener*, std::unique_ptr<EventFilter>> filters_;
    mutable std::mutex filtersMutex_;
    
};
} // namespace engine::event
