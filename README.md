# CS5850 2D Game Engine

A modular, reusable 2D game engine in modern C++ (using SDL3).  
**Designed for learning, extensibility, and clean separation between engine and game/demo code.**

**Current Focus: Building a complete engine for 2D top-down zombie shooting games**

---

## 📁 Directory Structure

```text
src/
├── engine/                    # Engine Core (Reusable SDK)
│   ├── Engine.hpp/cpp        # Unified Engine API (COMPLETED ✅)
│   ├── core/
│   │   ├── ecs/              # ECS Architecture (COMPLETED ✅)
│   │   │   ├── components/   # Core 2D Components (Transform2D, Sprite2D, etc.) + AI Component
│   │   │   ├── systems/      # Core Systems (Physics, Collision, Lifetime, AI)
│   │   │   ├── EntityFactory.hpp/cpp    # Entity lifecycle management
│   │   │   ├── ComponentManager.hpp     # Type-safe component storage
│   │   │   ├── SystemManager.hpp/cpp    # System orchestration
│   │   │   ├── World.hpp/cpp            # Unified ECS interface
│   │   │   └── WorldState.hpp/cpp       # Global state management
│   │   ├── scene/            # Scene Management (COMPLETED ✅)
│   │   ├── event/            # Event System (COMPLETED ✅)
│   │   └── Types.hpp         # Core type definitions
│   ├── graphics/
│   │   ├── renderer/         # Core Renderer (COMPLETED ✅)
│   │   └── sprite/           # SpriteRenderer (COMPLETED ✅)
│   ├── input/                # Input Management (COMPLETED ✅)
│   ├── resource/             # Resource Management (COMPLETED ✅)
│   └── utils/                # PathUtils and other utilities
│
├── sandbox/                  # Feature Demos and Integration Tests
│   ├── main.cpp              # Engine API Test Program
│   └── testbed/              # Various Test Scenes
│       ├── render_test/      # Rendering tests (with ECS integration)
│       ├── ecs_test/         # ECS-specific tests
│       └── integration/      # System integration tests
│
├── examples/                 # Minimal/Basic Usage Examples
│   └── README.md
│
├── tests/                    # Unit Tests
│
├── assets/                   # Asset Files
└── CMakeLists.txt
```

---

## 🧱 Core Modules Status

### ✅ **Unified Engine API** - **COMPLETED**
- **Engine Class**: Complete unified engine interface with simple 3-5 line initialization
- **EngineConfig**: Flexible engine configuration structure
- **System Integration**: All core systems fully integrated and tested
- **Lifecycle Management**: Complete initialization/run/shutdown flow
- **API Validation**: All API tests pass, production-ready

### ✅ **ECS (Entity-Component-System)** - **COMPLETED**
- **EntityFactory**: Thread-safe entity creation/destruction with ID recycling
- **ComponentManager**: Type-safe component storage with template-based queries
- **SystemManager**: Priority-based system execution with pause/resume
- **World**: Unified interface for all ECS functionality
- **Core Components**: Transform2D, Sprite2D, Collider2D, Velocity2D, Lifetime, Tag, AIComponent
- **Core Systems**: CollisionSystem, PhysicsSystem, LifetimeSystem, AISystem
- **Performance**: Supports 300+ entities with O(1) component access

### ✅ **Event System** - **COMPLETED**
- **EventManager**: Thread-safe publish/subscribe system with priority support
- **Event Filtering**: Complete filtering infrastructure with TypeFilter, PriorityFilter
- **Event Types**: Comprehensive 2D game event types (Input, Physics, Render, Scene)
- **Integration**: Seamless integration with all engine systems

### ✅ **Input Management** - **COMPLETED**
- **InputManager**: Complete SDL3 input abstraction
- **Key States**: Down, held, up state tracking
- **Mouse Support**: Position, delta, button states
- **Event Integration**: Automatic event publishing
- **Combined Queries**: Multi-key and multi-button support

### ✅ **Scene Management** - **COMPLETED**
- **SceneManager**: Dynamic scene switching with event-driven transitions
- **Scene Base Class**: Comprehensive lifecycle management
- **ECS Integration**: Scenes can create and manage entities
- **Event Integration**: Scene transitions via event system

### ✅ **Collision System** - **COMPLETED**
- **CollisionSystem**: AABB collision detection with layer-based filtering
- **Dynamic Layers**: Runtime collision layer management
- **Event Publishing**: Automatic collision event generation
- **Performance Monitoring**: Collision statistics and optimization

### ✅ **Physics System** - **COMPLETED**
- **PhysicsSystem**: Velocity-based movement with gravity and friction
- **Physics Modes**: Different physics behaviors per entity
- **Boundary Checking**: World boundary collision
- **Event Integration**: Collision event handling

### ✅ **Rendering System** - **COMPLETED**
- **Renderer**: Core SDL3 rendering abstraction with frame management
- **SpriteRenderer**: 2D sprite rendering with rotation and flipping
- **Texture Support**: Integrated with ResourceManager

### ✅ **Resource Management** - **COMPLETED**
- **ResourceManager**: Unified resource loading and caching
- **Texture Management**: SDL3 texture loading and lifetime management
- **Path Utilities**: Cross-platform path handling

### ✅ **AI Framework** - **COMPLETED**
- **AIComponent**: Minimal AI component with state management (ACTIVE, INACTIVE, DISABLED)
- **AISystem**: Abstract base class providing essential AI services
- **Basic Services**: Entity positioning, movement control, distance calculation
- **Inheritance-Based Design**: Games extend AISystem to implement specific AI behaviors
- **Performance Optimized**: Configurable update intervals to reduce CPU overhead
- **State Management**: Built-in state transitions with notification hooks

---

## 🚀 Recent Achievements

### **Complete Engine API Implementation**
- ✅ **Unified Interface**: Engine class provides unified access to all systems
- ✅ **Simple Initialization**: 3-5 lines of code to start the engine
- ✅ **System Integration**: All core systems fully integrated
- ✅ **API Validation**: Passes complete API test suite

### **Full ECS Implementation**
- ✅ **Complete Entity Lifecycle**: Efficient entity creation, destruction, querying
- ✅ **Advanced Component Queries**: Multi-component entity retrieval
- ✅ **System Integration**: All systems work seamlessly with ECS
- ✅ **Performance Optimized**: Supports hundreds of entities at 60 FPS

### **Comprehensive Event System**
- ✅ **Priority-Based Processing**: Critical events processed first
- ✅ **Advanced Filtering**: Type, priority, and composite filters
- ✅ **Thread-Safe Operations**: Concurrent event publishing and processing
- ✅ **Complete Integration**: All systems communicate via events

### **Collision Detection & Physics**
- ✅ **Layer-Based Collision**: Configurable collision rules between layers
- ✅ **Event-Driven Response**: Collision events trigger game logic
- ✅ **Physics Integration**: Velocity-based movement with realistic physics
- ✅ **Performance Monitoring**: Real-time collision statistics

### **Input & Scene Management**
- ✅ **Complete Input Abstraction**: Mouse and keyboard with state tracking
- ✅ **Scene Lifecycle**: Proper load/unload/update/render cycles
- ✅ **ECS-Scene Integration**: Scenes can manage entities directly

### **Minimal AI Framework**
- ✅ **Inheritance-Based Design**: Minimal engine core with game-layer extension
- ✅ **Essential Services**: Position queries, movement control, state management
- ✅ **Performance Optimized**: Configurable update intervals
- ✅ **Clean Architecture**: Pure virtual ProcessAI forces game-specific implementation

---

## 🎯 Current Development Focus

### 📋 **Planned Next**
- **Game Examples**: Building complete 2D top-down shooter demonstration
- **Performance Optimization**: Further optimizing collision detection and rendering
- **Audio System**: Sound effects and background music support
- **UI Framework**: Basic UI components and text rendering
- **Game State Management**: Save/load and state transitions

---

## 🛠️ Build and Usage

### **Dependencies**
- SDL3, SDL3_image (automatically fetched via CMake)
- C++20 compatible compiler

### **Build Instructions**
  ```sh
  mkdir build && cd build
  cmake ..
  make
  ./bin/2DEngine
  ```

### **Quick Run**
```sh
./run.sh  # Build and run in one command
```

### **Engine API Usage Example**
```cpp
#include "engine/Engine.hpp"

int main() {
    // 1. Create engine
    engine::Engine gameEngine;
    
    // 2. Configure engine
    engine::EngineConfig config;
    config.windowTitle = "My Game";
    config.windowWidth = 800;
    config.windowHeight = 600;
    
    // 3. Initialize engine
    if (!gameEngine.Initialize(config)) {
        return -1;
    }
    
    // 4. Register scenes
    gameEngine.RegisterScene<MyGameScene>("MainGame");
    gameEngine.ChangeScene("MainGame");
    
    // 5. Run game
    gameEngine.Run();
    gameEngine.Shutdown();
    
    return 0;
}
```

### **AI System Usage Example**
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
    
    void OnStateChanged(EntityID entity, AIState oldState, AIState newState) override {
        // Handle state transitions
        if (newState == AIState::DISABLED) {
            StopMovement(entity);
        }
    }
};
```

### **Testing**
- Run `./bin/2DEngine` to see complete Engine API tests
- All core system functionality is verified
- Console output shows detailed system status

---

## 🧠 Design Philosophy

- **Engine vs Game Separation**: Engine provides generic systems, games provide specific components
- **ECS-First Architecture**: All game objects managed through Entity-Component-System
- **Event-Driven Communication**: Systems communicate via events, not direct coupling
- **Modular Design**: Each subsystem is independent and testable
- **Minimal Engine Core**: Engine provides essential services, games implement specific logic
- **Performance Focus**: Optimized for hundreds of entities at 60 FPS

---

## 📈 Performance Targets

- **Entities**: 300+ entities with full ECS functionality
- **Collision Detection**: < 5ms per frame for typical game scenarios
- **Input Response**: < 16ms latency
- **Memory Usage**: < 100MB for typical 2D games
- **Frame Rate**: Consistent 60 FPS with complex scenes
- **AI Processing**: Configurable update rates for optimal performance

---

## 🎮 Target Games

This engine is specifically designed for:
- **2D Top-Down Shooters** (primary focus)
- **2D Platformers**
- **2D Puzzle Games**
- **2D RPGs**

The engine provides generic systems while games implement specific components like health, weapons, enemies, and power-ups.

---

## 📚 Documentation

- **ECS Documentation**: `src/engine/core/ecs/README.md`
- **Development Plans**: 
  - `src/engine/core/ecs/ECS-Development-Plan.md`
  - `src/engine/core/event/Event-Development-Plan.md`
  - `src/engine/input/Input-Development-Plan.md`
  - `src/engine/core/scene/Scene-Development-Plan.md`
- **Priority List**: `src/engine/Priority_List.md`

---

## 🎯 Next Important Tasks

### **1. Complete Game Example** (Highest Priority)
- Build 2D top-down zombie shooter demonstration
- Validate all engine systems in real usage
- Provide complete game development reference

### **2. Audio System**
- Sound effect playback and background music support
- Integration with event system
- 3D audio positioning support

### **3. UI Framework**
- Basic UI components and text rendering
- Integration with input and event systems
- Layout management

---

## 🤝 Contributing

- PRs and suggestions welcome!
- Focus on generic engine systems, not game-specific features
- Maintain clean separation between engine and game logic
- Follow existing code style and architecture patterns

---



