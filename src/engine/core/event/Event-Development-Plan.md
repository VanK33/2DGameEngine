# Event System Development Plan for 2D Game Engine - Single-Person Project Focus

## Executive Summary

This document outlines the **essential** features needed for the Event System in our 2D game engine, **prioritized for a single-person project with tight deadlines**. The goal is to build multiple games for homework assignments quickly, not to create a production-ready event system.

**Focus: Minimum viable event system that enables rapid game development.**

---

## 🎯 Current State Analysis

### ✅ **What We Have (Good Foundation)**
- Basic publish/subscribe event system with `EventManager`
- Thread-safe event queuing and processing
- Type-safe event types with `EventType` enum
- Generic event payload support with `std::shared_ptr<void>`
- Event timestamp tracking
- Exception-safe event dispatch
- Basic integration with SceneManager

### ❌ **Critical Gaps (Blocking Game Development)**
- Limited event type definitions (only 7 basic types) - **BLOCKING**
- No event filtering or conditional subscription - **BLOCKING**
- Missing event priority and ordering - **BLOCKING**
- No event history or replay capabilities - **BLOCKING**

### ⚠️ **Nice-to-Have (Can Skip for Now)**
- Advanced debugging and profiling tools
- Event serialization for save/load
- Event batching and optimization
- Event validation or schema enforcement
- Complex event-driven systems

---

## 🚀 Phase 1: Essential Event Infrastructure (Priority: CRITICAL)

### 1.1 Essential 2D Event Types - **MUST HAVE**
**Files to Create:**
- `events/RenderEvents.hpp`
- `events/InputEvents.hpp`
- `events/PhysicsEvents.hpp`

**Features (Minimal):**
```cpp
// RenderEvents.hpp
enum class RenderEventType {
    SPRITE_CREATED,
    SPRITE_DESTROYED,
    SPRITE_VISIBILITY_CHANGED,
    CAMERA_MOVED,
    TEXTURE_LOADED
};

struct SpriteCreatedData {
    EntityID entityId;
    std::string texturePath;
    SDL_FRect bounds;
};

// InputEvents.hpp
enum class InputEventType {
    KEY_PRESSED,
    KEY_RELEASED,
    KEY_HELD,
    MOUSE_MOVED,
    MOUSE_CLICKED,
    MOUSE_DRAGGED
};

struct MouseEventData {
    int x, y;
    Uint32 buttonMask;
};

// PhysicsEvents.hpp
enum class PhysicsEventType {
    COLLISION_STARTED,
    COLLISION_ENDED,
    TRIGGER_ENTERED,
    TRIGGER_EXITED
};

struct CollisionData {
    EntityID entityA, entityB;
    SDL_FRect overlap;
    bool isTrigger;
};
```

**Why Critical:** These are the basic events needed for 2D games. **Essential for game logic.**

**Implementation Time:** 2-3 hours

### 1.2 Basic Event Priority - **MUST HAVE**
**Files to Modify:**
- `Event.hpp` (extend existing)
- `EventManager.hpp/cpp` (extend existing)

**Features (Minimal):**
```cpp
enum class EventPriority {
    CRITICAL = 0,    // System events, must be processed first
    HIGH = 1,        // Game logic events
    NORMAL = 2,      // Default priority
    LOW = 3          // UI updates, visual effects
};

class Event {
    // Existing functionality...
    EventPriority getPriority() const { return priority_; }
    void setPriority(EventPriority priority) { priority_ = priority; }
    
private:
    EventPriority priority_ = EventPriority::NORMAL;
};

class EventManager {
    // Priority-based processing
    void publishWithPriority(std::shared_ptr<Event> event, EventPriority priority);
    void processEventsByPriority();
};
```

**Why Critical:** Critical events (like collision) must be processed before visual updates. **Essential for game responsiveness.**

**Implementation Time:** 1-2 hours

### 1.3 Basic Event Filtering - **MUST HAVE**
**Files to Create:**
- `EventFilter.hpp/cpp` (Simple implementation)

**Features (Minimal):**
```cpp
class EventFilter {
public:
    virtual ~EventFilter() = default;
    virtual bool ShouldProcess(const Event& event) const = 0;
};

class TypeFilter : public EventFilter {
    std::vector<EventType> allowedTypes_;
public:
    TypeFilter(const std::vector<EventType>& types) : allowedTypes_(types) {}
    
    bool ShouldProcess(const Event& event) const override {
        return std::find(allowedTypes_.begin(), allowedTypes_.end(), 
                        event.getType()) != allowedTypes_.end();
    }
};

class ConditionalEventListener : public EventListener {
    std::unique_ptr<EventFilter> filter_;
public:
    void onEvent(const std::shared_ptr<Event>& event) override {
        if (!filter_ || filter_->ShouldProcess(*event)) {
            ProcessEvent(event);
        }
    }
    
protected:
    virtual void ProcessEvent(const std::shared_ptr<Event>& event) = 0;
};
```

**Why Critical:** Systems need to filter events to avoid unnecessary processing. **Essential for performance.**

**Implementation Time:** 1-2 hours

---

## 🎨 Phase 2: Essential Event Integration (Priority: HIGH)

### 2.1 Enhanced EventManager Integration - **MUST HAVE**
**Files to Modify:**
- `EventManager.hpp/cpp` (extend existing)

**Features (Minimal):**
```cpp
class EventManager {
    // Existing functionality...
    
    // Essential additions
    void subscribeWithFilter(EventType type, EventListener* listener, 
                           std::unique_ptr<EventFilter> filter);
    void subscribeToMultiple(const std::vector<EventType>& types, EventListener* listener);
    
    // Event scheduling (simple)
    void scheduleEvent(std::shared_ptr<Event> event, float delaySeconds);
    
    // Event debugging (simple)
    void enableEventLogging(bool enabled);
    std::vector<std::string> getEventLog() const;
    
private:
    struct ScheduledEvent {
        std::shared_ptr<Event> event;
        float remainingTime;
    };
    std::vector<ScheduledEvent> scheduledEvents_;
    bool loggingEnabled_ = false;
    std::vector<std::string> eventLog_;
};
```

**Why Critical:** Need advanced subscription and scheduling for complex game logic. **Essential for game development.**

**Implementation Time:** 2-3 hours

### 2.2 Basic Event History - **ONLY IF NEEDED**
**Files to Create:**
- `EventHistory.hpp/cpp` (Simple implementation)

**Features (Minimal):**
```cpp
class EventHistory {
public:
    // Circular buffer for recent events
    void AddEvent(const Event& event);
    std::vector<std::shared_ptr<Event>> GetRecentEvents(size_t count) const;
    
    // Simple search
    std::vector<std::shared_ptr<Event>> FindEvents(EventType type) const;
    
private:
    static const size_t MAX_HISTORY = 1000;
    std::vector<std::shared_ptr<Event>> eventHistory_;
};
```

**When to Implement:** Only if you need to track recent events for debugging or game logic

**Implementation Time:** 1-2 hours

---

## ⚡ Phase 3: Quick Performance Wins (Priority: MEDIUM - Only if needed)

### 3.1 Simple Event Batching - **ONLY IF PERFORMANCE IS AN ISSUE**
**Files to Create:**
- `EventBatcher.hpp/cpp` (Simple implementation)

**Features (Minimal):**
```cpp
class EventBatcher {
public:
    // Merge similar events
    void AddToBatch(const Event& event);
    std::vector<std::shared_ptr<Event>> GetBatchedEvents();
    
    // Simple deduplication
    void RemoveDuplicateEvents(std::vector<std::shared_ptr<Event>>& events);
    
private:
    std::unordered_map<EventType, std::vector<std::shared_ptr<Event>>> batches_;
};
```

**When to Implement:** Only if you have performance issues with high event frequency

**Implementation Time:** 2-3 hours

### 3.2 Basic Event Profiling - **ONLY IF DEBUGGING IS HARD**
**Files to Create:**
- `EventProfiler.hpp/cpp` (Simple implementation)

**Features (Minimal):**
```cpp
class EventProfiler {
public:
    // Basic metrics
    void StartEventProcessing();
    void EndEventProcessing();
    
    // Simple statistics
    struct EventStats {
        size_t totalEvents;
        size_t eventsPerSecond;
        std::unordered_map<EventType, size_t> eventTypeCounts;
    };
    
    EventStats GetStats() const;
    void ResetStats();
    
private:
    size_t totalEvents_ = 0;
    float processingTime_ = 0.0f;
    std::unordered_map<EventType, size_t> eventTypeCounts_;
};
```

**When to Implement:** Only if debugging event performance becomes difficult

**Implementation Time:** 1-2 hours

---

## 🔧 Phase 4: Integration (Priority: LOW - Only if needed)

### 4.1 Event System Configuration - **ONLY IF NEEDED**
**Files to Create:**
- `EventConfig.hpp/cpp` (Simple implementation)

**Features (Minimal):**
```cpp
struct EventConfig {
    // Performance settings
    size_t maxQueueSize = 1000;
    float maxProcessingTimePerFrame = 16.67f; // 60 FPS
    
    // Debug settings
    bool enableLogging = false;
    bool enableProfiling = false;
    
    // Event filtering
    std::vector<EventType> disabledEventTypes;
};

class EventConfigLoader {
public:
    EventConfig LoadFromFile(const std::string& filename);
    EventConfig GetDefaultConfig() const;
};
```

**When to Implement:** Only if you need runtime configuration

**Implementation Time:** 1-2 hours

### 4.2 Basic Event Testing - **ONLY IF TESTING IS HARD**
**Files to Create:**
- `EventTestFramework.hpp/cpp` (Simple implementation)

**Features (Minimal):**
```cpp
class EventTestFramework {
public:
    // Test utilities
    void SimulateEvent(EventType type, std::shared_ptr<void> data = nullptr);
    bool HasEventOccurred(EventType type) const;
    void ClearEventHistory();
    
private:
    std::vector<EventType> occurredEvents_;
};
```

**When to Implement:** Only if testing event-driven logic becomes difficult

**Implementation Time:** 1 hour

---

## ❌ Phase 5: Advanced Features (Priority: SKIP FOR NOW)

### **Skip These for Now:**
- Event serialization (save/load)
- Event replay capabilities
- Advanced event validation
- Event compression
- Network event synchronization
- Complex event-driven systems
- Advanced profiling tools

**Why Skip:** These are nice-to-have features that don't block basic game development.

---

## 📊 Revised Implementation Timeline (Single-Person Focus)

### **Week 1: Essential Features (CRITICAL)**
- **Day 1:** Essential 2D Event Types
- **Day 2:** Basic Event Priority
- **Day 3:** Basic Event Filtering
- **Day 4:** Enhanced EventManager Integration
- **Day 5:** Test with simple game scenarios

### **Week 2: Integration & Testing**
- **Day 1-2:** Create test games to validate event system
- **Day 3-4:** Fix bugs and optimize based on actual usage
- **Day 5:** Add basic event history if needed

### **Week 3+: Optional Performance Features**
- **Only if needed:** Event Batching, Event Profiling
- **Only if needed:** Event Configuration, Event Testing

---

## 🎯 Success Criteria (Simplified)

### **Minimum Viable Event System:**
- ✅ Can publish and subscribe to events
- ✅ Can handle priority-based event processing
- ✅ Can filter events by type
- ✅ Can schedule delayed events
- ✅ Can integrate with ECS and other engine systems
- ✅ Can support basic 2D game events (input, collision, rendering)

### **Performance Targets (Relaxed):**
- Process 1,000+ events per second (not 10,000)
- Event dispatch time < 5ms (not 1ms)
- Memory usage < 10MB for 1,000 queued events (not 50MB)

### **Usability Targets:**
- Simple API for common event operations
- Basic documentation
- Working example games
- No memory leaks

---

## 🚨 Risk Mitigation

### **If Behind Schedule:**
1. **Skip Phase 3 entirely** - Performance features can be added later
2. **Use simple event types** - Don't over-engineer event data structures
3. **Focus on essential events only** - Input, collision, basic rendering
4. **Use existing event system** - Leverage what's already working

### **If Performance Issues Arise:**
1. **Profile first** - Don't optimize prematurely
2. **Simple solutions first** - Event batching before complex optimizations
3. **Reduce event frequency** - Design games with fewer events

---

## 💡 Development Tips

### **Start Simple:**
- Use events for basic game logic (input, collision, state changes)
- Keep event data structures simple
- Test event flow with simple game scenarios

### **Test Early:**
- Create test scenes immediately after each feature
- Validate that events flow correctly through the system
- Fix event ordering issues before adding complexity

### **Document as You Go:**
- Write simple usage examples
- Document event types and their purposes
- Keep a list of known event flow patterns

---

## 📚 Essential Resources

- [Observer Pattern](https://en.wikipedia.org/wiki/Observer_pattern) - For understanding event systems
- [Event-Driven Architecture](https://martinfowler.com/articles/201701-event-driven.html) - For event system design
- [C++ Event Systems](https://gameprogrammingpatterns.com/event-queue.html) - For implementation patterns

---

*This plan prioritizes getting a working event system quickly over building a perfect event architecture. Focus on what you need to build games, not what would be nice to have.* 