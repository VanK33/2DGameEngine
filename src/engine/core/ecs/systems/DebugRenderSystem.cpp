// src/engine/core/ecs/systems/DebugRenderSystem.cpp

#include "DebugRenderSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Sprite2D.hpp"
#include "engine/input/InputManager.hpp"
#include <iostream>
#include <sstream>

namespace engine::ECS {

DebugRenderSystem::DebugRenderSystem(SDL_Renderer* renderer, engine::input::InputManager* inputManager)
    : renderer_(renderer)
    , inputManager_(inputManager)
    , debugModeEnabled_(false)
    , f1KeyWasPressed_(false) {
    }

void DebugRenderSystem::Init() {
    std::cout << "[DebugRenderSystem] Initialized - Press F1 to toggle debug mode" << std::endl;
}

void DebugRenderSystem::Update(float deltaTime) {
    if (!renderer_ || !inputManager_ ) {
        return;
    }

    HandleInput();

    if (debugModeEnabled_) {
        RenderDebugInfo();
    }
}

void DebugRenderSystem::Shutdown() {
    std::cout << "[DebugRenderSystem] Shutdown" << std::endl;
    debugModeEnabled_ = false;
    f1KeyWasPressed_ = false;
}

void DebugRenderSystem::HandleInput() {
    bool f1KeyCurrentlyPressed = inputManager_->IsKeyDown(SDLK_F1);

    if (f1KeyCurrentlyPressed && !f1KeyWasPressed_) {
        debugModeEnabled_ = !debugModeEnabled_;
        std::cout << "[DebugRenderSystem] Debug mode " 
                  << (debugModeEnabled_ ? "ENABLED" : "DISABLED") << std::endl;
    }

    f1KeyWasPressed_ = f1KeyCurrentlyPressed;
}

void DebugRenderSystem::RenderDebugInfo() {
    RenderGrid(50);
    RenderMouseCoordinates();
    RenderEntityBoundingBoxes();
}

void DebugRenderSystem::RenderGrid(int gridSize) {
    SDL_SetRenderDrawColor(renderer_, 128, 128, 128, 100);

    int screenWidth, screenHeight;
    SDL_GetRenderOutputSize(renderer_, &screenWidth, &screenHeight);

    for (int x = 0; x < screenWidth; x += gridSize) {
        SDL_RenderLine(renderer_, x, 0, x, screenHeight);
    }

    for (int y = 0; y < screenHeight; y += gridSize) {
        SDL_RenderLine(renderer_, 0, y, screenWidth, y);
    }
}

void DebugRenderSystem::RenderMouseCoordinates() {
    auto mousePos = inputManager_->GetMousePosition();
    
    // Set color for mouse indicator (bright yellow)
    SDL_SetRenderDrawColor(renderer_, 255, 255, 0, 255);
    
    // Draw crosshair at mouse position
    int size = 10;
    SDL_RenderLine(renderer_, mousePos.x - size, mousePos.y, mousePos.x + size, mousePos.y);
    SDL_RenderLine(renderer_, mousePos.x, mousePos.y - size, mousePos.x, mousePos.y + size);
    
    // TODO: Add text rendering for coordinates (requires font system)
    // For now, keep console output for coordinates
    static int frameCounter = 0;
    if (frameCounter % 30 == 0) {
        std::cout << "[DEBUG] Mouse: (" << mousePos.x << ", " << mousePos.y << ")" << std::endl;
    }
    frameCounter++;
}

void DebugRenderSystem::RenderEntityBoundingBoxes() {
    if (!world_) return;
    
    auto& componentManager = world_->GetComponentManager();
    auto entities = componentManager.GetEntitiesWithComponent<Transform2D>();
    
    for (auto entity : entities) {
        auto transform = componentManager.GetComponent<Transform2D>(entity);
        auto sprite = componentManager.GetComponent<Sprite2D>(entity);
        
        if (transform) {
            // Set entity bounding box color (green)
            SDL_SetRenderDrawColor(renderer_, 0, 255, 0, 255);
            
            // Calculate entity bounds
            float width = 32.0f * transform->scaleX;  // Default size
            float height = 32.0f * transform->scaleY;
            
            if (sprite && sprite->sourceRect.w > 0 && sprite->sourceRect.h > 0) {
                width = sprite->sourceRect.w * transform->scaleX;
                height = sprite->sourceRect.h * transform->scaleY;
            }
            
            // Draw bounding box
            SDL_FRect rect = {
                transform->x - width/2,
                transform->y - height/2,
                width,
                height
            };
            SDL_RenderRect(renderer_, &rect);
            
            // Render entity info
            RenderEntityInfo(entity, transform, sprite);
        }
    }
}

void DebugRenderSystem::RenderEntityInfo(EntityID entity, const Transform2D* transform, const Sprite2D* sprite) {
    // Set info color (white)
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
    
    // Draw a small dot at entity center
    SDL_FRect centerDot = {
        transform->x - 2,
        transform->y - 2,
        4, 4
    };
    SDL_RenderFillRect(renderer_, &centerDot);
    
    // TODO: Add text rendering for entity ID, coordinates, layer info
    // For now, log to console periodically
    static int logCounter = 0;
    if (logCounter % 120 == 0) {  // Every 2 seconds at 60fps
        std::cout << "[DEBUG] Entity " << entity 
                  << " at (" << transform->x << ", " << transform->y << ")";
        if (sprite) {
            std::cout << " layer: " << sprite->renderLayer;
        }
        std::cout << std::endl;
    }
    logCounter++;
}

} // namespace engine::ECS