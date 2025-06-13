# CS5850 2D Game Engine

A modular, reusable 2D game engine in modern C++ (using SDL3).  
**Designed for learning, extensibility, and clean separation between engine and game/demo code.**

---

## 📁 Directory Structure

```text
src/
├── engine/                    # Engine Core (Reusable SDK)
│   ├── core/
│   │   ├── ecs/              # ECS Base: World, System, Entity, Component
│   │   ├── scene/            # SceneManager, Scene Base
│   │   └── event/            # EventSystem
│   ├── graphics/
│   │   ├── renderer/         # Renderer Base
│   │   └── sprite/           # SpriteRenderer
│   ├── input/                # InputManager
│   ├── resource/             # ResourceManager
│   └── PathUtils.hpp/cpp     # Utility Classes
│
├── sandbox/                  # Feature Demos and Integration Tests
│   └── testbed/              # Various Test Scenes (e.g., DebugScene, SpriteScene)
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

- **ECS**: Entity-Component-System architecture, decoupling game objects and behaviors
- **Renderer**: 2D rendering based on SDL3
- **ResourceManager**: Unified resource management and caching (e.g., textures)
- **InputManager**: Input abstraction
- **EventSystem**: Event publish/subscribe mechanism
- **SceneManager**: Scene switching and management
- **Utilities**: Path handling and other utilities

---

## 🧩 Extensions and Demos

- `sandbox/testbed/` contains debug scenes, rendering tests, etc.
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

---

## 🧠 Design Philosophy

- **Separation of Engine and Game Logic**: Core code contains no game-specific logic
- **Modularity**: Each subsystem is independent, facilitating extension and maintenance
- **Testability**: All core functionalities have dedicated test scenes

---

## 📚 References and Contributions

- PRs and suggestions are welcome!
- For detailed API and usage, see `examples/` and module comments

---

For more detailed module descriptions or contribution guidelines, feel free to add them at the end of this README.



