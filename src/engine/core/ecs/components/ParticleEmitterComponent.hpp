// src/engine/core/ecs/components/ParticleEmitterComponent.hpp

#pragma once

#include "engine/core/Types.hpp"
#include <SDL3/SDL.h>
#include <random>

namespace engine::ECS {

enum class EmitterShape {
    POINT,      // Emit from a single point
    CIRCLE,     // Emit from circle perimeter
    CONE        // Emit in a cone shape
};

struct ParticleEmitterComponent {
    // Emission properties
    bool isActive = true;               // Is emitter active
    float emissionRate = 10.0f;         // Particles per second
    float emissionAccumulator = 0.0f;   // Time accumulator for emission
    
    int maxParticles = 100;             // Maximum particles this emitter can have
    int activeParticles = 0;            // Current active particle count
    
    // Particle initial properties
    float particleLifetime = 1.0f;      // Lifetime of each particle
    float lifetimeVariance = 0.2f;      // Random variance in lifetime
    
    // Initial velocity
    Vector2 initialVelocity = {0, -100}; // Base velocity
    float velocityVariance = 20.0f;      // Random variance in velocity
    
    // Acceleration (gravity, etc)
    Vector2 acceleration = {0, 50};      // Acceleration applied to particles
    
    // Size
    float startSize = 4.0f;
    float startSizeVariance = 1.0f;
    float endSize = 0.0f;
    float endSizeVariance = 0.0f;
    
    // Color
    SDL_Color startColor = {255, 255, 255, 255};
    SDL_Color endColor = {255, 255, 255, 0};
    
    // Rotation
    float rotationSpeed = 0.0f;
    float rotationSpeedVariance = 0.0f;
    
    // Emission shape
    EmitterShape shape = EmitterShape::POINT;
    float shapeRadius = 0.0f;           // For CIRCLE shape
    float coneAngle = 45.0f;            // For CONE shape (in degrees)
    float coneDirection = 0.0f;         // Direction of cone (in degrees)
    
    // Texture for particles (optional)
    std::string particleTexture = "pixel.png";  // Default to pixel texture
    
    // One-shot vs continuous
    bool isOneShot = false;             // If true, emit all particles at once
    int burstCount = 50;                // Number of particles for one-shot
};

} // namespace engine::ECS