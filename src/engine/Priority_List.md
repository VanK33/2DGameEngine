# Engine Priority List: 2D Top-Down Zombie Shooting Game

## Executive Summary

This document outlines the **10 most critical tasks** needed to make the engine ready for building a 2D top-down zombie shooting game. The engine will be used as an external library, so focus is on creating a clean, standalone API that can be easily integrated into game projects.

**Target Game:** 2D top-down zombie shooting game with player movement, weapon systems, enemy AI, collision detection, and basic UI.

**Important:** Engine provides **generic systems**, game provides **game-specific components**.

---

## 🎯 Priority Tasks (Ranked by Criticality)

### **1. Complete ECS Core Implementation (CRITICAL - Week 1)**
**Why Critical:** ECS is the foundation for all game objects (player, zombies, bullets, weapons)
**Current State:** Basic ECS exists but missing entity lifecycle and component querying
**Tasks:**
- Implement `EntityFactory` for entity creation/destruction
- Add basic component querying (`GetEntitiesWithComponent<T>`)
- Create `SystemManager` for system execution ordering
- Implement core 2D components: `Transform2D`, `Sprite2D`, `Collider2D`, `Velocity2D`, `Lifetime`, `Tag`
- Integrate ECS with existing `World` class

**Files to Create/Modify:**
- `core/ecs/EntityFactory.hpp/cpp`
- `core/ecs/SystemManager.hpp/cpp`
- `core/ecs/components/Transform2D.hpp`
- `core/ecs/components/Sprite2D.hpp`
- `core/ecs/components/Collider2D.hpp`
- `core/ecs/components/Velocity2D.hpp`
- `core/ecs/components/Lifetime.hpp`
- `core/ecs/components/Tag.hpp`
- `core/ecs/World.hpp/cpp` (extend existing)

**Success Criteria:** Can create entities with position, sprite, collision, velocity, lifetime, and tag components

---

### **2. Implement Core Engine API (CRITICAL - Week 1)**
**Why Critical:** Need a clean, external-facing API for game developers
**Current State:** No unified engine interface
**Tasks:**
- Create main `Engine` class as the primary interface
- Implement engine initialization and shutdown
- Add scene management integration
- Create simple game loop management
- Add resource management integration

**Files to Create/Modify:**
- `Engine.h` (currently empty)
- `Engine.cpp`
- `EngineConfig.hpp` for configuration options

**API Example:**
```cpp
class Engine {
public:
    bool Initialize(const EngineConfig& config);
    void Shutdown();
    void Run();
    void SetScene(std::unique_ptr<Scene> scene);
    World& GetWorld();
    ResourceManager& GetResourceManager();
    InputManager& GetInputManager();
};
```

**Success Criteria:** Game developers can initialize engine and start building games with minimal boilerplate

---

### **3. Implement Collision Detection System (CRITICAL - Week 2)**
**Why Critical:** Essential for any game with object interactions
**Current State:** No collision system exists
**Tasks:**
- Create `CollisionSystem` for ECS
- Implement AABB collision detection
- Add collision layers (configurable by game)
- Create collision event system
- Add spatial partitioning for performance (simple grid)

**Files to Create:**
- `core/ecs/systems/CollisionSystem.hpp/cpp`
- `core/physics/CollisionDetector.hpp/cpp`
- `core/physics/SpatialGrid.hpp/cpp`
- `core/event/events/CollisionEvents.hpp`

**Success Criteria:** Any entities with Collider2D components can detect collisions and trigger events

---

### **4. Implement Input System for Top-Down Movement (CRITICAL - Week 2)**
**Why Critical:** Player needs WASD movement and mouse aiming for shooting
**Current State:** Basic input exists but needs top-down specific features
**Tasks:**
- Extend `InputManager` with mouse position tracking
- Add mouse button support (left click for shooting)
- Implement continuous movement (WASD keys)
- Add input event system for ECS integration
- Create `InputSystem` for ECS

**Files to Create/Modify:**
- `input/InputManager.hpp/cpp` (extend existing)
- `core/ecs/systems/InputSystem.hpp/cpp`
- `core/ecs/components/PlayerInput.hpp`
- `core/event/events/InputEvents.hpp`

**Success Criteria:** Engine provides input events that games can use for any input-based logic

---

### **5. Implement Physics and Movement System (HIGH - Week 2)**
**Why Critical:** Generic movement system that any game can use
**Current State:** No physics system exists
**Tasks:**
- Create `PhysicsSystem` for velocity-based movement
- Implement `MovementSystem` for input-driven movement
- Add `LifetimeSystem` for entity cleanup
- Create generic projectile movement (not weapon-specific)
- Add basic physics constraints

**Files to Create:**
- `core/ecs/systems/PhysicsSystem.hpp/cpp`
- `core/ecs/systems/MovementSystem.hpp/cpp`
- `core/ecs/systems/LifetimeSystem.hpp/cpp`
- `core/physics/PhysicsConstraints.hpp`

**Success Criteria:** Any entity with Velocity2D can move, any entity with Lifetime can be automatically destroyed

---

### **6. Implement Basic AI Framework (HIGH - Week 3)**
**Why Critical:** Generic AI system that games can extend for specific behaviors
**Current State:** No AI system exists
**Tasks:**
- Create `AIComponent` for generic AI data
- Implement `AISystem` for AI processing
- Add basic AI behaviors (follow target, wander, patrol)
- Create AI event system
- Add spawn system for entity creation

**Files to Create:**
- `core/ecs/components/AIComponent.hpp`
- `core/ecs/systems/AISystem.hpp/cpp`
- `core/ecs/systems/SpawnSystem.hpp/cpp`
- `core/ai/AIBehaviors.hpp`
- `core/ai/AIFramework.hpp`

**Success Criteria:** Games can create AI entities that follow targets, wander, or patrol using generic AI system

---

### **7. Implement Event and Messaging Framework (HIGH - Week 3)**
**Why Critical:** Generic event system that games can use for any custom logic
**Current State:** Basic event system exists but needs enhancement
**Tasks:**
- Enhance existing event system with custom event types
- Create event component for ECS integration
- Add event filtering and routing
- Create message passing between systems
- Add event history for debugging

**Files to Create:**
- `core/ecs/components/EventComponent.hpp`
- `core/ecs/systems/EventSystem.hpp/cpp`
- `core/event/EventFramework.hpp/cpp`
- `core/event/MessageSystem.hpp/cpp`
- `core/event/EventHistory.hpp/cpp`

**Success Criteria:** Games can create custom events and systems that communicate through the event framework

---

### **8. Implement Basic UI Framework (MEDIUM - Week 3)**
**Why Critical:** Generic UI system for displaying game information
**Current State:** No UI system exists
**Tasks:**
- Create `UISystem` for rendering UI elements
- Implement text rendering for scores and information
- Add generic UI component for ECS
- Create UI event system
- Add simple menu framework

**Files to Create:**
- `core/ui/UISystem.hpp/cpp`
- `core/ui/TextRenderer.hpp/cpp`
- `core/ui/UIFramework.hpp/cpp`
- `core/ecs/components/UIComponent.hpp`
- `core/ecs/systems/UISystem.hpp/cpp`

**Success Criteria:** Games can display text, health bars, and other UI elements using generic UI system

---

### **9. Implement Audio Framework (MEDIUM - Week 4)**
**Why Critical:** Generic audio system for sound effects and music
**Current State:** No audio system exists
**Tasks:**
- Create `AudioManager` for sound management
- Implement generic sound effect system
- Add background music support
- Create audio components for ECS
- Integrate with existing event system

**Files to Create:**
- `audio/AudioManager.hpp/cpp`
- `audio/SoundEffect.hpp/cpp`
- `audio/MusicPlayer.hpp/cpp`
- `core/ecs/components/AudioComponent.hpp`
- `core/ecs/systems/AudioSystem.hpp/cpp`

**Success Criteria:** Games can play sound effects and music using generic audio system

---

### **10. Implement Game State Framework (MEDIUM - Week 4)**
**Why Critical:** Generic state management for different game phases
**Current State:** Basic scene system exists
**Tasks:**
- Create `GameStateManager` for state transitions
- Implement generic state system (menu, playing, paused, game over)
- Add state event system
- Create state transition framework
- Add basic save/load framework

**Files to Create:**
- `core/game/GameStateManager.hpp/cpp`
- `core/game/GameStates.hpp`
- `core/game/StateFramework.hpp/cpp`
- `core/save/SaveFramework.hpp/cpp`

**Success Criteria:** Games can manage different states (menu, playing, paused) using generic state system

---

## 📊 Implementation Timeline

### **Week 1: Foundation (Tasks 1-2)**
- **Days 1-3:** Complete ECS Core Implementation
- **Days 4-5:** Implement Core Engine API
- **Weekend:** Test basic engine functionality

### **Week 2: Core Systems (Tasks 3-5)**
- **Days 1-2:** Collision Detection System
- **Days 3-4:** Input System for Top-Down Movement
- **Day 5:** Physics and Movement System
- **Weekend:** Test basic systems

### **Week 3: Framework Systems (Tasks 6-8)**
- **Days 1-2:** Basic AI Framework
- **Days 3-4:** Event and Messaging Framework
- **Day 5:** Basic UI Framework
- **Weekend:** Test complete framework

### **Week 4: Polish Framework (Tasks 9-10)**
- **Days 1-2:** Audio Framework
- **Days 3-4:** Game State Framework
- **Day 5:** Integration testing and bug fixes
- **Weekend:** Create demo game using engine

---

## 🎯 Success Criteria for Engine

### **Minimum Viable Engine:**
- ✅ Can initialize and run as external library
- ✅ Supports ECS for game object management
- ✅ Has collision detection and physics
- ✅ Supports top-down movement and mouse input
- ✅ Has generic movement and physics systems
- ✅ Supports basic AI framework
- ✅ Has event and messaging framework
- ✅ Supports basic UI framework
- ✅ Has audio framework
- ✅ Supports game state framework

### **Performance Targets:**
- Support 100+ entities (any type) at 60 FPS
- Collision detection < 5ms per frame
- Input response time < 16ms
- Memory usage < 100MB for typical game

### **API Design Goals:**
- Simple initialization (3-5 lines of code)
- Clear separation between engine and game logic
- Consistent naming conventions
- Good error handling and debugging support

---

## 🚨 Risk Mitigation

### **If Behind Schedule:**
1. **Skip Task 10** - Game state management can be implemented in the game
2. **Simplify Task 9** - Use basic SDL audio instead of complex audio system
3. **Focus on core systems** - Prioritize Tasks 1-5 over polish features
4. **Use existing systems** - Leverage current scene and event systems

### **If Performance Issues Arise:**
1. **Profile first** - Don't optimize prematurely
2. **Reduce entity count** - Design games with fewer objects
3. **Simplify collision** - Use basic AABB instead of complex shapes
4. **Optimize rendering** - Batch similar sprites

---

## 💡 Development Tips

### **Start Simple:**
- Build a basic prototype with just player movement and generic physics
- Add AI entities one feature at a time
- Test each system independently before integration

### **Focus on API Design:**
- Design for external use from the beginning
- Keep interfaces simple and consistent
- Provide good documentation and examples

### **Test Early:**
- Create test scenes for each system
- Validate that external games can use the engine
- Fix API issues before adding features

### **Remember Engine vs Game Separation:**
- **Engine provides:** Generic systems (ECS, collision, physics, AI framework, events)
- **Game provides:** Game-specific components (health, weapons, zombies, power-ups)
- **Engine handles:** How things work (movement, collision, rendering, events)
- **Game handles:** What things are (health systems, weapons, enemies, items)

---

## 📚 Essential Resources

- [SDL3 Documentation](https://wiki.libsdl.org/) - For rendering and input
- [ECS Best Practices](https://github.com/SanderMertens/ecs-faq) - For ECS design
- [Game Programming Patterns](https://gameprogrammingpatterns.com/) - For architecture
- [2D Game Development](https://www.gamasutra.com/view/feature/132500/designing_fast_maintainable_event_.php) - For 2D game design

---

*This priority list focuses on building a complete, usable engine with generic systems that any 2D game can use. Game-specific components like health, weapons, zombies, and power-ups should be implemented in the game, not the engine.* 