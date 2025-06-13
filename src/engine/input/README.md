# InputManager Design - Jeff's 2D Game Engine

## Overview

The `InputManager` module in this game engine is responsible for managing and dispatching user input from SDL events (e.g., keyboard and mouse) to relevant parts of the game. This centralized input handling facilitates clean architecture and decoupled system design. The decision to build a dedicated `InputManager` stems from the need to:

* Provide a unified way to query and poll input states
* Separate SDL logic from scene logic
* Support flexible event listener systems
* Enable easier testing and simulation of input

---

## Key Design Choices

### 1. **Singleton-Like Central Access (but externally owned)**

Rather than making `InputManager` a hard singleton, it is instantiated and injected (e.g., into `SceneManager`). This improves testability and avoids tight coupling, while still behaving as a single access point in practice.

### 2. **State-Based Key Tracking**

We maintain:

* `pressedKeys_` (keys currently held down)
* `releasedKeys_` (keys released in the last frame)
* `justPressedKeys_` (keys pressed this frame only)

This supports nuanced gameplay interactions, like triggering an action only on the key's rising or falling edge.

### 3. **Frame-Based State Refresh**

The `Update()` method clears `justPressedKeys_` and `releasedKeys_` after each frame, ensuring state values are relevant to the current frame only.

### 4. **Mouse Input Support**

Basic mouse support is included with:

* `mousePosition_`
* `mouseButtons_`

This allows querying position and button states, extensible for future drag, wheel, and gesture support.

### 5. **Non-Blocking Input Query Interface**

Consumers can call methods like:

* `IsKeyPressed(SDL_Scancode key)`
* `IsKeyJustPressed(SDL_Scancode key)`
* `IsKeyReleased(SDL_Scancode key)`

These methods make querying intuitive and consistent without SDL dependency.

---

## Example Usage

```cpp
if (inputManager->IsKeyJustPressed(SDL_SCANCODE_SPACE)) {
    player.Jump();
}
```

```cpp
Vector2 mousePos = inputManager->GetMousePosition();
```

---

## Benefits of This Design

* ✅ Separation of concerns: decouples low-level SDL events from scene logic
* ✅ Extensible: easy to add joystick/gamepad support later
* ✅ Testable: mockable interface for input during unit testing
* ✅ Clean: per-frame tracking avoids unintended key repeat issues

---

## Future Extensions

* Input binding remapping
* Input event callback registration

---

## File Structure

```
src/
├── engine/
│   └── input/
│       ├── InputManager.hpp
│       └── InputManager.cpp
```

---

## Summary

The `InputManager` was designed to bring clarity, flexibility, and scalability to input handling. By treating input as a queryable service and separating concerns properly, this approach lays a solid foundation for both simple prototypes and more complex gameplay systems.
