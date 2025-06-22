// src/engine/core/event/EventManager.hpp

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

    void subscribe(EventType type, EventListener* listener);
    void unsubscribe(EventType type, EventListener* listener);
    void publish(std::shared_ptr<Event> event);
    void update();
    void clear();
    size_t getListenerCount(EventType type) const;
    size_t getQueueSize() const;

    void publishWithPriority(std::shared_ptr<Event> event, EventPriority priority);

    void subscribeWithFilter(EventType type, EventListener* listener, std::unique_ptr<EventFilter> filter);
    void subscribeToMultipleWithFilter(const std::vector<EventType>& types, EventListener* listener, std::unique_ptr<EventFilter> filter);

    void subscribeToMultiple(const std::vector<EventType>& types, EventListener* listener);

private:
    std::unordered_map<EventType, std::unordered_set<EventListener*>> listeners_;
    std::queue<std::shared_ptr<Event>> eventQueue_;

    mutable std::mutex listenersMutex_;
    mutable std::mutex queueMutex_;
    
    void processEventsByPriority();
    void processEvent(const std::shared_ptr<Event>& event); 
    std::vector<std::shared_ptr<Event>> getAndSortEvents();

    std::unordered_map<EventListener*, std::unique_ptr<EventFilter>> filters_;
    mutable std::mutex filtersMutex_;
    
};
} // namespace engine::event
