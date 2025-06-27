# ResourceManager Design - 2D Game Engine

## Overview

The `ResourceManager` module provides centralized resource loading, caching, and lifecycle management for game assets, particularly textures. It is designed to handle **efficient resource reuse**, **automatic memory management**, and **reference counting** to prevent resource leaks while maintaining optimal performance.

The decision to build a dedicated `ResourceManager` stems from these needs:

* Centralize asset loading and prevent duplicate resource allocation
* Provide automatic reference counting for safe resource sharing
* Abstract platform-specific resource loading details
* Enable efficient resource caching and batch operations
* Support fallback mechanisms for missing assets

**Status: COMPLETED ✅ - Production Ready**

---

## Key Design Choices

### 1. **Reference Counting System**

The ResourceManager implements automatic reference counting for all loaded textures:

* `LoadTexture()` increments reference count (or loads if new)
* `ReleaseTexture()` decrements reference count
* Resources are automatically freed when reference count reaches zero
* Prevents memory leaks and duplicate loading

### 2. **Path Normalization**

All asset paths are normalized to ensure consistent caching:

* Handles relative and absolute paths uniformly
* Prevents cache misses due to path variations
* Supports cross-platform path handling

### 3. **Fallback Texture Support**

The system supports fallback textures for missing assets:

* `SetFallbackTexture()` defines a default texture
* Failed loads return fallback instead of null
* Prevents crashes from missing assets during development

### 4. **Thread-Safe Operations**

Resource operations are designed to be safe for concurrent access:

* Proper mutex protection for cache operations
* Safe for use across multiple systems
* Supports multithreaded resource loading

### 5. **Comprehensive Logging**

Detailed logging for debugging and optimization:

* Resource loading/unloading events
* Reference count tracking
* Cache status reporting
* Performance timing information

---

## Current Implementation Status

### ✅ **Completed Features**

- **Texture Loading**: Complete SDL3_image integration
- **Reference Counting**: Automatic memory management
- **Path Normalization**: Cross-platform path handling
- **Fallback Support**: Missing asset handling
- **Cache Management**: Efficient resource reuse
- **Debug Logging**: Comprehensive status reporting
- **Memory Safety**: Proper resource cleanup

### 📋 **Future Extensions**

- Audio resource management
- Font loading and caching
- Batch loading operations
- Asynchronous resource loading
- Resource streaming for large assets

---

## Example Usage

```cpp
// Initialize ResourceManager
ResourceManager resourceManager(renderer);

// Load textures (automatically cached)
SDL_Texture* playerTexture = resourceManager.LoadTexture("sprites/player.png");
SDL_Texture* enemyTexture = resourceManager.LoadTexture("sprites/enemy.png");

// Resources are automatically reference counted
SDL_Texture* sameTexture = resourceManager.LoadTexture("sprites/player.png"); // Reuses cached version

// Release when done (automatic cleanup when ref count reaches 0)
resourceManager.ReleaseTexture("sprites/player.png");
resourceManager.ReleaseTexture("sprites/player.png"); // Texture freed here

// Set fallback for missing assets
resourceManager.SetFallbackTexture(defaultTexture);

// Debug cache status
resourceManager.PrintCacheStatus();
```

---

## Integration with Engine Systems

### **Renderer Integration**
- Requires SDL_Renderer for texture creation
- Seamlessly works with SpriteRenderer
- Supports all SDL3 texture formats

### **ECS Integration**
- Used by Sprite2D components for texture management
- Automatic cleanup when entities are destroyed
- Efficient sharing of textures across entities

### **Scene Integration**
- Scene-specific resource loading and cleanup
- Automatic resource management during scene transitions
- Support for preloading scene assets

---

## Performance Characteristics

- **Memory Efficient**: Reference counting prevents duplication
- **Fast Access**: O(1) cache lookup for loaded resources
- **Minimal Overhead**: Lightweight caching mechanism
- **Scalable**: Handles hundreds of textures efficiently

---

## File Structure

```
src/
├── engine/
│   └── resource/
│       ├── ResourceManager.hpp
│       ├── ResourceManager.cpp
│       └── README.md
```

---

## Benefits

* ✅ **Memory Safe**: Automatic reference counting prevents leaks
* ✅ **Performance Optimized**: Efficient caching and reuse
* ✅ **Developer Friendly**: Comprehensive logging and debugging
* ✅ **Robust**: Fallback support for missing assets
* ✅ **Scalable**: Handles large numbers of resources efficiently
* ✅ **Cross-Platform**: Works across different operating systems

---

## Summary

The `ResourceManager` provides a robust, efficient foundation for asset management in 2D games. Its reference counting system ensures memory safety while the caching mechanism optimizes performance. The comprehensive logging and fallback support make it ideal for both development and production environments.
