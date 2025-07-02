# ECS Systems - CS5850 2D Game Engine

This directory contains all the core systems that operate on entities and components in the ECS (Entity-Component-System) architecture. Each system implements specific game logic and is designed to be modular, reusable, and performant.

## 🏗️ System Architecture

All systems inherit from the base `System` class and follow these principles:
- **Single Responsibility**: Each system handles one specific aspect of game logic
- **Component-Based**: Systems operate on entities that have specific component combinations
- **Performance Optimized**: Systems are designed for efficient iteration over large numbers of entities
- **Event-Driven**: Systems can publish and subscribe to events for loose coupling

## 📋 Available Systems

### ✅ **RenderSystem** - Layered Sprite Rendering
**File**: `RenderSystem.hpp/cpp`
**Priority**: 50 (renders after all logic updates)

**Purpose**: Handles layered rendering of all sprite entities with proper depth sorting.

**Components Used**:
- `Transform2D` - Position, rotation, scale
- `Sprite2D` - Texture path, visibility, render layer, tinting

**Key Features**:
- **Layer-based rendering**: Higher `renderLayer` values render on top
- **Automatic sorting**: Sprites sorted by render layer before rendering
- **Visibility culling**: Only renders sprites with `visible = true`
- **Transform support**: Full position, rotation, and scaling
- **Color tinting**: Support for sprite color modulation
- **Performance monitoring**: Tracks rendered sprite count

**Usage Example**:
```cpp
// Background layer (renders first)
auto backgroundEntity = entityFactory.CreateEntity("Background");
componentManager.AddComponent(backgroundEntity, Transform2D{0, 0, 0, 1, 1});
componentManager.AddComponent(backgroundEntity, Sprite2D{"background.png", {}, true, {255,255,255,255}, 0});

// Player layer (renders on top)
auto playerEntity = entityFactory.CreateEntity("Player");
componentManager.AddComponent(playerEntity, Transform2D{100, 100, 0, 1, 1});
componentManager.AddComponent(playerEntity, Sprite2D{"player.png", {}, true, {255,255,255,255}, 10});

// UI layer (renders on top of everything)
auto uiEntity = entityFactory.CreateEntity("UI");
componentManager.AddComponent(uiEntity, Transform2D{0, 0, 0, 1, 1});
componentManager.AddComponent(uiEntity, Sprite2D{"ui.png", {}, true, {255,255,255,255}, 100});
```

---

### ✅ **CollisionSystem** - Advanced Collision Detection
**File**: `CollisionSystem.hpp/cpp`
**Priority**: 10 (high priority - detects collisions first)

**Purpose**: Handles AABB collision detection with layer-based filtering and spatial optimization.

**Components Used**:
- `Transform2D` - World position and scale
- `Collider2D` - Collision bounds, trigger flag, collision layer

**Key Features**:
- **Layer-based collision**: Configure which layers can collide with each other
- **Spatial optimization**: Supports brute force, grid, and QuadTree algorithms
- **Trigger support**: Separate handling for trigger vs solid collisions
- **Event publishing**: Automatically publishes collision events
- **Performance monitoring**: Tracks collision check count and collision count
- **Dynamic configuration**: Runtime collision rule modification

**Collision Types**:
- `BRUTE_FORCE`: O(n²) but simple, good for small entity counts
- `SIMPLE_GRID`: Spatial grid partitioning for medium entity counts
- `QUAD_TREE`: Hierarchical partitioning for large entity counts

**Usage Example**:
```cpp
// Setup collision layers
collisionSystem->AddCollisionLayer("player", true);
collisionSystem->AddCollisionLayer("enemy", true);
collisionSystem->AddCollisionLayer("projectile", true);

// Configure collision rules
collisionSystem->SetCollisionRule("player", "enemy", true);    // Player can hit enemies
collisionSystem->SetCollisionRule("projectile", "enemy", true); // Projectiles can hit enemies
collisionSystem->SetCollisionRule("player", "projectile", false); // Player can't hit own projectiles
```

---

### ✅ **PhysicsSystem** - Velocity-Based Movement
**File**: `PhysicsSystem.hpp/cpp`
**Priority**: 20 (medium priority - handles movement and physics)

**Purpose**: Handles velocity-based movement, gravity, friction, and collision response.

**Components Used**:
- `Transform2D` - Position updates
- `Velocity2D` - Velocity and max speed
- `PhysicsModeComponent` - Physics behavior configuration

**Key Features**:
- **Gravity simulation**: Configurable gravity per entity
- **Friction support**: Air resistance and surface friction
- **Velocity limiting**: Maximum speed constraints
- **Collision response**: Integrates with CollisionSystem for physics reactions
- **Boundary checking**: World boundary collision handling
- **Event-driven**: Responds to collision events for physics reactions

**Physics Modes**:
- Gravity enabled/disabled per entity
- Friction enabled/disabled per entity
- Custom collision response callbacks

**Usage Example**:
```cpp
auto entity = entityFactory.CreateEntity("MovingObject");
componentManager.AddComponent(entity, Transform2D{100, 100});
componentManager.AddComponent(entity, Velocity2D{50.0f, -20.0f, 200.0f}); // vx, vy, maxSpeed
componentManager.AddComponent(entity, PhysicsModeComponent{true, true}); // gravity, friction
```

---

### ✅ **LifetimeSystem** - Entity Lifecycle Management
**File**: `LifetimeSystem.hpp/cpp`
**Priority**: 30 (low priority - cleanup after other systems)

**Purpose**: Manages entity lifetimes and automatic cleanup.

**Components Used**:
- `Lifetime` - Remaining time and destruction behavior

**Key Features**:
- **Automatic cleanup**: Destroys entities when lifetime expires
- **Flexible behavior**: Choose to destroy entity or just remove Lifetime component
- **Time tracking**: Precise countdown using delta time
- **Memory management**: Prevents entity leaks

**Usage Example**:
```cpp
// Create a temporary projectile that self-destructs after 5 seconds
auto projectile = entityFactory.CreateEntity("TempProjectile");
componentManager.AddComponent(projectile, Transform2D{x, y});
componentManager.AddComponent(projectile, Sprite2D{"bullet.png"});
componentManager.AddComponent(projectile, Lifetime{5.0f, true}); // 5 seconds, destroy on expire
```

---

### ✅ **AISystem** - AI Framework Base
**File**: `AISystem.hpp/cpp`
**Priority**: Variable (configurable)

**Purpose**: Provides base infrastructure for AI behaviors with inheritance-based design.

**Components Used**:
- `AIComponent` - AI state and configuration
- `Transform2D` - For position queries and movement

**Key Features**:
- **Inheritance-based**: Games extend AISystem to implement specific behaviors
- **State management**: ACTIVE, INACTIVE, DISABLED states with transitions
- **Essential services**: Position queries, movement control, distance calculation
- **Performance optimization**: Configurable update intervals
- **Pure virtual interface**: Forces game-specific AI implementation

**Design Pattern**:
```cpp
// Game-specific AI implementation
class GameAISystem : public engine::ECS::AISystem {
protected:
    void ProcessAI(EntityID entity, AIComponent& ai, float deltaTime) override {
        // Implement specific AI behavior
        if (ai.targetEntity != 0) {
            float distance = GetDistance(entity, ai.targetEntity);
            if (distance <= ai.detectionRadius) {
                Vector2 targetPos = GetEntityPosition(ai.targetEntity);
                MoveTowards(entity, targetPos, ai.speed);
            }
        }
    }
};
```

## 🔧 System Management

### System Registration
Systems are registered in `Engine::InitializeSystems()` with specific priorities:

```cpp
auto& systemManager = world_.GetSystemManager();

// High priority - detect collisions first
systemManager.AddSystem(std::make_unique<CollisionSystem>(), 10);

// Medium priority - handle movement and collision response  
systemManager.AddSystem(std::make_unique<PhysicsSystem>(), 20);

// Low priority - cleanup
systemManager.AddSystem(std::make_unique<LifetimeSystem>(), 30);

// Lowest priority - render after all logic updates
systemManager.AddSystem(std::make_unique<RenderSystem>(spriteRenderer, resourceManager), 50);
```

### System Lifecycle
All systems follow the same lifecycle:
1. **Construction**: System is created and configured
2. **Init()**: System initializes and sets up dependencies
3. **Update(deltaTime)**: System processes entities each frame
4. **Shutdown()**: System cleans up resources

### System Communication
Systems communicate through:
- **Events**: Publish/subscribe pattern for loose coupling
- **Shared components**: Multiple systems can read/write the same components
- **World state**: Access to global game state through World

## 📊 Performance Considerations

### System Priorities
- **10-19**: High priority (collision detection, input handling)
- **20-29**: Medium priority (physics, movement)  
- **30-39**: Low priority (lifecycle management)
- **40+**: Very low priority (rendering, effects)

### Optimization Tips
1. **Component queries**: Use efficient multi-component queries
2. **Data locality**: Process entities in cache-friendly order
3. **Early exits**: Skip unnecessary processing when possible
4. **Spatial partitioning**: Use appropriate spatial algorithms for large entity counts
5. **Event batching**: Batch event publishing when possible

## 🎯 Usage Patterns

### Entity Creation Pattern
```cpp
// Create a complete game entity
auto entity = entityFactory.CreateEntity("CompleteEntity");

// Add transform (required for most systems)
componentManager.AddComponent(entity, Transform2D{x, y, rotation, scaleX, scaleY});

// Add visual representation
componentManager.AddComponent(entity, Sprite2D{texturePath, sourceRect, visible, tint, renderLayer});

// Add physics
componentManager.AddComponent(entity, Velocity2D{vx, vy, maxSpeed});
componentManager.AddComponent(entity, PhysicsModeComponent{enableGravity, enableFriction});

// Add collision
componentManager.AddComponent(entity, Collider2D{bounds, isTrigger, layer});

// Add lifecycle management (optional)
componentManager.AddComponent(entity, Lifetime{time, destroyOnExpire});

// Add AI (optional)
componentManager.AddComponent(entity, AIComponent{state, targetEntity, detectionRadius, speed});
```

### Query Patterns
```cpp
// Single component query
auto transforms = componentManager.GetEntitiesWithComponent<Transform2D>();

// Multi-component query
auto renderables = componentManager.GetEntitiesWithComponents<Transform2D, Sprite2D>();

// Component access
for (EntityID entity : renderables) {
    auto* transform = componentManager.GetComponent<Transform2D>(entity);
    auto* sprite = componentManager.GetComponent<Sprite2D>(entity);
    // Process entity...
}
```

## 🚀 Future Extensions

- **AudioSystem**: 3D positional audio with Transform2D integration
- **ParticleSystem**: Particle effects with lifecycle management
- **UISystem**: User interface rendering and interaction
- **NetworkSystem**: Multiplayer synchronization
- **ScriptSystem**: Lua/Python scripting integration
- **AnimationSystem**: Sprite animation and tweening

---

## 📖 Related Documentation

- **ECS Architecture**: `../README.md`
- **Components**: `../components/`
- **Engine Integration**: `../../README.md`
- **Development Plan**: `../ECS-Development-Plan.md` 