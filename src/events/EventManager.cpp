#include "EventManager.hpp"
#include <algorithm> // for std::find
#include <iostream>

namespace game {
namespace events {

void EventManager::subscribe(EventType type, EventListener* listener) {
    std::lock_guard<std::mutex> lock(listenersMutex_);
    listeners_[type].insert(listener);
    std::cout << "[EventManager] Subscribed listener for type " << (int)type 
              << ", total listeners: " << listeners_[type].size() << std::endl;
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
    std::cout << "[EventManager] Published event type " << (int)event->getType() 
              << ", queue size: " << eventQueue_.size() << std::endl;
}

void EventManager::update() {
    std::queue<std::shared_ptr<Event>> queueCopy;
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        std::swap(queueCopy, eventQueue_);
    }
    
    // std::cout << "[EventManager] Processing " << queueCopy.size() << " events" << std::endl;
    
    while (!queueCopy.empty()) {
        auto event = queueCopy.front();
        queueCopy.pop();
        
        std::unordered_set<EventListener*> listenersCopy;
        {
            std::lock_guard<std::mutex> lock(listenersMutex_);
            auto it = listeners_.find(event->getType());
            if (it != listeners_.end()) {
                listenersCopy = it->second;
                std::cout << "[EventManager] Found " << listenersCopy.size() 
                          << " listeners for event type " << (int)event->getType() << std::endl;
            } else {
                std::cout << "[EventManager] No listeners found for event type " 
                          << (int)event->getType() << std::endl;
            }
        }
        
        // Dispatch to each listener safely
        for (auto* listener : listenersCopy) {
            if (!listener) {
                std::cout << "[EventManager] Warning: null listener!" << std::endl;
                continue;
            }
            try {
                std::cout << "[EventManager] Dispatching to listener..." << std::endl;
                listener->onEvent(event);
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