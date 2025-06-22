# Scene Development Plan for 2D Game Engine - Priority List Alignment

## Executive Summary

This document outlines the **essential enhancements** needed for the Scene system to support the 2D top-down zombie shooting game requirements from the Priority List. The current scene system provides a solid foundation but needs integration with ECS, performance optimizations, and enhanced API design.

**Focus: Enhance scene system to work seamlessly with ECS and support 3-digit entity counts efficiently.**

---

## 🎯 Current State Analysis

### ✅ **What We Have (Good Foundation)**
- Basic scene management with `Scene` base class and `SceneManager`
- Scene factory system for dynamic scene creation
- Event-driven scene transitions
- Integration with EventManager and InputManager
- Proper scene lifecycle (Load/Unload/Update/Render)

### ❌ **Critical Gaps (Blocking Priority List Tasks)**
- No ECS integration - scenes can't access World/EntityFactory - **BLOCKING**
- Missing scene-specific entity management - **BLOCKING**
- No scene state persistence between transitions - **BLOCKING**
- Limited scene configuration and setup - **BLOCKING**
- No scene performance monitoring - **BLOCKING**

### ⚠️ **Nice-to-Have (Can Skip for Now)**
- Advanced scene transitions and effects
- Scene serialization and save/load
- Multi-scene rendering (overlays, HUD)
- Scene-specific resource management
- Advanced scene debugging tools

---

## 🚀 Phase 1: ECS Integration (CRITICAL)

### 1.1 Scene-ECS Integration - **MUST HAVE**
**Files to Modify:**
- `Scene.hpp/cpp` (extend existing)
- `SceneManager.hpp/cpp` (extend existing)

**Features (Minimal):**
```cpp
class Scene {
public:
    // Existing functionality...
    
    // ECS Integration
    virtual void SetWorld(ecs::World* world) { world_ = world; }
    ecs::World* GetWorld() const { return world_; }
    
    // Scene-specific entity management
    virtual ecs::EntityID CreateSceneEntity(const std::string& name = "");
    virtual void DestroySceneEntity(ecs::EntityID id);
    virtual void ClearSceneEntities();
    
    // Scene state management
    virtual void PauseScene() { isPaused_ = true; }
    virtual void ResumeScene() { isPaused_ = false; }
    virtual bool IsPaused() const { return isPaused_; }
    
protected:
    ecs::World* world_ = nullptr;
    std::vector<ecs::EntityID> sceneEntities_;
    bool isPaused_ = false;
};
```

**Why Critical:** Scenes need to create and manage entities. **Essential for Task 1 and Task 2.**

**Status:** 📋 **PLANNED** - Next implementation priority

### 1.2 Enhanced SceneManager - **MUST HAVE**
**Files to Modify:**
- `SceneManager.hpp/cpp` (extend existing)

**Features (Minimal):**
```cpp
class SceneManager {
public:
    // Existing functionality...
    
    // ECS Integration
    void SetWorld(ecs::World* world) { world_ = world; }
    ecs::World* GetWorld() const { return world_; }
    
    // Scene configuration
    void SetDefaultScene(const std::string& sceneId);
    std::string GetDefaultScene() const { return defaultSceneId_; }
    
    // Scene state management
    void PauseCurrentScene();
    void ResumeCurrentScene();
    bool IsCurrentScenePaused() const;
    
    // Performance monitoring
    size_t GetCurrentSceneEntityCount() const;
    float GetCurrentSceneUpdateTime() const;
    
private:
    ecs::World* world_ = nullptr;
    std::string defaultSceneId_;
    float lastUpdateTime_ = 0.0f;
};
```

**Why Critical:** Need unified access to ECS from scene management. **Essential for engine API.**

**Status:** 📋 **PLANNED** - After Scene-ECS integration

---

## 🎨 Phase 2: Scene Configuration & Setup (HIGH)

### 2.1 Scene Configuration System - **MUST HAVE**
**Files to Create:**
- `SceneConfig.hpp`
- `SceneSetup.hpp`

**Features (Minimal):**
```cpp
struct SceneConfig {
    std::string sceneId;
    std::string name;
    bool autoLoad = true;
    bool persistentEntities = false;
    int maxEntities = 1000;
    float updateRate = 60.0f;
};

class SceneSetup {
public:
    static void ConfigureScene(Scene* scene, const SceneConfig& config);
    static void SetupDefaultSystems(Scene* scene);
    static void SetupDefaultComponents(Scene* scene);
    
    // Scene-specific setups
    static void SetupGameplayScene(Scene* scene);
    static void SetupMenuScene(Scene* scene);
    static void SetupLoadingScene(Scene* scene);
};
```

**Why Critical:** Need standardized scene setup for different game states. **Essential for Task 2.**

**Status:** 📋 **PLANNED** - After ECS integration

### 2.2 Scene State Persistence - **MUST HAVE**
**Files to Create:**
- `SceneState.hpp`
- `ScenePersistence.hpp`

**Features (Minimal):**
```cpp
struct SceneState {
    std::string sceneId;
    std::vector<ecs::EntityID> persistentEntities;
    std::unordered_map<std::string, std::string> sceneData;
    bool isPaused = false;
};

class ScenePersistence {
public:
    static void SaveSceneState(const Scene* scene, SceneState& state);
    static void LoadSceneState(Scene* scene, const SceneState& state);
    static void ClearSceneState(const std::string& sceneId);
    
    // Entity persistence
    static void MarkEntityPersistent(ecs::EntityID id);
    static void UnmarkEntityPersistent(ecs::EntityID id);
};
```

**Why Critical:** Need to preserve important entities between scenes. **Essential for game flow.**

**Status:** 📋 **PLANNED** - After configuration system

---

## ⚡ Phase 3: Performance & Optimization (MEDIUM)

### 3.1 Scene Performance Monitoring - **SHOULD HAVE**
**Files to Create:**
- `SceneProfiler.hpp/cpp`

**Features (Minimal):**
```cpp
struct ScenePerformance {
    float updateTime = 0.0f;
    float renderTime = 0.0f;
    size_t entityCount = 0;
    size_t systemCount = 0;
    float memoryUsage = 0.0f;
};

class SceneProfiler {
public:
    void BeginUpdate();
    void EndUpdate();
    void BeginRender();
    void EndRender();
    
    ScenePerformance GetPerformance() const;
    void Reset();
    
private:
    ScenePerformance performance_;
    std::chrono::high_resolution_clock::time_point updateStart_;
    std::chrono::high_resolution_clock::time_point renderStart_;
};
```

**Why Important:** Need to monitor scene performance for optimization. **Useful for debugging.**

**Status:** 📋 **PLANNED** - After core features

### 3.2 Scene Entity Pooling - **SHOULD HAVE**
**Files to Create:**
- `SceneEntityPool.hpp/cpp`

**Features (Minimal):**
```cpp
class SceneEntityPool {
public:
    void PreallocateEntities(size_t count);
    ecs::EntityID GetEntity();
    void ReturnEntity(ecs::EntityID id);
    void Clear();
    
    size_t GetAvailableCount() const;
    size_t GetTotalCount() const;
    
private:
    std::queue<ecs::EntityID> availableEntities_;
    std::unordered_set<ecs::EntityID> activeEntities_;
    size_t totalAllocated_ = 0;
};
```

**Why Important:** Improve performance for frequent entity creation/destruction. **Optimization for Task 1.**

**Status:** 📋 **PLANNED** - Performance optimization phase

---

## 🎯 Success Criteria

### **Minimum Viable Scene System:**
- ✅ Can create and manage scenes
- ✅ Can handle scene transitions
- ✅ Can integrate with EventManager
- 📋 Can integrate with ECS (World/EntityFactory)
- 📋 Can manage scene-specific entities
- 📋 Can persist scene state between transitions

### **Enhanced Scene System:**
- 📋 Can configure scenes with different setups
- 📋 Can monitor scene performance
- 📋 Can optimize entity management with pooling
- 📋 Can handle complex scene hierarchies

---

## 🚀 Implementation Priority

### **Phase 1: ECS Integration (CRITICAL)**
1. **Scene-ECS Integration** - Add World access to Scene class
2. **Enhanced SceneManager** - Add ECS integration to SceneManager
3. **Testing** - Validate ECS integration works correctly

### **Phase 2: Configuration & Setup (HIGH)**
1. **Scene Configuration System** - Standardized scene setup
2. **Scene State Persistence** - Entity and data persistence
3. **Integration Testing** - Test with different scene types

### **Phase 3: Performance & Optimization (MEDIUM)**
1. **Scene Performance Monitoring** - Add profiling capabilities
2. **Scene Entity Pooling** - Optimize entity management
3. **Performance Testing** - Validate optimizations

---

## 📝 Notes

- **Focus on ECS integration first** - This is blocking other Priority List tasks
- **Keep it simple** - Avoid over-engineering for homework scope
- **Test incrementally** - Validate each phase before moving to next
- **Performance matters** - Need to support 3-digit entity counts efficiently
- **Integration ready** - Design for seamless ECS and Event system integration 