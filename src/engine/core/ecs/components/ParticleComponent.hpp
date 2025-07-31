// src/engine/core/ecs/components/ParticleComponent.hpp

#pragma once

#include "engine/core/Types.hpp"
#include <SDL3/SDL.h>

namespace engine::ECS {

struct ParticleComponent {
    // Particle properties
    Vector2 velocity;           // Current velocity
    Vector2 acceleration;       // Acceleration (gravity, forces)
    
    float lifetime;             // Total lifetime in seconds
    float age;                  // Current age in seconds
    
    float startSize;            // Initial size
    float endSize;              // Final size
    float currentSize;          // Current interpolated size
    
    SDL_Color startColor;       // Initial color
    SDL_Color endColor;         // Final color
    SDL_Color currentColor;     // Current interpolated color
    
    float rotation;             // Current rotation in radians
    float rotationSpeed;        // Rotation speed in radians/second
    
    bool isActive = true;       // Is this particle still alive
};

} // namespace engine::ECS