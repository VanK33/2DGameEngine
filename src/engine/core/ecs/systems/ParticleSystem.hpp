// src/engine/core/ecs/systems/ParticleSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/Types.hpp"
#include <SDL3/SDL.h>
#include <random>
#include <vector>

namespace engine::ECS {

class ParticleSystem : public System {
public:
    ParticleSystem();
    ~ParticleSystem() override = default;
    
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    
    const char* GetName() const override { return "ParticleSystem"; }
    
    // Create a burst of particles at a position
    void CreateParticleBurst(const Vector2& position, int count, 
                           const SDL_Color& color, float speed = 100.0f);
    
private:
    // Update all particle emitters
    void UpdateEmitters(float deltaTime);
    
    // Update individual particles
    void UpdateParticles(float deltaTime);
    
    // Emit new particles from an emitter
    void EmitParticles(EntityID emitterId, float deltaTime);
    
    // Create a single particle
    EntityID CreateParticle(EntityID emitterId, const Vector2& position);
    
    // Clean up dead particles
    void CleanupDeadParticles();
    
    // Helper to get random float in range
    float GetRandomFloat(float min, float max);
    
    // Helper to get random vector with variance
    Vector2 GetRandomVector(const Vector2& base, float variance);
    
    // Interpolate between colors
    SDL_Color InterpolateColor(const SDL_Color& start, const SDL_Color& end, float t);
    
private:
    std::mt19937 randomEngine_;
    std::uniform_real_distribution<float> randomDist_{0.0f, 1.0f};
    
    // Track particles for cleanup
    std::vector<EntityID> particlesToRemove_;
};

} // namespace engine::ECS