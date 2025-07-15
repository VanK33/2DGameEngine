// src/examples/zombie_survivor/ecs/RenderLayer.hpp

#pragma once

namespace ZombieSurvivor::ECS {

enum class RenderLayer {
    BACKGROUND = 0,      // Ground tiles, far background
    GROUND_DETAILS = 5,  // Decorations, rocks, grass  
    ENTITIES = 10,       // Players, enemies, projectiles
    WEAPON = 12,         // Weapons (render on top of entities)
    EFFECTS = 15,        // Visual effects, particles, explosions
    UI = 20             // User interface elements
};

// Utility functions
inline int ToInt(RenderLayer layer) {
    return static_cast<int>(layer);
}

inline RenderLayer FromInt(int value) {
    return static_cast<RenderLayer>(value);
}

} // namespace ZombieSurvivor::ECS