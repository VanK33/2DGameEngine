# CS5850 2D Game Engine

A modular, reusable 2D game engine in modern C++ (using SDL3).  
**Designed for learning, extensibility, and clean separation between engine and game/demo code.**

**Current Focus: Building a complete engine for 2D top-down zombie shooting games**

---

## 📁 Directory Structure

```text
src/
├── engine/                    # Engine Core (Reusable SDK)
│   ├── core/
│   │   ├── ecs/              # ECS Architecture (COMPLETED ✅)
│   │   │   ├── components/   # Core 2D Components (Transform2D, Sprite2D, etc.)
│   │   │   ├── systems/      # Core Systems (Physics, Collision, Lifetime)
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

### ✅ **ECS (Entity-Component-System)** - **COMPLETED**
- **EntityFactory**: Thread-safe entity creation/destruction with ID recycling
- **ComponentManager**: Type-safe component storage with template-based queries
- **SystemManager**: Priority-based system execution with pause/resume
- **World**: Unified interface for all ECS functionality
- **Core Components**: Transform2D, Sprite2D, Collider2D, Velocity2D, Lifetime, Tag
- **Core Systems**: CollisionSystem, PhysicsSystem, LifetimeSystem
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

---

## 🚀 Recent Achievements

### **Complete ECS Implementation**
- ✅ **Full Entity Lifecycle**: Create, destroy, query entities efficiently
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

---

## 🎯 Current Development Focus

### 🔄 **In Progress**
- **Core Engine API**: Creating unified Engine class for external use
- **AI Framework**: Basic AI components and behavior system
- **Enhanced Physics**: Advanced collision response and constraints

### 📋 **Planned Next**
- **Audio System**: Sound effects and background music
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

### **Testing**
- Press `1` in DebugScene to enter ECS testing mode
- Press `SPACE` to run step-by-step ECS tests
- Console output shows detailed system status

---

## 🧠 Design Philosophy

- **Engine vs Game Separation**: Engine provides generic systems, games provide specific components
- **ECS-First Architecture**: All game objects managed through Entity-Component-System
- **Event-Driven Communication**: Systems communicate via events, not direct coupling
- **Modular Design**: Each subsystem is independent and testable
- **Performance Focus**: Optimized for hundreds of entities at 60 FPS

---

## 📈 Performance Targets

- **Entities**: 300+ entities with full ECS functionality
- **Collision Detection**: < 5ms per frame for typical game scenarios
- **Input Response**: < 16ms latency
- **Memory Usage**: < 100MB for typical 2D games
- **Frame Rate**: Consistent 60 FPS with complex scenes

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

## 🤝 Contributing

- PRs and suggestions welcome!
- Focus on generic engine systems, not game-specific features
- Maintain clean separation between engine and game logic
- Follow existing code style and architecture patterns

---

*This engine represents a complete, production-ready foundation for 2D games with a focus on performance, modularity, and clean architecture. All core systems are implemented and tested.*



