#include "EventManager.hpp"
#include <algorithm> // for std::find
#include <iostream>

namespace engine::event {

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
    processEventsByPriority();
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
    
    {
        std::lock_guard<std::mutex> lock3(filtersMutex_);
        filters_.clear();
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


std::vector<std::shared_ptr<Event>> EventManager::getAndSortEvents() {
    // Safe Queue duplication
    std::queue<std::shared_ptr<Event>> queueCopy;
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        std::swap(queueCopy, eventQueue_);
    }

    // Vectorize
    std::vector<std::shared_ptr<Event>> events;
    while (!queueCopy.empty()) {
        events.push_back(queueCopy.front());
        queueCopy.pop();
    }

    // Sort by Priority
    std::sort(events.begin(), events.end(), 
        [](const std::shared_ptr<Event>& a, const std::shared_ptr<Event>& b) {
            return static_cast<int>(a->getPriority()) < static_cast<int>(b->getPriority());
        });
    
    return events;
}

void EventManager::publishWithPriority(std::shared_ptr<Event> event, EventPriority priority) {
    if (event) {
        event->setPriority(priority);
    }
    publish(event);
}

void EventManager::processEventsByPriority() {
    auto events = getAndSortEvents();
    
    
    for (const auto& event : events) {
        processEvent(event);
    }
}

void EventManager::processEvent(const std::shared_ptr<Event>& event) {
    // Safe listener copy
    std::unordered_set<EventListener*> listenersCopy;
    {
        std::lock_guard<std::mutex> lock(listenersMutex_);
        auto it = listeners_.find(event->getType());
        if (it != listeners_.end()) {
            listenersCopy = it->second;
            std::cout << "[EventManager] Found " << listenersCopy.size() 
                      << " listeners for event type " << (int)event->getType() 
                      << " (priority: " << (int)event->getPriority() << ")" << std::endl;
        } else {
            std::cout << "[EventManager] No listeners found for event type " 
                      << (int)event->getType() << std::endl;
        }
    }
    
    // Safe event dispatch
    for (auto* listener : listenersCopy) {
        if (!listener) {
            std::cout << "[EventManager] Warning: null listener!" << std::endl;
            continue;
        }

        bool shouldProcess = true;
        {
            std::lock_guard<std::mutex> lock(filtersMutex_);
            auto filterIt = filters_.find(listener);
            if (filterIt != filters_.end() && filterIt->second) {
                shouldProcess = filterIt->second->ShouldProcess(event);
                if (!shouldProcess) {
                    std::cout << "[EventManager] Event filtered out for listener" << std::endl;
                }
            }
        }

        if (shouldProcess) {
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

void EventManager::subscribeWithFilter(EventType type, EventListener* listener, 
                                     std::unique_ptr<EventFilter> filter) {
    if (!listener) {
        std::cout << "[EventManager] Warning: Attempting to subscribe null listener!" << std::endl;
        return;
    }
    
    // 订阅事件
    subscribe(type, listener);
    
    // 存储过滤器
    if (filter) {
        std::lock_guard<std::mutex> lock(filtersMutex_);
        filters_[listener] = std::move(filter);
        std::cout << "[EventManager] Added filter for listener" << std::endl;
    }
}

void EventManager::subscribeToMultiple(const std::vector<EventType>& types, EventListener* listener) {
    if (!listener) {
        std::cout << "[EventManager] Warning: Attempting to subscribe null listener!" << std::endl;
        return;
    }

    for (auto type : types) {
        subscribe(type, listener);
    }
}

void EventManager::subscribeToMultipleWithFilter(const std::vector<EventType>& types, EventListener* listener, std::unique_ptr<EventFilter> filter) {
    if (!listener) {
        std::cout << "[EventManager] Warning: Attempting to subscribe null listener!" << std::endl;
        return;
    }

    subscribeToMultiple(types, listener);

    if (filter) {
        std::lock_guard<std::mutex> lock(filtersMutex_);
        filters_[listener] = std::move(filter);
        std::cout << "[EventManager] Added filter for multi-type listener" << std::endl;
    }

}

} // namespace engine::event