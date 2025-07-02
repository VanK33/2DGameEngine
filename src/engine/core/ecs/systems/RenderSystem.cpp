#include "RenderSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include <algorithm>
#include <iostream>

namespace engine::ECS {

RenderSystem::RenderSystem(engine::graphics::SpriteRenderer* spriteRenderer, 
                           engine::resources::ResourceManager* resourceManager)
    : spriteRenderer_(spriteRenderer)
    , resourceManager_(resourceManager)
    , renderedSpriteCount_(0) {
}

void RenderSystem::Init() {
    std::cout << "[RenderSystem] Initialized with layered rendering support" << std::endl;
}

void RenderSystem::Update(float deltaTime) {
    if (!world_ || !spriteRenderer_ || !resourceManager_) {
        return;
    }

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
    // Load texture
    SDL_Texture* texture = resourceManager_->LoadTexture(renderable.sprite->texturePath);
    if (!texture) {
        // Texture loading failed, skip rendering
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
    
    // Call SpriteRenderer for rendering
    spriteRenderer_->Draw(texture, 
                         transform->x, transform->y,
                         spriteWidth, spriteHeight,
                         transform->rotation);
    
    renderedSpriteCount_++;
}

} // namespace engine::ECS 