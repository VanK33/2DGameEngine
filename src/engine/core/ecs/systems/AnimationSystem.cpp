// src/engine/core/ecs/systems/AnimationSystem.cpp

#include "AnimationSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/SpriteAnimation.hpp"
#include "engine/core/ecs/components/AnimationState.hpp"
#include "engine/core/ecs/components/Sprite2D.hpp"
#include <SDL3/SDL.h>

namespace engine::ECS {

AnimationSystem::AnimationSystem(engine::resources::ResourceManager* resourceManager)
    : resourceManager_(resourceManager) {
}

void AnimationSystem::Update(float deltaTime) {
    auto* world = GetWorld();
    if (!world) {
        return;
    }

    auto& componentManager = world->GetComponentManager();
    
    // Get all entities that have animation components
    auto entities = componentManager.GetEntitiesWithComponents<SpriteAnimation, AnimationState, Sprite2D>();
    
    for (EntityID entityId : entities) {
        auto* animation = componentManager.GetComponent<SpriteAnimation>(entityId);
        auto* state = componentManager.GetComponent<AnimationState>(entityId);
        auto* sprite = componentManager.GetComponent<Sprite2D>(entityId);
        
        if (!animation || !state || !sprite) {
            continue;
        }
        
        // Skip if animation is not playing or auto-play is disabled and not started
        if (!state->isPlaying) {
            continue;
        }
        
        // Skip if animation has completed and doesn't loop
        if (state->hasCompleted && !animation->loop) {
            continue;
        }
        
        // Update elapsed time
        state->elapsedTime += deltaTime;
        
        // Check if it's time to advance to the next frame
        if (state->elapsedTime >= animation->frameDuration) {
            state->elapsedTime -= animation->frameDuration;
            state->currentFrame++;
            
            // Handle looping or completion
            if (state->currentFrame >= animation->frameCount) {
                if (animation->loop) {
                    state->currentFrame = 0;
                    state->loopCount++;
                } else {
                    state->currentFrame = animation->frameCount - 1;
                    state->hasCompleted = true;
                    state->isPlaying = false;
                }
            }
            
            // Update the sprite's sourceRect
            UpdateAnimationFrame(entityId);
        }
    }
}

void AnimationSystem::UpdateAnimationFrame(EntityID entityId) {
    auto* world = GetWorld();
    if (!world) {
        return;
    }
    
    auto& componentManager = world->GetComponentManager();
    
    auto* animation = componentManager.GetComponent<SpriteAnimation>(entityId);
    auto* state = componentManager.GetComponent<AnimationState>(entityId);
    auto* sprite = componentManager.GetComponent<Sprite2D>(entityId);
    
    if (!animation || !state || !sprite) {
        return;
    }
    
    // Calculate frame dimensions if not explicitly set
    int frameWidth = animation->frameWidth;
    int frameHeight = animation->frameHeight;
    
    if (frameWidth == 0 || frameHeight == 0) {
        CalculateFrameDimensions(sprite->texturePath, animation->framesPerRow, frameWidth, frameHeight);
    }
    
    // Calculate source rectangle for current frame
    int currentFrame = state->currentFrame;
    int framesPerRow = animation->framesPerRow;
    
    int frameX = (currentFrame % framesPerRow) * frameWidth;
    int frameY = (currentFrame / framesPerRow) * frameHeight;
    
    // Update sprite's source rectangle
    sprite->sourceRect.x = frameX;
    sprite->sourceRect.y = frameY;
    sprite->sourceRect.w = frameWidth;
    sprite->sourceRect.h = frameHeight;
    
    // Debug output for sprite cutting
    static int debugCount = 0;
    if (debugCount < 10) { // Only show first 10 updates
        std::cout << "*** [AnimationSystem] Entity " << entityId 
                  << " Frame " << currentFrame << ": sourceRect(" 
                  << frameX << "," << frameY << "," << frameWidth << "," << frameHeight 
                  << ") ***" << std::endl;
        debugCount++;
    }
}

void AnimationSystem::CalculateFrameDimensions(const std::string& texturePath, int framesPerRow, int& frameWidth, int& frameHeight) {
    if (!resourceManager_) {
        frameWidth = frameHeight = 64; // Default fallback
        return;
    }
    
    SDL_Texture* texture = resourceManager_->GetTexture(texturePath);
    if (!texture) {
        // Try loading the texture
        texture = resourceManager_->LoadTexture(texturePath);
    }
    
    if (texture) {
        float textureWidth, textureHeight;
        if (SDL_GetTextureSize(texture, &textureWidth, &textureHeight)) {
            frameWidth = static_cast<int>(textureWidth) / framesPerRow;
            frameHeight = static_cast<int>(textureHeight); // Assume single row for now
        } else {
            frameWidth = frameHeight = 64; // Default fallback
        }
    } else {
        frameWidth = frameHeight = 64; // Default fallback
    }
}

} // namespace engine::ECS