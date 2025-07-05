# Event System - 2D Game Engine

## Overview

The Event System provides a robust, thread-safe publish/subscribe mechanism for decoupled communication between engine systems. It supports priority-based event processing, advanced filtering, and comprehensive event management for 2D games.

**Status: COMPLETED ✅

---

## Key Features

### 🎯 **Core Functionality**
- **Thread-Safe Operations**: Concurrent event publishing and processing
- **Priority-Based Processing**: Critical events processed first
- **Type-Safe Events**: Compile-time type checking for event types
- **Advanced Filtering**: Multiple filter types and combinations
- **Event Listener System**: Flexible subscription mechanisms

### 🔄 **Event Processing**
- **Publish/Subscribe Pattern**: Decoupled system communication
- **Event Queuing**: Efficient batch processing of events
- **Exception Safety**: Comprehensive error handling during event dispatch
- **Performance Monitoring**: Event processing statistics

---

## Architecture

### **Core Components**

#### **EventManager** (`EventManager.hpp/cpp`)
- **Central Event Hub**: Manages all event publishing and subscription
- **Thread-Safe Design**: Safe for concurrent access from multiple threads
- **Priority Processing**: Events processed in priority order
- **Listener Management**: Automatic listener registration and cleanup

#### **Event** (`Event.hpp`)
- **Base Event Class**: Foundation for all event types
- **Type Safety**: Compile-time type checking
- **Priority Support**: EventPriority enum for processing order
- **Timestamp Tracking**: Event creation and processing timing

#### **EventListener** (`EventListener.hpp`)
- **Abstract Base Class**: Interface for event consumers
- **Virtual Dispatch**: Polymorphic event handling
- **Flexible Integration**: Easy integration with any system

#### **Event Filtering** (`EventFilter.hpp/cpp`)
- **TypeFilter**: Filter events by type
- **PriorityFilter**: Filter events by priority level
- **Composite Filters**: AndFilter, OrFilter, NotFilter for complex logic
- **ConditionalEventListener**: Base class for filtered event processing

---

## Event Types

### **Core 2D Game Events**
```cpp
enum class EventType {
    // Input Events
    KEY_DOWN, KEY_UP, KEY_HELD,
    MOUSE_CLICK, MOUSE_MOVE, MOUSE_DRAGGED, MOUSE_WHEEL,
    
    // Physics Events
    ENTITY_COLLISION, COLLISION_STARTED, COLLISION_ENDED,
    TRIGGER_ENTERED, TRIGGER_EXITED, ENTITY_MOVED, ENTITY_STOPPED,
    
    // Rendering Events
    SPRITE_CREATED, SPRITE_DESTROYED, SPRITE_VISIBILITY_CHANGED,
    CAMERA_MOVED, TEXTURE_LOADED,
    
    // Scene Events
    SCENE_CHANGE, GAME_OVER,
    
    // System Events
    CUSTOM
};
```

### **Event Priority Levels**
```cpp
enum class EventPriority {
    CRITICAL = 0,    // System events, must be processed first
    HIGH = 1,        // Game logic events
    NORMAL = 2,      // Default priority
    LOW = 3          // UI updates, visual effects
};
```

---

## Usage Examples

### **Basic Event Publishing**
```cpp
// Create event manager
engine::event::EventManager eventManager;

// Publish a key down event
auto keyEvent = std::make_shared<engine::event::InputKeyEvent>(
    engine::event::EventType::KEY_DOWN, SDL_SCANCODE_SPACE);
eventManager.publish(keyEvent);

// Process events
eventManager.update(); // Call this in your main loop
```

### **Event Subscription**
```cpp
class GameSystem : public engine::event::EventListener {
public:
    void onEvent(const std::shared_ptr<engine::event::Event>& event) override {
        switch (event->getType()) {
            case engine::event::EventType::KEY_DOWN:
                handleKeyDown(event);
                break;
            case engine::event::EventType::ENTITY_COLLISION:
                handleCollision(event);
                break;
        }
    }
    
private:
    void handleKeyDown(const std::shared_ptr<engine::event::Event>& event) {
        // Handle key press
    }
    
    void handleCollision(const std::shared_ptr<engine::event::Event>& event) {
        // Handle collision
    }
};

// Register listener
GameSystem gameSystem;
eventManager.subscribe(engine::event::EventType::KEY_DOWN, &gameSystem);
eventManager.subscribe(engine::event::EventType::ENTITY_COLLISION, &gameSystem);
```

### **Event Filtering**
```cpp
// Create type filter for input events only
auto inputFilter = std::make_unique<engine::event::TypeFilter>(
    std::vector<engine::event::EventType>{
        engine::event::EventType::KEY_DOWN,
        engine::event::EventType::MOUSE_CLICK
    });

// Create conditional listener with filter
class InputHandler : public engine::event::ConditionalEventListener {
protected:
    void ProcessEvent(const std::shared_ptr<engine::event::Event>& event) override {
        // Only input events reach here
        handleInputEvent(event);
    }
};

InputHandler inputHandler;
inputHandler.SetFilter(std::move(inputFilter));
eventManager.subscribe(engine::event::EventType::KEY_DOWN, &inputHandler);
```

### **Priority-Based Processing**
```cpp
// High priority system event
auto systemEvent = std::make_shared<engine::event::Event>(
    engine::event::EventType::SCENE_CHANGE);
systemEvent->setPriority(engine::event::EventPriority::CRITICAL);
eventManager.publish(systemEvent);

// Low priority UI event
auto uiEvent = std::make_shared<engine::event::Event>(
    engine::event::EventType::SPRITE_VISIBILITY_CHANGED);
uiEvent->setPriority(engine::event::EventPriority::LOW);
eventManager.publish(uiEvent);

// Events processed in priority order: CRITICAL first, then LOW
```

---

## Integration with Engine Systems

### **Input System Integration**
```cpp
// InputManager automatically publishes events
inputManager.SetEventManager(&eventManager);

// Input events are automatically published when input occurs
// Systems can subscribe to input events directly
```

### **Collision System Integration**
```cpp
// CollisionSystem publishes collision events
collisionSystem.SetEventManager(&eventManager);

// Collision events are automatically published when collisions occur
// Physics and game systems can respond to collisions
```

### **Scene System Integration**
```cpp
// SceneManager publishes scene change events
sceneManager.SetEventManager(&eventManager);

// Scene transitions trigger events for proper cleanup and initialization
```

---

## Performance Characteristics

- **Thread-Safe**: Safe for concurrent access from multiple threads
- **Efficient Processing**: Priority-based event queue processing
- **Memory Efficient**: Shared pointer management prevents memory leaks
- **Scalable**: Handles hundreds of events per frame efficiently
- **Low Latency**: Minimal overhead for event dispatch

---

## Advanced Features

### **Event Filtering Combinations**
```cpp
// Complex filter: (KEY_DOWN OR MOUSE_CLICK) AND HIGH_PRIORITY
auto typeFilter = std::make_unique<engine::event::TypeFilter>(
    std::vector<engine::event::EventType>{
        engine::event::EventType::KEY_DOWN,
        engine::event::EventType::MOUSE_CLICK
    });

auto priorityFilter = std::make_unique<engine::event::PriorityFilter>(
    engine::event::EventPriority::HIGH);

auto combinedFilter = std::make_unique<engine::event::AndFilter>(
    std::move(typeFilter), std::move(priorityFilter));
```

### **Multiple Event Subscription**
```cpp
// Subscribe to multiple event types at once
std::vector<engine::event::EventType> inputEvents = {
    engine::event::EventType::KEY_DOWN,
    engine::event::EventType::KEY_UP,
    engine::event::EventType::MOUSE_CLICK
};

eventManager.subscribeToMultiple(inputEvents, &inputHandler);
```

---

## File Structure

```
src/engine/core/event/
├── EventManager.hpp/cpp        # Core event management
├── Event.hpp                   # Base event class
├── EventListener.hpp           # Event subscription interface
├── EventType.hpp               # Event type definitions
├── EventFilter.hpp/cpp         # Event filtering system
├── ConditionalEventListener.hpp/cpp  # Filtered event processing
├── events/                     # Specific event implementations
│   ├── InputEvents.hpp         # Input-related events
│   ├── PhysicsEvents.hpp       # Physics-related events
│   └── RenderEvents.hpp        # Rendering-related events
└── README.md                   # This documentation
```

---

## Benefits

* ✅ **Decoupled Architecture**: Systems communicate without direct dependencies
* ✅ **Thread-Safe**: Safe for use in multi-threaded environments
* ✅ **High Performance**: Efficient event processing with priority support
* ✅ **Flexible Filtering**: Advanced event filtering for targeted processing
* ✅ **Type Safety**: Compile-time type checking prevents runtime errors
* ✅ **Easy Integration**: Simple subscription and publishing interfaces

---

## Summary

The Event System provides a powerful, flexible foundation for decoupled system communication in 2D games. Its priority-based processing, advanced filtering capabilities, and thread-safe design make it suitable for both simple prototypes and complex game architectures.  
