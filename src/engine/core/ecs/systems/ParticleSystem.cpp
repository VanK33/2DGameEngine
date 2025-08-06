// src/engine/core/ecs/systems/ParticleSystem.cpp

#include "ParticleSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "engine/core/ecs/components/Sprite2D.hpp"
#include "engine/core/ecs/components/ParticleComponent.hpp"
#include "engine/core/ecs/components/ParticleEmitterComponent.hpp"
#include <iostream>
#include <cmath>

namespace engine::ECS {

ParticleSystem::ParticleSystem() : randomEngine_(std::random_device{}()) {
}

void ParticleSystem::Init() {
    std::cout << "[ParticleSystem] Initialized" << std::endl;
}

void ParticleSystem::Update(float deltaTime) {
    UpdateEmitters(deltaTime);
    UpdateParticles(deltaTime);
    CleanupDeadParticles();
}

void ParticleSystem::Shutdown() {
    std::cout << "[ParticleSystem] Shutdown" << std::endl;
}

void ParticleSystem::UpdateEmitters(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto emitters = componentManager.GetEntitiesWithComponent<ParticleEmitterComponent>();
    
    for (auto emitterId : emitters) {
        auto* emitter = componentManager.GetComponent<ParticleEmitterComponent>(emitterId);
        if (!emitter || !emitter->isActive) continue;
        
        EmitParticles(emitterId, deltaTime);
    }
}

void ParticleSystem::UpdateParticles(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto particles = componentManager.GetEntitiesWithComponent<ParticleComponent>();
    
    for (auto particleId : particles) {
        auto* particle = componentManager.GetComponent<ParticleComponent>(particleId);
        auto* transform = componentManager.GetComponent<Transform2D>(particleId);
        auto* velocity = componentManager.GetComponent<Velocity2D>(particleId);
        auto* sprite = componentManager.GetComponent<Sprite2D>(particleId);
        
        if (!particle || !transform) continue;
        
        // Update age
        particle->age += deltaTime;
        
        // Check if particle should die
        if (particle->age >= particle->lifetime) {
            particle->isActive = false;
            particlesToRemove_.push_back(particleId);
            continue;
        }
        
        // Calculate life progress (0 to 1)
        float lifeProgress = particle->age / particle->lifetime;
        
        // Update velocity with acceleration
        if (velocity) {
            velocity->vx += particle->acceleration.x * deltaTime;
            velocity->vy += particle->acceleration.y * deltaTime;
        }
        
        // Update rotation
        transform->rotation += particle->rotationSpeed * deltaTime;
        
        // Interpolate size
        particle->currentSize = particle->startSize + 
            (particle->endSize - particle->startSize) * lifeProgress;
        // Use current size directly as scale factor
        transform->scaleX = particle->currentSize;
        transform->scaleY = particle->currentSize;
        
        // Interpolate color
        if (sprite) {
            particle->currentColor = InterpolateColor(
                particle->startColor, 
                particle->endColor, 
                lifeProgress
            );
            sprite->tint = particle->currentColor;
        }
    }
}

void ParticleSystem::EmitParticles(EntityID emitterId, float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* emitter = componentManager.GetComponent<ParticleEmitterComponent>(emitterId);
    auto* emitterTransform = componentManager.GetComponent<Transform2D>(emitterId);
    
    if (!emitter || !emitterTransform) return;
    
    if (emitter->isOneShot) {
        // Emit all particles at once
        if (emitter->activeParticles == 0) {
            for (int i = 0; i < emitter->burstCount && emitter->activeParticles < emitter->maxParticles; ++i) {
                CreateParticle(emitterId, {emitterTransform->x, emitterTransform->y});
                emitter->activeParticles++;
            }
            emitter->isActive = false; // Deactivate after burst
        }
    } else {
        // Continuous emission
        emitter->emissionAccumulator += deltaTime;
        float particlesToEmit = emitter->emissionRate * emitter->emissionAccumulator;
        
        while (particlesToEmit >= 1.0f && emitter->activeParticles < emitter->maxParticles) {
            CreateParticle(emitterId, {emitterTransform->x, emitterTransform->y});
            emitter->activeParticles++;
            particlesToEmit -= 1.0f;
            emitter->emissionAccumulator -= 1.0f / emitter->emissionRate;
        }
    }
}

EntityID ParticleSystem::CreateParticle(EntityID emitterId, const Vector2& position) {
    auto* world = GetWorld();
    if (!world) return 0;
    
    auto& entityFactory = world->GetEntityFactory();
    auto& componentManager = world->GetComponentManager();
    
    auto* emitter = componentManager.GetComponent<ParticleEmitterComponent>(emitterId);
    if (!emitter) return 0;
    
    // Create particle entity
    EntityID particleId = entityFactory.CreateEntity();
    
    // Add Transform2D
    Transform2D transform;
    transform.x = position.x;
    transform.y = position.y;
    transform.rotation = 0.0f;
    transform.scaleX = 1.0f;
    transform.scaleY = 1.0f;
    componentManager.AddComponent<Transform2D>(particleId, transform);
    
    // Add ParticleComponent
    ParticleComponent particle;
    particle.lifetime = emitter->particleLifetime + 
        GetRandomFloat(-emitter->lifetimeVariance, emitter->lifetimeVariance);
    particle.age = 0.0f;
    
    // Set velocity based on emitter shape
    Vector2 baseVelocity = emitter->initialVelocity;
    if (emitter->shape == EmitterShape::CONE) {
        float angleRad = (emitter->coneDirection - emitter->coneAngle/2.0f + 
                         GetRandomFloat(0, emitter->coneAngle)) * M_PI / 180.0f;
        float speed = std::sqrt(baseVelocity.x * baseVelocity.x + baseVelocity.y * baseVelocity.y);
        baseVelocity.x = std::cos(angleRad) * speed;
        baseVelocity.y = std::sin(angleRad) * speed;
    }
    
    particle.velocity = GetRandomVector(baseVelocity, emitter->velocityVariance);
    particle.acceleration = emitter->acceleration;
    
    particle.startSize = emitter->startSize + 
        GetRandomFloat(-emitter->startSizeVariance, emitter->startSizeVariance);
    particle.endSize = emitter->endSize + 
        GetRandomFloat(-emitter->endSizeVariance, emitter->endSizeVariance);
    particle.currentSize = particle.startSize;
    
    particle.startColor = emitter->startColor;
    particle.endColor = emitter->endColor;
    particle.currentColor = particle.startColor;
    
    particle.rotationSpeed = emitter->rotationSpeed + 
        GetRandomFloat(-emitter->rotationSpeedVariance, emitter->rotationSpeedVariance);
    
    componentManager.AddComponent<ParticleComponent>(particleId, particle);
    
    // Add Velocity2D
    Velocity2D velocity;
    velocity.vx = particle.velocity.x;
    velocity.vy = particle.velocity.y;
    velocity.maxSpeed = 1000.0f; // High max speed for particles
    componentManager.AddComponent<Velocity2D>(particleId, velocity);
    
    // Add Sprite2D
    Sprite2D sprite;
    sprite.texturePath = emitter->particleTexture;
    sprite.sourceRect = {0, 0, 0, 0}; // Use full texture (0,0,0,0 means use entire texture)
    sprite.visible = true;
    sprite.tint = particle.currentColor;
    sprite.renderLayer = 15; // Above entities, below UI
    sprite.pivotOffset = {0.5f, 0.5f}; // Center the particle
    componentManager.AddComponent<Sprite2D>(particleId, sprite);
    
    std::cout << "[ParticleSystem] Created particle " << particleId 
              << " at (" << position.x << ", " << position.y 
              << ") with lifetime " << particle.lifetime << "s" 
              << ", size " << particle.startSize << ", texture: " << sprite.texturePath << std::endl;
    
    return particleId;
}

void ParticleSystem::CleanupDeadParticles() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& entityFactory = world->GetEntityFactory();
    auto& componentManager = world->GetComponentManager();
    
    for (auto particleId : particlesToRemove_) {
        // Update emitter's active particle count
        auto particles = componentManager.GetEntitiesWithComponent<ParticleEmitterComponent>();
        for (auto emitterId : particles) {
            auto* emitter = componentManager.GetComponent<ParticleEmitterComponent>(emitterId);
            if (emitter && emitter->activeParticles > 0) {
                emitter->activeParticles--;
            }
        }
        
        // Remove all components and destroy entity
        componentManager.RemoveAllComponents(particleId);
        entityFactory.DestroyEntity(particleId);
    }
    
    particlesToRemove_.clear();
}

void ParticleSystem::CreateParticleBurst(const Vector2& position, int count, 
                                       const SDL_Color& color, float speed) {
    std::cout << "[ParticleSystem] Creating particle burst: " << count 
              << " particles at (" << position.x << ", " << position.y << ")" << std::endl;
              
    auto* world = GetWorld();
    if (!world) {
        std::cout << "[ParticleSystem] ERROR: No world available for particle burst" << std::endl;
        return;
    }
    
    auto& entityFactory = world->GetEntityFactory();
    auto& componentManager = world->GetComponentManager();
    
    // Create temporary emitter for burst
    EntityID emitterId = entityFactory.CreateEntity();
    
    Transform2D transform;
    transform.x = position.x;
    transform.y = position.y;
    componentManager.AddComponent<Transform2D>(emitterId, transform);
    
    ParticleEmitterComponent emitter;
    emitter.isOneShot = true;
    emitter.burstCount = count;
    emitter.maxParticles = count;
    emitter.shape = EmitterShape::CONE;
    emitter.coneAngle = 360.0f; // Full circle
    emitter.initialVelocity = {0, -speed};
    emitter.velocityVariance = speed * 0.5f;
    emitter.particleLifetime = 0.5f;
    emitter.lifetimeVariance = 0.1f;
    emitter.startColor = color;
    emitter.endColor = {color.r, color.g, color.b, 0}; // Fade out
    emitter.startSize = 0.5f;
    emitter.endSize = 0.05f;
    emitter.acceleration = {0, 100}; // Gravity
    
    componentManager.AddComponent<ParticleEmitterComponent>(emitterId, emitter);
}

float ParticleSystem::GetRandomFloat(float min, float max) {
    return min + (max - min) * randomDist_(randomEngine_);
}

Vector2 ParticleSystem::GetRandomVector(const Vector2& base, float variance) {
    return {
        base.x + GetRandomFloat(-variance, variance),
        base.y + GetRandomFloat(-variance, variance)
    };
}

SDL_Color ParticleSystem::InterpolateColor(const SDL_Color& start, const SDL_Color& end, float t) {
    return {
        static_cast<Uint8>(start.r + (end.r - start.r) * t),
        static_cast<Uint8>(start.g + (end.g - start.g) * t),
        static_cast<Uint8>(start.b + (end.b - start.b) * t),
        static_cast<Uint8>(start.a + (end.a - start.a) * t)
    };
}

} // namespace engine::ECS