# SpriteRenderer Design - Jeff's 2D Game Engine

## Overview

The `SpriteRenderer` class builds on top of the low-level `Renderer`, offering a higher-level abstraction specialized for 2D sprite rendering. It is designed to handle common needs such as **rotation**, **flipping**, and **intuitive screen placement**, enabling rapid iteration for 2D games without coupling game logic to SDL internals.

This renderer is commonly used by game entities (e.g., players, enemies, bullets) and complements the `Renderer` class by enabling per-sprite transformations.

---

## Key Design Choices

### 1. **Center-Based Rotation**

`Draw()` takes a `rotation` parameter and rotates around the geometric center (`width / 2`, `height / 2`) of the sprite. This reflects a design choice inspired by common 2D action and FPS games, where rotation-based aiming or projectile motion is standard and expected.

### 2. **Texture Flipping Support**

The `SDL_FlipMode flip` parameter supports horizontal/vertical flipping. This allows:

* Directional changes (left/right) for characters
* Sprite reuse in mirrored animations
* Efficient rendering logic for symmetrical behavior

### 3. **No Ownership of Textures**

`SpriteRenderer` does not manage the texture lifecycle. It purely renders what it’s given, adhering to the **Single Responsibility Principle** and allowing integration with asset systems, ECS frameworks, or custom loaders.

### 4. **Dependency Injection (No Global Access)**

The class accepts an external SDL\_Renderer pointer. This matches the architecture of other engine subsystems (like `InputManager`), supporting modularity and testability.

---

## Example Usage

```cpp
spriteRenderer->Draw(playerTexture, 100, 200, 64, 64, 90.0f); // rotated 90 degrees
spriteRenderer->Draw(enemyTexture, 300, 400, 32, 32, 0.0f, SDL_FLIP_HORIZONTAL); // mirrored
```

---

## Benefits

* ✅ **Rotation-ready**: Center-based by default, enabling dynamic movement
* ✅ **Flip support**: Reduces sprite memory usage
* ✅ **Minimal abstraction**: Simple API without hiding low-level flexibility
* ✅ **Composable**: Cleanly integrates into scene systems or render queues

---

## Future Extensions

* Z-index-based layered rendering
* Depth ordering for entity overlap

---

## File Structure

```
src/
├── engine/
│   └── graphics/
│       └── sprite/
│           ├── SpriteRenderer.hpp
│           └── SpriteRenderer.cpp
```

---

## Summary

`SpriteRenderer` simplifies sprite transformations like rotation and flip while maintaining flexibility and modularity. It is purpose-built for 2D game logic, empowering the engine to scale up with minimal assumptions and maximum clarity.
