// src/examples/zombie_survivor/ecs/components/BoundaryComponent.hpp
#pragma once

namespace ZombieSurvivor::Component {

enum class BoundaryType {
    SCREEN_BOUNDS,    // Screen boundaries
    CUSTOM_RECT       // Custom rectangular area
};

struct BoundaryComponent {
    BoundaryType type = BoundaryType::SCREEN_BOUNDS;
    
    // Entity size (used for boundary calculation, assuming square)
    float entitySize = 64.0f;
    
    // Custom boundaries (used when type is CUSTOM_RECT)
    float minX = 0.0f;
    float maxX = 800.0f;
    float minY = 0.0f;
    float maxY = 600.0f;
    
    // Whether boundary constraint is enabled
    bool enabled = true;
};

} // namespace ZombieSurvivor::Component