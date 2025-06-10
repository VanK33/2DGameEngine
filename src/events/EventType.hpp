#pragma once

namespace game {
namespace events {

/**
 * Enum representing all supported event types in the system.
 */
enum class EventType {
    KEY_DOWN,
    KEY_UP,
    MOUSE_CLICK,
    MOUSE_MOVE,    // use with caution
    SCENE_CHANGE,
    GAME_OVER,
    ENTITY_COLLISION,
    /**
     * Reserved for dynamically defined or script-driven events.
     * 
     * CUSTOM allows flexibility to support events that are not known at compile time,
     * such as those triggered by external scripts, plugins, or debugging tools.
     *
     * ⚠️ Use with caution:
     * - Event semantics must be conveyed through the payload.
     * - Overuse may reduce type safety and make event handling less predictable.
     * - Prefer defining explicit EventType enums when possible for clarity and tooling support.
     */
    CUSTOM,
    /*
    potential future implementation
    ASSET_LOADED,
    TIMER_EXPIRED,
    ANIMATION_FINISHED,
    UI_BUTTON_CLICKED,
    PLAYER_DIED
    */
};

} // namespace events
} // namespace game
