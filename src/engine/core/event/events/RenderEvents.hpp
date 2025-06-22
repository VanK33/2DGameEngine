#pragma once

#include "EventType.hpp"
#include <SDL3/SDL.h>
#include <string>

namespace engine::event {

// Extend EventType Enum
enum class RenderEventType {
    SPRITE_CREATED,
    SPRITE_DESTROYED,
    SPRITE_VISIBILITY_CHANGED,
    CAMERA_MOVED,
    TEXTURE_LOADED
};

// Event Data Structures
struct SpriteCreatedData {
    uint32_t entityId;  // Corresponds to ECS EntityID
    std::string texturePath;
    SDL_FRect bounds;
    bool visible;
};

struct SpriteDestroyedData {
    uint32_t entityId;
    std::string texturePath;
};

struct SpriteVisibilityData {
    uint32_t entityId;
    bool visible;
    std::string reason;  // "user_action", "game_logic", etc.
};

struct CameraMovedData {
    float oldX, oldY;
    float newX, newY;
    float deltaX, deltaY;
};

struct TextureLoadedData {
    std::string texturePath;
    bool success;
    std::string errorMessage;  // If loading failed
};

} // namespace engine::event 