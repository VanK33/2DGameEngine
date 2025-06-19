# ECS Development Plan for 2D Game Engine - 2-Day Sprint Focus

## Executive Summary

This document outlines the **essential** features needed for the ECS (Entity-Component-System) architecture in our 2D game engine, **prioritized for a 2-day sprint** to complete Task 1 from the Priority List. The goal is to build a working ECS foundation that can support 3-digit entity counts for zombie shooting games.

**Focus: Complete ECS Core Implementation in 2 days.**

---

## 🎯 Current State Analysis

### ✅ **What We Have (Good Foundation)**
- Basic ECS architecture with Entity, Component, System, and World classes
- Type-safe component management using templates and `std::type_index`
- Move-only Entity design for performance
- Simple system execution loop
- Basic world state management (pause/resume, scene tracking)

### ❌ **Critical Gaps (Blocking Task 1)**
- No entity lifecycle management (creation/destruction) - **BLOCKING**
- Missing component querying and filtering capabilities - **BLOCKING**
- No system dependency management or execution ordering - **BLOCKING**
- Missing core 2D components - **BLOCKING**
- Limited integration with existing World class - **BLOCKING**

---

## 🚀 Day 1: Core ECS Infrastructure (CRITICAL)

### 1.1 Entity Factory & Lifecycle Management - **MUST HAVE**
**Files to Create:**
- `EntityFactory.hpp/cpp`

**Features (Minimal):**
```cpp
class EntityFactory {
public:
    EntityID CreateEntity(const std::string& name = "");
    void DestroyEntity(EntityID id);
    bool IsValid(EntityID id) const;
    void ClearAll();
    size_t GetActiveEntityCount() const;
    
private:
    std::vector<EntityID> availableIds_;
    std::unordered_set<EntityID> activeIds_;
    EntityID nextId_ = 1;
    std::mutex mutex_; // For thread safety
};
```

**Why Critical:** Foundation for all entity creation/destruction. **Blocking everything else.**

**Implementation Time:** 3-4 hours

### 1.2 Enhanced Component Querying - **MUST HAVE**
**Files to Modify:**
- `ComponentManager.hpp` (extend existing)

**Features (Minimal):**
```cpp
// Essential queries for Task 1
template<typename T>
std::vector<EntityID> GetEntitiesWithComponent();

template<typename T, typename U>
std::vector<EntityID> GetEntitiesWithComponents();

template<typename T, typename U, typename V>
std::vector<EntityID> GetEntitiesWithComponents();

// Simple iteration
template<typename T>
void ForEachComponent(std::function<void(EntityID, T&)> fn);

template<typename T, typename U>
void ForEachComponent(std::function<void(EntityID, T&, U&)> fn);
```

**Why Critical:** Systems need to find entities with specific components. **Essential for all systems.**

**Implementation Time:** 2-3 hours

### 1.3 System Manager - **MUST HAVE**
**Files to Create:**
- `SystemManager.hpp/cpp`

**Features (Minimal):**
```cpp
class SystemManager {
public:
    void AddSystem(SystemPtr system, int priority = 0);
    void Update(float deltaTime);
    void SetSystemOrder(const std::string& name, int priority);
    void RemoveSystem(const std::string& name);
    size_t GetSystemCount() const;
    
private:
    std::vector<std::pair<int, SystemPtr>> systems_;
    std::unordered_map<std::string, SystemPtr> systemMap_;
};
```

**Why Critical:** Need to manage multiple systems and control execution order. **Essential for game logic.**

**Implementation Time:** 2-3 hours

---

## 🎨 Day 2: Core Components & Integration (CRITICAL)

### 2.1 Core 2D Components - **MUST HAVE**
**Files to Create:**
- `components/Transform2D.hpp`
- `components/Sprite2D.hpp`
- `components/Collider2D.hpp`
- `components/Velocity2D.hpp`
- `components/Lifetime.hpp`
- `components/Tag.hpp`

**Features (Minimal):**
```cpp
struct Transform2D {
    float x = 0.0f, y = 0.0f;
    float rotation = 0.0f;
    float scaleX = 1.0f, scaleY = 1.0f;
};

struct Sprite2D {
    std::string texturePath;
    SDL_Rect sourceRect = {0, 0, 0, 0};
    bool visible = true;
    SDL_Color tint = {255, 255, 255, 255};
};

struct Collider2D {
    SDL_FRect bounds = {0, 0, 0, 0};
    bool isTrigger = false;
    std::string layer = "default";
};

struct Velocity2D {
    float vx = 0.0f, vy = 0.0f;
    float maxSpeed = 100.0f;
};

struct Lifetime {
    float remainingTime = 0.0f;
    bool destroyOnExpire = true;
};

struct Tag {
    std::string tag;
    std::vector<std::string> tags;
};
```

**Why Critical:** These are the basic building blocks for 2D games. **Essential for any 2D game.**

**Implementation Time:** 3-4 hours

### 2.2 World Integration - **MUST HAVE**
**Files to Modify:**
- `World.hpp/cpp` (extend existing)

**Features (Minimal):**
```cpp
class World {
public:
    // Existing functionality...
    
    // Essential additions for Task 1
    EntityFactory& GetEntityFactory() { return entityFactory_; }
    ComponentManager& GetComponentManager() { return componentManager_; }
    SystemManager& GetSystemManager() { return systemManager_; }
    
    // Entity management
    void ClearAllEntities();
    size_t GetEntityCount() const;
    bool HasEntity(EntityID id) const;
    
    // Simple scene management
    void Pause() { worldState_.SetPaused(true); }
    void Resume() { worldState_.SetPaused(false); }
    bool IsPaused() const { return worldState_.IsPaused(); }

private:
    EntityFactory entityFactory_;
    ComponentManager componentManager_;
    SystemManager systemManager_;
    WorldState worldState_;
};
```

**Why Critical:** Need a unified interface to access all ECS functionality. **Essential for game development.**

**Implementation Time:** 2-3 hours

### 2.3 Spatial Grid for Performance - **MUST HAVE**
**Files to Create:**
- `SpatialGrid.hpp/cpp`

**Features (Minimal):**
```cpp
class SpatialGrid {
public:
    SpatialGrid(float cellSize = 64.0f);
    
    void Insert(EntityID id, const SDL_FRect& bounds);
    std::vector<EntityID> Query(const SDL_FRect& area);
    void Update(EntityID id, const SDL_FRect& newBounds);
    void Remove(EntityID id);
    void Clear();
    
    // Performance monitoring
    size_t GetGridCellCount() const;
    size_t GetEntityCount() const;
    
private:
    std::unordered_map<std::pair<int, int>, std::vector<EntityID>> grid_;
    std::unordered_map<EntityID, std::pair<int, int>> entityPositions_;
    float cellSize_;
};
```

**Why Critical:** Essential for 3-digit entity counts. **Performance requirement for zombie games.**

**Implementation Time:** 3-4 hours

---

## 🎯 Success Criteria for 2-Day Sprint

### **Minimum Viable ECS:**
- ✅ Can create and destroy entities with EntityFactory
- ✅ Can query entities by component combinations
- ✅ Can manage systems with execution ordering
- ✅ Has all core 2D components (Transform2D, Sprite2D, Collider2D, Velocity2D, Lifetime, Tag)
- ✅ World class provides unified access to all ECS functionality
- ✅ Spatial grid supports 3-digit entity counts efficiently
- ✅ Can create a simple test scene with 100+ entities

### **Performance Targets:**
- Support 300+ entities at 60 FPS
- Component query time < 1ms for typical queries
- Spatial grid query time < 2ms for collision detection
- Memory usage < 50MB for 300 entities

---

## 📊 2-Day Implementation Timeline

### **Day 1: Core Infrastructure (8-10 hours)**
- **Morning (4 hours):** Entity Factory & Lifecycle Management
- **Afternoon (4 hours):** Enhanced Component Querying
- **Evening (2 hours):** System Manager

### **Day 2: Components & Integration (8-10 hours)**
- **Morning (4 hours):** Core 2D Components
- **Afternoon (3 hours):** World Integration
- **Evening (3 hours):** Spatial Grid Implementation

---

## 🚨 Risk Mitigation

### **If Behind Schedule:**
1. **Skip Spatial Grid** - Can implement basic collision detection without it
2. **Simplify components** - Focus on Transform2D, Sprite2D, Collider2D only
3. **Use existing systems** - Leverage current World and ComponentManager
4. **Reduce scope** - Focus on core functionality over optimization

### **If Performance Issues Arise:**
1. **Profile first** - Don't optimize prematurely
2. **Reduce entity count** - Start with 50-100 entities
3. **Simplify spatial grid** - Use larger cell sizes
4. **Optimize later** - Focus on functionality first

---

## 💡 Development Tips

### **Start Simple:**
- Build EntityFactory first - everything depends on it
- Test each component independently
- Create simple test scenes to validate functionality

### **Focus on Core Functionality:**
- Don't over-engineer - keep implementations simple
- Use existing patterns from current codebase
- Prioritize working code over perfect architecture

### **Test Early:**
- Create test entities immediately after EntityFactory
- Validate component queries work correctly
- Test system execution order

---

## 📋 Wait-List (Future Reference)

### **Phase 2: Performance Optimizations**
- Object pooling for entities and components
- Component archetypes for memory optimization
- Advanced spatial partitioning (Quadtree)
- Multi-threading support for systems

### **Phase 3: Advanced Features**
- Component serialization and save/load
- Advanced debugging and profiling tools
- Hot-reloading for components and systems
- Component validation and schema enforcement

### **Phase 4: Integration Features**
- Scripting integration (Lua/Python)
- Network synchronization for components
- Advanced event system integration
- Component-based animation system

### **Phase 5: Developer Tools**
- Entity inspector and debugger
- Visual component editor
- Performance profiling tools
- Memory leak detection

---

## 📚 Essential Resources

- [ECS FAQ](https://github.com/SanderMertens/ecs-faq) - For understanding ECS patterns
- [Game Programming Patterns](https://gameprogrammingpatterns.com/) - For general game architecture
- [SDL3 Documentation](https://wiki.libsdl.org/) - For rendering and input

---

*This plan focuses on completing Task 1 from the Priority List within 2 days. Focus on core functionality that enables game development, not perfect optimization.* 