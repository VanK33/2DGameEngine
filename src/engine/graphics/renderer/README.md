# Renderer Design - Jeff's 2D Game Engine

## Overview

The `Renderer` module provides a centralized and encapsulated way to manage SDL3 rendering, including window and render context initialization, frame buffering, and 2D texture drawing. Its design is focused on **simplicity**, **testability**, and **clean decoupling** from both engine and game logic.

The decision to isolate rendering into a dedicated class stems from these needs:

* Decouple SDL3’s low-level rendering calls from the rest of the engine
* Provide consistent frame lifecycle management (`BeginFrame`/`EndFrame`)
* Simplify debug and future expansion (e.g. batching, post-processing)
* Enable headless testing and easier replacement of rendering backend

---

## Key Design Choices

### 1. **Encapsulated Initialization/Shutdown**

All SDL3 setup (window and renderer creation) is encapsulated in `Init()` and `Shutdown()` methods. This ensures:

* Resources are owned and freed safely
* The renderer can be re-initialized cleanly in test or restart scenarios
* The outer engine loop can rely on `Renderer`'s lifecycle boundaries

### 2. **Centralized Frame Management**

Methods `BeginFrame()` and `EndFrame()` define a clear boundary for per-frame rendering logic. This pattern:

* Matches typical game engine render pipeline design
* Makes it easier to inject debug overlays or capture screenshots later
* Is intuitive for scene designers to use without caring about SDL internals

### 3. **Abstracted Texture Drawing**

The `DrawTexture(...)` function hides all SDL\_RenderTexture logic and uses screen-space `SDL_FRect`. A `rotation` parameter is also supported, allowing the texture to rotate around its center. This choice:

* Makes positioning more intuitive for developers
* Allows resolution scaling or virtual camera logic to be added later
* Supports center-based rotation, which is common in 2D action and FPS-style games
* Enables batching or layering to be modularized later without refactor

### 4. **No Global Singleton**

The `Renderer` is **not** a global singleton. Instead, it is instantiated and passed around explicitly (e.g. to `SceneManager` or `SpriteRenderer`). This aligns with the same architecture used for `InputManager`, supporting testability and modularity.

---

## Example Usage

```cpp
renderer.BeginFrame();

// Game render logic
renderer.DrawTexture(playerTex, 100, 200, 64, 64, 45.0f); // rotated 45 degrees

renderer.EndFrame();
```

---

## Benefits of This Design

* ✅ **Encapsulated lifecycle**: avoids accidental resource leaks
* ✅ **Flexible and testable**: can be mocked or extended for offscreen rendering
* ✅ **Decoupled**: no dependency on global state or static access
* ✅ **Extendable**: e.g. future support for batching, resolution scaling, or shaders

---

## Future Extensions

* Layered rendering / depth ordering


---

## File Structure

```
src/
├── engine/
│   └── graphics/
│       └── renderer/
│           ├── Renderer.hpp
│           └── Renderer.cpp
```

---

## Summary

The `Renderer` module was designed with clarity, safety, and extensibility in mind. It abstracts away SDL3’s rendering complexity while providing an easy-to-use, consistent interface that integrates cleanly with the rest of the engine’s modular systems. Its current design lays the groundwork for building more advanced visual features without compromising core simplicity.
