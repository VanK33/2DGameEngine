#include "EventManager.hpp"
#include <algorithm> // for std::find
#include <iostream>

namespace game {
namespace events {

void EventManager::subscribe(EventType type, EventListener* listener) {
    std::lock_guard<std::mutex> lock(listenersMutex_);
    listeners_[type].insert(listener);
}

void EventManager::unsubscribe(EventType type, EventListener* listener) {
    std::lock_guard<std::mutex> lock(listenersMutex_);
    auto it = listeners_.find(type);
    if (it != listeners_.end()) {
        it->second.erase(listener);
        if (it->second.empty()) {
            listeners_.erase(it);
        }
    }
}

void EventManager::publish(std::shared_ptr<Event> event) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    eventQueue_.push(event);
}

void EventManager::update() {
    std::queue<std::shared_ptr<Event>> queueCopy;

    {
        // Copy the event queue under lock, then release quickly
        std::lock_guard<std::mutex> lock(queueMutex_);
        std::swap(queueCopy, eventQueue_);
    }

    // Now process the copied queue safely
    while (!queueCopy.empty()) {
        auto event = queueCopy.front();
        queueCopy.pop();

        std::unordered_set<EventListener*> listenersCopy;

        {
            std::lock_guard<std::mutex> lock(listenersMutex_);
            auto it = listeners_.find(event->getType());
            if (it != listeners_.end()) {
                listenersCopy = it->second;
            }
        }

        // Dispatch to each listener safely
        for (auto* listener : listenersCopy) {
            if (!listener) continue;

            try {
                listener->onEvent(event);  // shared_ptr<Event>
            } catch (const std::exception& e) {
                std::cerr << "[EventManager] Listener threw exception: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "[EventManager] Listener threw unknown exception." << std::endl;
            }
        }
    }
}

void EventManager::clear() {
    {
        std::lock_guard<std::mutex> lock1(queueMutex_);
        while (!eventQueue_.empty()) {
            eventQueue_.pop();
        }
    }

    {
        std::lock_guard<std::mutex> lock2(listenersMutex_);
        listeners_.clear();
    }
}

size_t EventManager::getListenerCount(EventType type) const {
    std::lock_guard<std::mutex> lock(listenersMutex_);
    auto it = listeners_.find(type);
    return (it != listeners_.end()) ? it->second.size() : 0;
}

size_t EventManager::getQueueSize() const {
    std::lock_guard<std::mutex> lock(queueMutex_);
    return eventQueue_.size();
}

} // namespace events
} // namespace game
