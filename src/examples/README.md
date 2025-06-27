# Engine Examples - 2D Game Engine

## Overview

This directory contains practical examples and tutorials demonstrating how to use the 2D Game Engine. Each example is designed to showcase specific engine features and provide templates for common game development patterns.

**Current Status: BASIC EXAMPLES PLANNED 📋**

---

## Example Categories

### 🎮 **Basic Usage Examples**

#### **hello_world/**
- **Status**: 📋 PLANNED
- **Purpose**: Minimal engine initialization and window creation
- **Features**: Basic Engine API usage, window setup, main loop
- **Target Audience**: First-time users

#### **ecs_basics/**
- **Status**: 📋 PLANNED  
- **Purpose**: Fundamental ECS usage patterns
- **Features**: Entity creation, component management, basic systems
- **Target Audience**: Developers learning ECS concepts

---

### 🎯 **Feature-Specific Examples**

#### **input_demo/**
- **Status**: 📋 PLANNED
- **Purpose**: Comprehensive input handling demonstration
- **Features**: Keyboard/mouse input, input combinations, event handling
- **Dependencies**: InputManager, EventManager

#### **collision_demo/**
- **Status**: 📋 PLANNED
- **Purpose**: Collision detection and response examples
- **Features**: AABB collision, collision layers, physics integration
- **Dependencies**: CollisionSystem, PhysicsSystem

#### **scene_management/**
- **Status**: 📋 PLANNED
- **Purpose**: Scene creation, transitions, and management
- **Features**: Multiple scenes, scene switching, state persistence
- **Dependencies**: SceneManager, EventManager

#### **sprite_rendering/**
- **Status**: 📋 PLANNED
- **Purpose**: 2D sprite rendering techniques
- **Features**: Sprite drawing, rotation, flipping, layering
- **Dependencies**: SpriteRenderer, ResourceManager

---

### 🎪 **Game Examples**

#### **top_down_shooter/**
- **Status**: 📋 PLANNED
- **Purpose**: Complete 2D top-down shooter game
- **Features**: Player movement, shooting, enemies, collision
- **Complexity**: Intermediate
- **Systems Used**: All core systems

#### **platformer_basic/**
- **Status**: 📋 PLANNED
- **Purpose**: Simple 2D platformer mechanics
- **Features**: Gravity, jumping, platform collision
- **Complexity**: Intermediate
- **Systems Used**: PhysicsSystem, CollisionSystem

#### **puzzle_game/**
- **Status**: 📋 PLANNED
- **Purpose**: Turn-based puzzle game mechanics
- **Features**: Grid-based movement, game state management
- **Complexity**: Beginner
- **Systems Used**: ECS, SceneManager

---

## Example Structure

Each example follows this standard structure:

```
example_name/
├── main.cpp              # Entry point
├── GameScene.hpp         # Main game scene
├── GameScene.cpp         # Scene implementation
├── README.md             # Example-specific documentation
├── assets/               # Example assets
│   ├── sprites/
│   ├── sounds/
│   └── data/
└── CMakeLists.txt        # Build configuration (if needed)
```

---

## How to Run Examples

### **Prerequisites**
- 2D Game Engine built and installed
- SDL3 and dependencies available
- C++20 compatible compiler

### **Building and Running**

```bash
# Navigate to project root
cd CS5850

# Build the engine
mkdir build && cd build
cmake ..
make

# Run specific example
cd examples/hello_world
mkdir build && cd build
cmake ..
make
./hello_world
```

---

## Example Development Guidelines

### **Code Standards**
- Follow engine coding conventions
- Include comprehensive comments
- Provide clear variable names
- Keep examples focused and minimal

### **Documentation Requirements**
- Each example must have a README.md
- Include learning objectives
- Explain key concepts demonstrated
- Provide expected output description

### **Asset Guidelines**
- Use placeholder assets or Creative Commons resources
- Keep asset sizes small for repository efficiency
- Include asset attribution where required

---

## Learning Path

### **Recommended Order for Beginners**

1. **hello_world** - Basic engine setup
2. **ecs_basics** - Understanding ECS concepts
3. **input_demo** - User input handling
4. **sprite_rendering** - Basic graphics
5. **collision_demo** - Game physics
6. **scene_management** - Application structure
7. **puzzle_game** - Simple complete game
8. **platformer_basic** - Physics-based gameplay
9. **top_down_shooter** - Complex game systems

### **Advanced Topics**
- Custom system development
- Performance optimization techniques
- Advanced rendering features
- Complex game state management

---

## Contributing Examples

### **Submission Guidelines**
- Examples should demonstrate specific engine features
- Include comprehensive documentation
- Test on multiple platforms if possible
- Follow the standard example structure

### **Example Ideas Needed**
- Audio system demonstration
- UI/Menu system examples
- Save/Load game state
- Particle system usage
- Animation system examples

---

## Current Development Priority

1. **hello_world** - Essential for new users
2. **ecs_basics** - Core engine concept
3. **top_down_shooter** - Demonstrates engine capabilities
4. **Feature-specific demos** - As systems are completed

---

## File Structure

```
src/
├── examples/
│   ├── README.md                    # This file
│   ├── hello_world/                 # Basic engine usage
│   ├── ecs_basics/                  # ECS fundamentals
│   ├── input_demo/                  # Input handling
│   ├── collision_demo/              # Collision detection
│   ├── scene_management/            # Scene system
│   ├── sprite_rendering/            # Graphics rendering
│   ├── top_down_shooter/            # Complete game example
│   ├── platformer_basic/            # Physics-based game
│   └── puzzle_game/                 # Turn-based game
```

---

## Summary

The examples directory serves as both a learning resource and a testing ground for engine features. Each example is designed to be educational, practical, and demonstrate real-world usage patterns of the 2D Game Engine.
