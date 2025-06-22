# CS5850 2D Game Engine

A modular, reusable 2D game engine in modern C++ (using SDL3).  
**Designed for learning, extensibility, and clean separation between engine and game/demo code.**

---

## 📁 Directory Structure

```text
src/
├── engine/                    # Engine Core (Reusable SDK)
│   ├── core/
│   │   ├── ecs/              # ECS Architecture (COMPLETED ✅)
│   │   │   ├── components/   # Core 2D Components (Transform2D, Sprite2D, etc.)
│   │   │   ├── EntityFactory.hpp/cpp    # Entity lifecycle management
│   │   │   ├── ComponentManager.hpp     # Type-safe component storage
│   │   │   ├── SystemManager.hpp/cpp    # System orchestration
│   │   │   ├── World.hpp/cpp            # Unified ECS interface
│   │   │   └── WorldState.hpp/cpp       # Global state management
│   │   ├── scene/            # SceneManager, Scene Base
│   │   └── event/            # EventSystem
│   ├── graphics/
│   │   ├── renderer/         # Renderer Base
│   │   └── sprite/           # SpriteRenderer
│   ├── input/                # InputManager
│   ├── resource/             # ResourceManager
│   └── utils/                # PathUtils and other utilities
│
├── sandbox/                  # Feature Demos and Integration Tests
│   └── testbed/              # Various Test Scenes
│       ├── render_test/      # Rendering tests (with ECS integration)
│       ├── ecs_test/         # ECS-specific tests (planned)
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

## 🧱 Core Modules

### ✅ **ECS (Entity-Component-System)** - **COMPLETED**
- **EntityFactory**: Thread-safe entity creation/destruction with ID recycling
- **ComponentManager**: Type-safe component storage with template-based queries
- **SystemManager**: Priority-based system execution with pause/resume
- **World**: Unified interface for all ECS functionality
- **Core Components**: Transform2D, Sprite2D, Collider2D, Velocity2D, Lifetime, Tag
- **Performance**: Supports 300+ entities with O(1) component access

### **Renderer**: 2D rendering based on SDL3
### **ResourceManager**: Unified resource management and caching (e.g., textures)
### **InputManager**: Input abstraction
### **EventSystem**: Event publish/subscribe mechanism
### **SceneManager**: Scene switching and management
### **Utilities**: Path handling and other utilities

---

## 🧩 Extensions and Demos

- `sandbox/testbed/render_test/` contains debug scenes with **ECS integration testing**
- `sandbox/testbed/ecs_test/` planned for dedicated ECS testing
- `examples/` provides minimal runnable examples for quick onboarding

---

## 🛠️ Build and Dependencies

- **Dependencies**: SDL3, SDL3_image (automatically fetched via CMake)
- **Build**:
  ```sh
  mkdir build && cd build
  cmake ..
  make
  ./bin/2DEngine
  ```
- **Quick Run**: Use `./run.sh` for build and run

---

## 🧠 Design Philosophy

- **Separation of Engine and Game Logic**: Core code contains no game-specific logic
- **Modularity**: Each subsystem is independent, facilitating extension and maintenance
- **Testability**: All core functionalities have dedicated test scenes
- **ECS-First**: Entity-Component-System architecture for scalable game object management

---

## 🚀 Recent Updates

### **ECS Implementation (Completed)**
- ✅ **EntityFactory**: Complete entity lifecycle management
- ✅ **ComponentManager**: Type-safe component storage with advanced queries
- ✅ **SystemManager**: Priority-based system execution
- ✅ **World Integration**: Unified ECS interface
- ✅ **Core 2D Components**: All essential components implemented
- ✅ **Testing Integration**: ECS tests integrated into DebugScene

### **Testing Features**
- **ECS Test Mode**: Press `1` in DebugScene to enter ECS testing
- **Step-by-Step Testing**: Press `SPACE` to run individual ECS tests
- **Real-time Validation**: Console output shows ECS functionality status

---

## 📚 References and Contributions

- PRs and suggestions are welcome!
- For detailed API and usage, see `examples/` and module comments
- ECS documentation: `src/engine/core/ecs/README.md`
- ECS development plan: `src/engine/core/ecs/ECS-Development-Plan.md`

---

For more detailed module descriptions or contribution guidelines, feel free to add them at the end of this README.



