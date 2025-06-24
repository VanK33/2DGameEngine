# Scene Development Plan for 2D Game Engine - Priority List Alignment

## Executive Summary

This document outlines the **essential enhancements** needed for the Scene system to support the 2D top-down zombie shooting game requirements from the Priority List. The current scene system provides a solid foundation and has been successfully integrated with ECS.

**Focus: Enhance scene system to work seamlessly with ECS and support 3-digit entity counts efficiently.**

**Status: Phase 1 COMPLETED ✅, Phase 2 PLANNED**

---

## 🎯 Current State Analysis

### ✅ **What We Have (COMPLETED)**
- Basic scene management with `Scene` base class and `SceneManager`
- Scene factory system for dynamic scene creation
- Event-driven scene transitions
- Integration with EventManager and InputManager
- Proper scene lifecycle (Load/Unload/Update/Render)
- ✅ **ECS Integration** - scenes can access World/EntityFactory
- ✅ **Scene-specific entity management** - CreateSceneEntity, DestroySceneEntity, ClearSceneEntities
- ✅ **Enhanced SceneManager** - World access and management

### ❌ **Critical Gaps (Remaining Work)**
- No scene state persistence between transitions - **PLANNED**
- Limited scene configuration and setup - **PLANNED**
- No scene performance monitoring - **PLANNED**

### ⚠️ **Nice-to-Have (Can Skip for Now)**
- Advanced scene transitions and effects
- Scene serialization and save/load
- Multi-scene rendering (overlays, HUD)
- Scene-specific resource management
- Advanced scene debugging tools

---

## 🚀 Phase 1: ECS Integration - COMPLETED ✅

### ✅ **1.1 Scene-ECS Integration - COMPLETED**
**Files Modified:**
- ✅ `Scene.hpp/cpp` (extended existing)

**Features Implemented:**
```cpp
class Scene {
public:
    // ECS Integration
    virtual void SetWorld(ECS::World* world) { world_ = world; }
    ECS::World* GetWorld() const { return world_; }
    
    // Scene-specific entity management
    virtual ECS::EntityID CreateSceneEntity(const std::string& name = "");
    virtual void DestroySceneEntity(ECS::EntityID id);
    virtual void ClearSceneEntities();
    
protected:
    ECS::World* world_ = nullptr;
    std::vector<ECS::EntityID> sceneEntities_;
};
```

**Status:** ✅ **COMPLETED** - Scene can now create and manage entities through ECS

### ✅ **1.2 Enhanced SceneManager - COMPLETED**
**Files Modified:**
- ✅ `SceneManager.hpp/cpp` (extended existing)

**Features Implemented:**
```cpp
class SceneManager {
public:
    // ECS Integration
    void SetWorld(ECS::World* world) { world_ = world; }
    ECS::World* GetWorld() const { return world_; }
    
private:
    ECS::World* world_ = nullptr;
};
```

**Status:** ✅ **COMPLETED** - SceneManager can manage World and pass it to scenes

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

**Status:** 📋 **PLANNED** - Next implementation priority

### 2.2 Scene State Persistence - **MUST HAVE**
**Files to Create:**
- `SceneState.hpp`
- `ScenePersistence.hpp`

**Features (Minimal):**
```cpp
struct SceneState {
    std::string sceneId;
    std::vector<ECS::EntityID> persistentEntities;
    std::unordered_map<std::string, std::string> sceneData;
    bool isPaused = false;
};

class ScenePersistence {
public:
    static void SaveSceneState(const Scene* scene, SceneState& state);
    static void LoadSceneState(Scene* scene, const SceneState& state);
    static void ClearSceneState(const std::string& sceneId);
    
    // Entity persistence
    static void MarkEntityPersistent(ECS::EntityID id);
    static void UnmarkEntityPersistent(ECS::EntityID id);
};
```

**Why Critical:** Need to preserve important entities between scenes. **Essential for game flow.**

**Status:** 📋 **PLANNED** - After configuration system

### 2.3 Input System Configuration - **LOW** (New Addition)
**Files to Create/Modify:**
- Extend `InputManager.hpp`
- Add to `Scene.hpp`

**Features (Future Implementation):**
```cpp
class InputManager {
public:
    enum class KeyBehavior {
        SINGLE_PRESS,    // 单次按下触发（如跳跃）
        CONTINUOUS,      // 持续触发（如移动）
        TOGGLE          // 切换状态（如切换武器）
    };

    // 将在未来实现的接口
    void SetKeyBehavior(SDL_Keycode key, KeyBehavior behavior);
    void ConfigureKeyBehaviors(const std::unordered_map<SDL_Keycode, KeyBehavior>& behaviors);
};

class Scene {
public:
    // 将在未来实现的虚函数
    virtual void ConfigureInput(InputManager& inputManager) {
        // 默认实现：使用基础按键行为
    }
};
```

**Why Important:** Support different input requirements for various game types (top-down vs platformer).
**Status:** 📋 **PLANNED** - Low priority, implement after core scene features.

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
    ECS::EntityID GetEntity();
    void ReturnEntity(ECS::EntityID id);
    void Clear();
    
    size_t GetAvailableCount() const;
    size_t GetTotalCount() const;
    
private:
    std::queue<ECS::EntityID> availableEntities_;
    std::unordered_set<ECS::EntityID> activeEntities_;
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
- ✅ Can integrate with ECS (World/EntityFactory)
- ✅ Can manage scene-specific entities
- 📋 Can persist scene state between transitions

### **Enhanced Scene System:**
- 📋 Can configure scenes with different setups
- 📋 Can monitor scene performance
- 📋 Can optimize entity management with pooling
- 📋 Can handle complex scene hierarchies

---

## 🚀 Implementation Priority

### **✅ Phase 1: ECS Integration (COMPLETED)**
1. ✅ **Scene-ECS Integration** - Add World access to Scene class
2. ✅ **Enhanced SceneManager** - Add ECS integration to SceneManager
3. ✅ **Testing** - Validate ECS integration works correctly

### **Phase 2: Configuration & Setup (HIGH)**
1. **Scene Configuration System** - Standardized scene setup
2. **Scene State Persistence** - Entity and data persistence
3. **Integration Testing** - Test with different scene types
4. **Input System Configuration** - Game-specific input behavior (LOW priority)

### **Phase 3: Performance & Optimization (MEDIUM)**
1. **Scene Performance Monitoring** - Add profiling capabilities
2. **Scene Entity Pooling** - Optimize entity management
3. **Performance Testing** - Validate optimizations

---

## 📝 Notes

- ✅ **ECS integration completed** - Scene system now fully integrated with ECS
- **Focus on configuration next** - Scene setup and state persistence are the next priorities
- **Keep it simple** - Avoid over-engineering for homework scope
- **Test incrementally** - Validate each phase before moving to next
- **Performance matters** - Need to support 3-digit entity counts efficiently
- **Integration ready** - Design for seamless ECS and Event system integration

---

## 🎉 Current Achievement Summary

### **✅ Completed Features:**
- **Scene-ECS Integration**: Scenes can create and manage entities through World
- **Enhanced SceneManager**: Unified access to ECS from scene management
- **Entity Lifecycle Management**: CreateSceneEntity, DestroySceneEntity, ClearSceneEntities
- **World Access**: Scenes can access ComponentManager, SystemManager through World

### ** Next Steps:**
- **Scene Configuration**: Standardized scene setup for different game states
- **Scene State Persistence**: Preserve important entities between scene transitions
- **Scene Input Configuration**: Support different input configurations
- **Performance Monitoring**: Add profiling capabilities for optimization

---

*Phase 1 has been successfully completed. The scene system is now fully integrated with ECS and ready for the next phase of development.* 