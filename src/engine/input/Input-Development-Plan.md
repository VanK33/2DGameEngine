# Input Manager Development Plan for 2D Game Engine

## Executive Summary

This document outlines the **essential features** needed for the Input Management system in our 2D game engine. The Input Manager serves as the bridge between SDL events and our engine's event system, providing a clean abstraction for handling user input across different game types.

**Focus: Create a flexible, event-driven input system that supports various game types while maintaining clean integration with the engine's event system.**

**Status: Phase 1 COMPLETED ✅, Phase 1.5 COMPLETED ✅, Phase 2 PLANNED**

---

## 🎯 Current State Analysis

### ✅ **What We Have (COMPLETED)**
- Basic SDL event handling and mapping
- Integration with engine's EventManager
- Key state tracking (down, held, up)
- Mouse position and button tracking
- Event publishing for basic input events
- Thread-safe event queuing
- Mouse button held state tracking
- Mouse delta movement tracking
- Combined key query support
- Convenient mouse position interfaces

### ❌ **Critical Gaps (Remaining Work)**
- No input mapping/rebinding system - **PLANNED**
- Limited support for different input behaviors per game type - **PLANNED**
- No input recording/playback for testing - **PLANNED**
- No gamepad/controller support - **PLANNED**

### ⚠️ **Nice-to-Have (Can Skip for Now)**
- Advanced input combinations (key sequences)
- Input action recording for replays
- Input buffering for fighting games
- Virtual controller support
- Touch input support
- Input visualization for debugging

---

## 🚀 Phase 1: Core Input System - COMPLETED ✅

### ✅ **1.1 Basic Input Handling - COMPLETED**
**Files Modified:**
- ✅ `InputManager.hpp/cpp`

**Features Implemented:**
```cpp
class InputManager {
public:
    void HandleEvent(const SDL_Event& event);
    void Update();
    
    bool IsKeyDown(SDL_Keycode key) const;
    bool IsKeyHeld(SDL_Keycode key) const;
    bool IsKeyUp(SDL_Keycode key) const;
    
    bool IsMouseButtonDown(Uint8 button) const;
    bool IsMouseButtonUp(Uint8 button) const;
    void GetMousePosition(int& x, int& y) const;

private:
    std::unordered_map<SDL_Keycode, bool> keyDown_;
    std::unordered_map<SDL_Keycode, bool> keyHeld_;
    std::unordered_map<SDL_Keycode, bool> keyUp_;
};
```

### ✅ **1.2 Event System Integration - COMPLETED**
**Features Implemented:**
```cpp
class InputManager {
public:
    void SetEventManager(engine::event::EventManager* eventManager);
    
private:
    void PublishKeyEvent(engine::event::EventType type, const SDL_Event& event);
    void PublishMouseButtonEvent(engine::event::EventType type, const SDL_Event& event);
    void PublishMouseMotionEvent(const SDL_Event& event);
    void PublishMouseWheelEvent(const SDL_Event& event);
};
```

## 🌟 Phase 1.5: Enhanced Input Capabilities - COMPLETED ✅

### ✅ **1.5.1 Extended Mouse Support**
**Features Implemented:**
```cpp
// Convenient data structures
struct MousePosition { 
    int x, y; 
    operator glm::vec2() const;
};

struct MouseDelta { 
    int dx, dy; 
    operator glm::vec2() const;
};

class InputManager {
public:
    // Enhanced mouse interfaces
    bool IsMouseButtonHeld(Uint8 button) const;
    MousePosition GetMousePosition() const;
    MouseDelta GetMouseDelta() const;
    void GetMouseDelta(int& dx, int& dy) const;

private:
    std::unordered_map<Uint8, bool> mouseButtonHeld_;
    int mouseDeltaX_ = 0, mouseDeltaY_ = 0;
};
```

### ✅ **1.5.2 Combined Input Queries**
**Features Implemented:**
```cpp
class InputManager {
public:
    // Key combinations
    bool IsAnyKeyDown(std::span<const SDL_Keycode> keys) const;
    bool IsAnyKeyHeld(std::span<const SDL_Keycode> keys) const;
    bool IsAnyKeyUp(std::span<const SDL_Keycode> keys) const;
    bool IsAllKeysDown(std::span<const SDL_Keycode> keys) const;
    bool IsAllKeysHeld(std::span<const SDL_Keycode> keys) const;

    // Mouse button combinations
    bool IsAnyMouseButtonDown(std::span<const Uint8> buttons) const;
    bool IsAnyMouseButtonHeld(std::span<const Uint8> buttons) const;
    bool IsAnyMouseButtonUp(std::span<const Uint8> buttons) const;
};
```

---

## 🎨 Phase 2: Input Configuration & Mapping (HIGH)

### 2.1 Input Action System - **MUST HAVE**
**Files to Create:**
- `InputAction.hpp`
- `InputActionMap.hpp`

**Features (Minimal):**
```cpp
enum class InputActionType {
    INSTANT,    // Triggered once on press
    CONTINUOUS, // Triggered while held
    RELEASE     // Triggered on release
};

struct InputAction {
    std::string name;
    InputActionType type;
    std::vector<SDL_Keycode> primaryBinding;
    std::vector<SDL_Keycode> alternateBinding;
    float deadZone = 0.1f;  // For analog inputs
};

class InputActionMap {
public:
    void MapAction(const std::string& name, const InputAction& action);
    void UnmapAction(const std::string& name);
    bool IsActionTriggered(const std::string& name) const;
    float GetActionValue(const std::string& name) const;  // For analog inputs
};
```

### 2.2 Input Device Abstraction - **MUST HAVE**
**Files to Create:**
- `InputDevice.hpp`
- `KeyboardDevice.hpp`
- `MouseDevice.hpp`

**Features (Minimal):**
```cpp
class InputDevice {
public:
    virtual ~InputDevice() = default;
    virtual void Update() = 0;
    virtual void HandleEvent(const SDL_Event& event) = 0;
    virtual bool IsActive() const = 0;
    virtual InputDeviceType GetType() const = 0;
    virtual void Configure(const InputDeviceConfig& config) = 0;
};

class KeyboardDevice : public InputDevice {
public:
    bool IsButtonPressed(SDL_Keycode key) const;
    bool IsButtonHeld(SDL_Keycode key) const;
    bool IsButtonReleased(SDL_Keycode key) const;
};

class MouseDevice : public InputDevice {
public:
    bool IsButtonPressed(uint8_t button) const;
    void GetPosition(int& x, int& y) const;
    void GetMotion(int& dx, int& dy) const;
};
```

### 2.3 Game-Specific Input Behaviors - **MEDIUM**
**Files to Create:**
- `InputBehavior.hpp`
- `InputProfile.hpp`

**Features (Minimal):**
```cpp
class InputProfile {
public:
    void ConfigureForTopDown();
    void ConfigureForPlatformer();
    void ConfigureCustom(const std::vector<InputAction>& actions);
    
    const InputActionMap& GetActionMap() const;
    void SetActionMap(const InputActionMap& map);
};
```

## ⚡ Phase 3: Advanced Features & Extensions (LOW)

### 3.1 Additional Input Devices - **LOW**
**Files to Create:**
- `GamepadDevice.hpp`
- `TouchDevice.hpp`

**Features (Minimal):**
```cpp
class GamepadDevice : public InputDevice {
public:
    bool Initialize(int deviceIndex);
    bool IsButtonPressed(GamepadButton button) const;
    float GetAxisValue(GamepadAxis axis) const;
};

class InputDeviceFactory {
public:
    static std::unique_ptr<InputDevice> CreateDevice(InputDeviceType type);
    static void RegisterCustomDevice(InputDeviceType type, 
                                   std::function<std::unique_ptr<InputDevice>()> creator);
};
```

### 3.2 Input Recording & Playback - **LOW**
**Files to Create:**
- `InputRecorder.hpp`

**Features (Minimal):**
```cpp
class InputRecorder {
public:
    void StartRecording();
    void StopRecording();
    void PlaybackRecording();
    void SaveRecording(const std::string& filename);
    void LoadRecording(const std::string& filename);
};
```

### 3.3 Input Debug Tools - **LOW**
**Files to Create:**
- `InputDebugger.hpp`

---

## 🧪 Testing Strategy

### Unit Tests
1. **Basic Input States**
   - Key states (down, held, up)
   - Mouse button states
   - Mouse position tracking

2. **Enhanced Features**
   - Mouse held state transitions
   - Mouse delta calculations
   - Combined key queries (Any/All)
   - Input device state management

3. **Event Integration**
   - Event publishing
   - Event subscription
   - Multi-frame state consistency

### Integration Tests
- Debug scene test suite
- Cross-component interaction tests
- Performance benchmarks

---

## 📊 Design Principles

### Engine Responsibilities
- Provide basic input query mechanisms
- Maintain input state management
- Publish standardized input events
- Support combined input queries

### Game Responsibilities
- Define action meanings
- Configure key bindings
- Implement game-specific input logic
- Handle input responses

### Engine Limitations
- No game-specific action names
- No action mapping system
- No preset game-type configurations