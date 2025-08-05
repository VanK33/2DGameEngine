// src/engine/core/ecs/components/SpriteStateComponent.hpp

#pragma once
#include <string>
#include <unordered_map>

namespace engine::ECS {

struct SpriteStateComponent {
    // Movement directions (8-directional)
    enum class Direction : uint8_t {
        DOWN = 0,           // Default/idle direction
        UP,
        LEFT,
        RIGHT,
        LEFT_UP,
        LEFT_DOWN,
        RIGHT_UP,
        RIGHT_DOWN
    };
    
    // Entity states
    enum class State : uint8_t {
        IDLE = 0,           // Not moving
        WALKING,            // Moving
        ATTACKING,          // Attack animation
        HURT,               // Taking damage
        DEAD                // Death animation
    };
    
    // Current state
    Direction currentDirection = Direction::DOWN;
    State currentState = State::IDLE;
    
    // Previous state for change detection
    Direction previousDirection = Direction::DOWN;
    State previousState = State::IDLE;
    
    // Sprite mappings for different direction/state combinations
    // Key: (direction << 8) | state, Value: texture path
    std::unordered_map<uint16_t, std::string> spriteMap;
    
    // Fallback sprite when specific combination not found
    std::string fallbackSprite = "pixel.png";
    
    // Helper functions
    static uint16_t MakeKey(Direction dir, State state) {
        return (static_cast<uint16_t>(dir) << 8) | static_cast<uint16_t>(state);
    }
    
    bool HasStateChanged() const {
        return currentDirection != previousDirection || currentState != previousState;
    }
    
    void UpdatePreviousState() {
        previousDirection = currentDirection;
        previousState = currentState;
    }
    
    std::string GetCurrentSprite() const {
        uint16_t key = MakeKey(currentDirection, currentState);
        auto it = spriteMap.find(key);
        return (it != spriteMap.end()) ? it->second : fallbackSprite;
    }
    
    void AddSpriteMapping(Direction dir, State state, const std::string& spritePath) {
        spriteMap[MakeKey(dir, state)] = spritePath;
    }
};

} // namespace engine::ECS