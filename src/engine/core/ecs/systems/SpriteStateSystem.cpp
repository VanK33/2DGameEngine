// src/engine/core/ecs/systems/SpriteStateSystem.cpp

#include "SpriteStateSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/SpriteStateComponent.hpp"
#include "engine/core/ecs/components/Sprite2D.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "engine/core/ecs/components/AnimationState.hpp"
#include "examples/zombie_survivor/ecs/components/AimingComponent.hpp"
#include <cmath>
#include <iostream>

namespace engine::ECS {

SpriteStateSystem::SpriteStateSystem(engine::resources::ResourceManager* resourceManager)
    : resourceManager_(resourceManager) {
}

void SpriteStateSystem::Update(float deltaTime) {
    auto* world = GetWorld();
    if (!world) {
        return;
    }

    auto& componentManager = world->GetComponentManager();
    
    // Get all entities with sprite state component
    auto entities = componentManager.GetEntitiesWithComponents<SpriteStateComponent, Sprite2D>();
    
    for (EntityID entityId : entities) {
        auto* spriteState = componentManager.GetComponent<SpriteStateComponent>(entityId);
        auto* velocity = componentManager.GetComponent<Velocity2D>(entityId);
        
        if (!spriteState) continue;
        
        // Prioritize aiming direction over movement direction for better gameplay
        auto* aimingComponent = componentManager.GetComponent<ZombieSurvivor::Component::AimingComponent>(entityId);
        
        SpriteStateComponent::Direction newDirection;
        static int directionDebugCount = 0;
        
        if (aimingComponent) {
            // Use aiming direction for entities that can aim (like players)
            newDirection = CalculateDirection(aimingComponent->aimDirection.x, aimingComponent->aimDirection.y);
            
            if (directionDebugCount < 10) {
                std::cout << "*** [SpriteStateSystem] Entity " << entityId 
                          << " using AIMING direction (" << aimingComponent->aimDirection.x 
                          << "," << aimingComponent->aimDirection.y << ") ***" << std::endl;
                directionDebugCount++;
            }
        } else if (velocity) {
            // Fallback to movement direction for entities without aiming (like zombies)
            newDirection = CalculateDirection(velocity->vx, velocity->vy);
        } else {
            // No direction info available, keep current direction
            newDirection = spriteState->currentDirection;
        }
        
        // Update state based on movement (still use velocity for walking/idle state)
        SpriteStateComponent::State newState = SpriteStateComponent::State::IDLE;
        if (velocity && (std::abs(velocity->vx) > MOVEMENT_THRESHOLD || std::abs(velocity->vy) > MOVEMENT_THRESHOLD)) {
            newState = SpriteStateComponent::State::WALKING;
        }
        
        // Only update if direction or state changed
        if (newDirection != spriteState->currentDirection || newState != spriteState->currentState) {
            spriteState->currentDirection = newDirection;
            spriteState->currentState = newState;
        }
        
        // Update sprite if state changed
        if (spriteState->HasStateChanged()) {
            UpdateEntitySprite(entityId);
            ResetAnimation(entityId);
            spriteState->UpdatePreviousState();
        }
    }
}

SpriteStateComponent::Direction SpriteStateSystem::CalculateDirection(float vx, float vy) const {
    // If not moving enough, keep current direction
    if (std::abs(vx) < MOVEMENT_THRESHOLD && std::abs(vy) < MOVEMENT_THRESHOLD) {
        return SpriteStateComponent::Direction::DOWN; // Default idle direction
    }
    
    // Calculate angle in degrees
    float angle = std::atan2(vy, vx) * 180.0f / M_PI;
    if (angle < 0) angle += 360.0f;
    
    // Debug output (only first few times)
    static int debugCount = 0;
    if (debugCount < 5 && (std::abs(vx) > MOVEMENT_THRESHOLD || std::abs(vy) > MOVEMENT_THRESHOLD)) {
        std::cout << "*** [SpriteStateSystem] Direction(" << vx << "," << vy 
                  << ") -> Angle(" << angle << "°) ***" << std::endl;
        debugCount++;
    }
    
    // Convert angle to 8-directional movement
    // 0° = RIGHT, 90° = DOWN, 180° = LEFT, 270° = UP
    if (angle >= 337.5f || angle < 22.5f) {
        return SpriteStateComponent::Direction::RIGHT;
    } else if (angle >= 22.5f && angle < 67.5f) {
        return SpriteStateComponent::Direction::RIGHT_DOWN;
    } else if (angle >= 67.5f && angle < 112.5f) {
        return SpriteStateComponent::Direction::DOWN;
    } else if (angle >= 112.5f && angle < 157.5f) {
        return SpriteStateComponent::Direction::LEFT_DOWN;
    } else if (angle >= 157.5f && angle < 202.5f) {
        return SpriteStateComponent::Direction::LEFT;
    } else if (angle >= 202.5f && angle < 247.5f) {
        return SpriteStateComponent::Direction::LEFT_UP;
    } else if (angle >= 247.5f && angle < 292.5f) {
        return SpriteStateComponent::Direction::UP;
    } else { // 292.5f to 337.5f
        return SpriteStateComponent::Direction::RIGHT_UP;
    }
}

void SpriteStateSystem::UpdateEntitySprite(EntityID entityId) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* spriteState = componentManager.GetComponent<SpriteStateComponent>(entityId);
    auto* sprite = componentManager.GetComponent<Sprite2D>(entityId);
    
    if (!spriteState || !sprite) return;
    
    // Get the appropriate sprite for current state
    std::string newSpritePath = spriteState->GetCurrentSprite();
    
    // Only update if sprite path changed
    if (sprite->texturePath != newSpritePath) {
        sprite->texturePath = newSpritePath;
        
        // Preload the new texture
        if (resourceManager_) {
            resourceManager_->LoadTexture(newSpritePath);
        }
        
        std::cout << "[SpriteStateSystem] Entity " << entityId 
                  << " switched to sprite: " << newSpritePath << std::endl;
    }
}

void SpriteStateSystem::ResetAnimation(EntityID entityId) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto* animState = componentManager.GetComponent<AnimationState>(entityId);
    
    if (animState) {
        // Reset animation to first frame
        animState->currentFrame = 0;
        animState->elapsedTime = 0.0f;
        animState->hasCompleted = false;
        animState->isPlaying = true;
        animState->loopCount = 0;
    }
}

} // namespace engine::ECS