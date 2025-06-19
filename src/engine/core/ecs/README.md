# ECS (Entity-Component-System) Architecture

## Overview
This ECS implementation provides a flexible, type-safe, and performant foundation for game object management. The design emphasizes **separation of concerns**, **type safety**, and **extensibility** while maintaining clean interfaces between engine and game logic.

## Core Components

### 🏗️ Architecture Design

#### **ComponentManager** (`ComponentManager.hpp`)
- **Type-Safe Component Storage**: Uses `std::type_index` for compile-time type safety
- **Template-Based Design**: Generic `ComponentStore<T>` for efficient component management
- **Memory Efficiency**: `std::unordered_map<EntityID, T>` for O(1) component access
- **Interface Abstraction**: `IComponentStore` base class enables polymorphic operations

**Key Features:**
- Automatic component store creation on first use
- Type-safe component retrieval and iteration
- Efficient entity-component mapping
- RAII-compliant memory management

#### **Entity** (`Entity.hpp`, `Entity.cpp`)
- **Identity Management**: Unique `EntityID` (uint32_t) for entity identification
- **Move Semantics**: Optimized for performance with move-only semantics
- **Debug Support**: Comprehensive `ToString()` method for debugging
- **Lifecycle Control**: Active/inactive state management

**Design Decisions:**
- Copy operations deleted to prevent accidental entity duplication
- Move semantics enabled for efficient entity transfer
- String-based naming for human-readable entity identification

#### **System** (`System.hpp`)
- **Abstract Base Class**: Pure virtual interface for system implementations
- **Lifecycle Management**: `Init()`, `Update()`, `Shutdown()` hooks
- **Enable/Disable Control**: Runtime system activation control
- **Naming Support**: System identification for debugging and profiling

**Philosophy:**
- Systems are **game-specific implementations** (not provided by engine)
- Engine provides the framework, games provide the logic
- Reduces coupling between engine and game code

#### **World** (`World.hpp`, `World.cpp`)
- **System Orchestration**: Manages system execution order and lifecycle
- **Shared Ownership**: Uses `std::shared_ptr<System>` for flexible system management
- **Update Loop Integration**: Centralized system update coordination
- **Clean Shutdown**: Proper resource cleanup on world destruction

#### **WorldState** (`WorldState.hpp`, `WorldState.cpp`)
- **Global State Management**: Pause/resume functionality
- **Scene Tracking**: Current scene name for context awareness
- **Frame Counting**: Frame-level timing and debugging support
- **Thread-Safe Design**: Simple state management for concurrent access

## 🎯 Design Philosophy

### **1. Separation of Concerns**
- **Components**: Pure data containers (no behavior)
- **Systems**: Pure behavior (no data storage)
- **Entities**: Identity and lifecycle management only
- **World**: Orchestration and coordination

### **2. Type Safety First**
- Compile-time type checking for all component operations
- Template-based design prevents runtime type errors
- Strong typing throughout the component management system

### **3. Performance Optimization**
- O(1) component access via hash maps
- Move semantics for efficient entity transfer
- Minimal virtual function overhead
- Cache-friendly data layout

### **4. Extensibility**
- Game-specific systems implemented outside engine
- Component types defined by game logic
- Pluggable system architecture
- No hard dependencies on specific game features

## 🔧 Usage Patterns

### Component Definition (Game-Side)
```cpp
struct TransformComponent {
    float x, y, z;
    float rotation;
    float scale;
};

struct SpriteComponent {
    std::string texturePath;
    SDL_Rect sourceRect;
};
```

### System Implementation (Game-Side)
```cpp
class RenderSystem : public engine::ECS::System {
public:
    void Update(float deltaTime) override {
        // Game-specific rendering logic
    }
    
    const char* GetName() const override {
        return "RenderSystem";
    }
};
```

### World Management
```cpp
auto world = std::make_unique<engine::ECS::World>();
world->AddSystem(std::make_shared<RenderSystem>());
world->Update(deltaTime);
```

## 🚀 Benefits

1. **Modularity**: Each subsystem is independent and testable
2. **Scalability**: Efficient component storage scales with entity count
3. **Maintainability**: Clear separation between engine and game code
4. **Performance**: Optimized data access patterns and memory layout
5. **Flexibility**: Easy to extend with new component types and systems

## 🔮 Future Considerations

- **Component Archetypes**: Predefined component combinations for common patterns
- **System Dependencies**: Dependency injection for system ordering
- **Parallel Processing**: Multi-threaded system execution
- **Serialization**: Component state persistence and loading
- **Event Integration**: ECS-aware event system integration