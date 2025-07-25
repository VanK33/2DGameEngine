#include "RenderSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/graphics/renderer/Renderer.hpp"
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace engine::ECS {

RenderSystem::RenderSystem(engine::graphics::SpriteRenderer* spriteRenderer, 
                           engine::resources::ResourceManager* resourceManager,
                           engine::graphics::Renderer* renderer)
    : spriteRenderer_(spriteRenderer)
    , resourceManager_(resourceManager)
    , renderer_(renderer)
    , renderedSpriteCount_(0) {
}

void RenderSystem::Init() {
    std::cout << "[RenderSystem] Initialized with layered rendering support" << std::endl;
}

void RenderSystem::Update(float deltaTime) {
    if (!world_ || !spriteRenderer_ || !resourceManager_) {
        return;
    }

    renderer_->BeginFrame();

    renderedSpriteCount_ = 0;
    
    // Collect all renderable sprites
    std::vector<RenderableSprite> renderables;
    CollectRenderableSprites(renderables);
    
    // Sort by renderLayer (higher values render later, displayed on top)
    std::sort(renderables.begin(), renderables.end());
    
    // Render all sprites in order
    for (const auto& renderable : renderables) {
        RenderSprite(renderable);
    }
    
    #ifdef DEBUG
    if (renderedSpriteCount_ > 0) {
        std::cout << "[RenderSystem] Rendered " << renderedSpriteCount_ << " sprites" << std::endl;
    }
    #endif

    renderer_->EndFrame(); 
}

void RenderSystem::Shutdown() {
    std::cout << "[RenderSystem] Shutdown" << std::endl;
    renderedSpriteCount_ = 0;
}

void RenderSystem::ResetStats() {
    renderedSpriteCount_ = 0;
}

void RenderSystem::CollectRenderableSprites(std::vector<RenderableSprite>& renderables) {
    auto& componentManager = world_->GetComponentManager();
    auto entities = componentManager.GetEntitiesWithComponents<Transform2D, Sprite2D>();
    
    renderables.reserve(entities.size());
    
    for (EntityID entityId : entities) {
        auto* transform = componentManager.GetComponent<Transform2D>(entityId);
        auto* sprite = componentManager.GetComponent<Sprite2D>(entityId);
        
        // Only collect visible sprites
        if (transform && sprite && sprite->visible) {
            renderables.push_back({entityId, transform, sprite});
        }
    }
}

void RenderSystem::RenderSprite(const RenderableSprite& renderable) {
    SDL_Texture* texture = resourceManager_->GetTexture(renderable.sprite->texturePath);
    if (!texture) {
        return;
    }
    
    // Calculate rendering parameters
    Transform2D* transform = renderable.transform;
    Sprite2D* sprite = renderable.sprite;
    
    // Determine sprite dimensions
    float spriteWidth, spriteHeight;
    if (sprite->sourceRect.w > 0 && sprite->sourceRect.h > 0) {
        // Use dimensions specified by sourceRect
        spriteWidth = sprite->sourceRect.w * transform->scaleX;
        spriteHeight = sprite->sourceRect.h * transform->scaleY;
    } else {
        // Use original texture dimensions
        float textureWidth, textureHeight;
        SDL_GetTextureSize(texture, &textureWidth, &textureHeight);
        spriteWidth = textureWidth * transform->scaleX;
        spriteHeight = textureHeight * transform->scaleY;
    }
    
    // Set color tinting
    SDL_SetTextureColorMod(texture, sprite->tint.r, sprite->tint.g, sprite->tint.b);
    SDL_SetTextureAlphaMod(texture, sprite->tint.a);
    
    // Calculate pivot point if specified
    SDL_FPoint* pivotPtr = nullptr;
    SDL_FPoint calculatedPivot;
    if (sprite->pivotOffset.x >= 0.0f && sprite->pivotOffset.y >= 0.0f) {
        // Convert relative pivot offset to absolute pixel coordinates
        calculatedPivot.x = spriteWidth * sprite->pivotOffset.x;
        calculatedPivot.y = spriteHeight * sprite->pivotOffset.y;
        pivotPtr = &calculatedPivot;
    }
    
    // Call SpriteRenderer for rendering
    spriteRenderer_->Draw(texture, 
                         transform->x, transform->y,
                         spriteWidth, spriteHeight,
                         transform->rotation,
                         SDL_FLIP_NONE,
                         pivotPtr);

    
    renderedSpriteCount_++;
}

} // namespace engine::ECS 