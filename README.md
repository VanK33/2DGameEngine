# CS5850 2D Game Engine Design

A standalone, reusable 2D game engine built in modern C++ using SDL3.  
Designed for learning, extensibility, and powering multiple 2D games through modular architecture.

---

## 🎯 Goals

- Understand the architecture of a 2D game engine  
- Build reusable and extensible engine modules  
- Learn graphics programming, system design, and ECS patterns  
- Enable future integration with scripting, AI agents, and planners  
- Maintain clear separation between **engine** and **game-specific logic**

---

## 🧱 Core Modules (MVP)

| Module                  | Description                                                                 | Scope   |
|-------------------------|-----------------------------------------------------------------------------|---------|
| `Renderer`              | Handles sprite drawing (using **SDL3**)                                     | Engine  |
| `EntityComponentSystem` | Manages entities and their components in a decoupled fashion                | Engine  |
| `ResourceManager`       | Caches and reuses textures, audio, and other assets                         | Engine  |
| `InputSystem`           | Handles keyboard/mouse input and maps to in-game actions                    | Engine  |
| `EventSystem`           | Decoupled publisher-subscriber system for game events                       | Engine  |
| `CollisionSystem`       | Supports AABB collision checks (SAT/Pixel-based planned later)              | Engine  |
| `SceneManager`          | Switches between different game scenes/states (menu, game, etc.)            | Engine  |
| `GameLoop`              | Controls the frame cycle (input → update → render)                          | Engine  |

---

## 🧩 Planned Extensions

- 🌀 **Animation System** – Sprite sheet-based animations  
- 🧠 **Scripting (Lua)** – Integrate Lua for behavior customization  
- 🎵 **Audio System** – Music and SFX via SDL3 audio or OpenAL  
- ⏱ **Time System** – Delta time, time scaling (for slow-mo, replay)  
- 🧪 **Debug Tools** – FPS counter, component viewer, on-screen logging  
- 💾 **Save/Load System** – Store game progress and preferences  
- 🧰 **Editor Toolchain** – Basic map or entity editor for prototyping  
- 🌐 **Networking** – Experimental multiplayer support (optional)

⚠️ All future features will be designed as **optional extensions**, keeping the core engine lightweight and reusable.

---

## 🧠 Engine Design Philosophy

This engine is **game-agnostic** and designed to be reused across multiple projects.

Core design principles:
- Keep all systems modular, loosely coupled, and self-contained
- Avoid embedding game-specific logic in engine code
- Let future games define their own:
  - Entities and components
  - Scenes and game-specific systems
  - Input mappings and behavior
- Support clean layering:
  - `Engine Core`: modules that apply to any 2D game
  - `Game Layer`: game-specific logic, scenes, and assets

---

## 📊 Module Interaction (Simplified)

```text
2DEngine/
├── src/
│   ├── core/             # Game loop, time, scene manager
│   ├── ecs/              # Entities, components, and systems
│   ├── graphics/         # Rendering logic (uses SDL3)
│   ├── input/            # Input abstraction
│   ├── events/           # Event system (pub/sub)
│   ├── resources/        # Texture/audio/font manager
│   ├── scenes/           # Scene/state interface & implementations
│   └── main.cpp
├── assets/               # Sprites, sounds, config
├── external/             # SDL3 or third-party deps
├── CMakeLists.txt
└── README.md
```



