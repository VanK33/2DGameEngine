# CS5850 2D Game Engine Design

A modular 2D game engine built from scratch in C++ for educational and practical use. The engine focuses on core systems like rendering, ECS, event handling, and collision detection, with future support for scripting, UI, and in-game editing.

## 🎯 Goals

- Understand core game engine architecture  
- Build reusable and extensible engine modules  
- Learn graphics programming, system design, and ECS  
- Prepare for future integration with AI-based agents or in-game planners

## 🧱 Core Modules (MVP)

| Module                  | Description                                                                 |
|-------------------------|-----------------------------------------------------------------------------|
| `Renderer`              | Handles sprite drawing (using **SDL3**)                                     |
| `EntityComponentSystem` | Manages entities and their components in a decoupled fashion                |
| `ResourceManager`       | Caches and reuses textures, audio, and other assets                         |
| `InputSystem`           | Handles keyboard/mouse input and maps to in-game actions                    |
| `EventSystem`           | Decoupled publisher-subscriber system for game events                       |
| `CollisionSystem`       | Supports AABB collision checks (SAT/Pixel-based planned later)              |
| `SceneManager`          | Switches between different game scenes/states (menu, game, etc.)            |
| `GameLoop`              | Controls the frame cycle (input → update → render)                          |


## 🧩 Planned Extensions

- 🌀 **Animation System** – Sprite sheet-based animations  
- 🧠 **Scripting (Lua)** – Integrate Lua for behavior customization  
- 🎵 **Audio System** – Music and SFX via SDL3 audio or OpenAL  
- ⏱ **Time System** – Delta time, time scaling (for slow-mo, replay)  
- 🧪 **Debug Tools** – FPS counter, component viewer, on-screen logging  
- 💾 **Save/Load System** – Store game progress and preferences  
- 🧰 **Editor Toolchain** – Basic map or entity editor for prototyping  
- 🌐 **Networking** – Experimental multiplayer support (optional)


## 🧭 Directory Plan (WIP)

Jeff2DEngine/
├── src/
│   ├── core/             # Game loop, time, system manager
│   ├── ecs/              # Entities, components, and systems
│   ├── graphics/         # Rendering logic (uses SDL3)
│   ├── input/            # Input abstraction
│   ├── events/           # Event system (pub/sub)
│   ├── collision/        # AABB, SAT, etc.
│   ├── resources/        # Texture/audio/font manager
│   ├── scenes/           # Scene/state switching
│   └── main.cpp
├── assets/               # Sprites, sounds, config
├── external/             # SDL3 or third-party deps
├── CMakeLists.txt
└── README.md



## 🛠️ Build Instructions

```bash
# Prerequisites:
# - C++17 compiler
# - SDL3 installed and discoverable via pkg-config or CMake

# Typical build with CMake:
mkdir build && cd build
cmake ..
make
./Jeff2DEngine
```


## 📦 Dependencies

- [SDL3](https://github.com/libsdl-org/SDL) – Cross-platform graphics/input/audio library  
- C++17 or later  
- (optional) Lua – for scripting support  
- (optional) stb_image – for texture loading (or use SDL_image3)  
- (optional) Box2D – if advanced physics are added later


## 📌 Development Status

MVP not yet implemented.  
✅ **First milestone**: `Renderer` + `ECS` + `ResourceManager`.


## 🤖 Notes for Codex

- Use modular architecture with one system per file  
- ECS components should be lightweight and POD-like  
- Avoid global states — pass context explicitly where possible  
- Systems should be update-driven, called from GameLoop  
- Prefer composition over inheritance for entity behavior


## 🙌 Author

**Jeff Ma**  
MSCS student focused on game engine design and agentic AI systems.  
Built for learning, extensibility, and experimentation.
