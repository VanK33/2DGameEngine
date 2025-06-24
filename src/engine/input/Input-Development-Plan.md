# Input Manager Development Plan for 2D Game Engine

## Executive Summary

This document outlines the **essential features** needed for the Input Management system in our 2D game engine. The Input Manager serves as the bridge between SDL events and our engine's event system, providing a clean abstraction for handling user input across different game types.

**Focus: Create a flexible, event-driven input system that supports various game types while maintaining clean integration with the engine's event system.**

**Status: Phase 1 COMPLETED ✅, Phase 2 PLANNED**

---

## 🎯 Current State Analysis

### ✅ **What We Have (COMPLETED)**
- Basic SDL event handling and mapping
- Integration with engine's EventManager
- Key state tracking (down, held, up)
- Mouse position and button tracking
- Event publishing for basic input events
- Thread-safe event queuing

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

### 2.2 Input Device Abstraction - **MUST HAVE** (New)
**Files to Create:**
- `InputDevice.hpp`
- `KeyboardDevice.hpp`
- `MouseDevice.hpp`

**Features (Minimal):**
```cpp
// 输入设备抽象基类
class InputDevice {
public:
    virtual ~InputDevice() = default;
    
    // 基础接口
    virtual void Update() = 0;
    virtual void HandleEvent(const SDL_Event& event) = 0;
    virtual bool IsActive() const = 0;
    
    // 设备类型识别
    virtual InputDeviceType GetType() const = 0;
    
    // 扩展接口
    virtual void Configure(const InputDeviceConfig& config) = 0;
};

// 键盘设备实现
class KeyboardDevice : public InputDevice {
public:
    bool IsButtonPressed(SDL_Keycode key) const;
    bool IsButtonHeld(SDL_Keycode key) const;
    bool IsButtonReleased(SDL_Keycode key) const;
    
    // 实现基类接口
    void Update() override;
    void HandleEvent(const SDL_Event& event) override;
    InputDeviceType GetType() const override { return InputDeviceType::Keyboard; }
};

// 鼠标设备实现
class MouseDevice : public InputDevice {
public:
    bool IsButtonPressed(uint8_t button) const;
    void GetPosition(int& x, int& y) const;
    void GetMotion(int& dx, int& dy) const;
    
    // 实现基类接口
    void Update() override;
    void HandleEvent(const SDL_Event& event) override;
    InputDeviceType GetType() const override { return InputDeviceType::Mouse; }
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
// Gamepad 设备实现
class GamepadDevice : public InputDevice {
public:
    bool Initialize(int deviceIndex);
    bool IsButtonPressed(GamepadButton button) const;
    float GetAxisValue(GamepadAxis axis) const;
    
    // 实现基类接口
    void Update() override;
    void HandleEvent(const SDL_Event& event) override;
    InputDeviceType GetType() const override { return InputDeviceType::Gamepad; }
};

// 输入设备工厂
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