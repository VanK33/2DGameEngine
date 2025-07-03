#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Sprite2D.hpp"
#include "engine/graphics/sprite/SpriteRenderer.hpp"
#include "engine/resource/ResourceManager.hpp"
#include "engine/core/Types.hpp"
#include <vector>

namespace engine::ECS {

using engine::EntityID;

class RenderSystem : public System {
public:
    RenderSystem(engine::graphics::SpriteRenderer* spriteRenderer, 
                 engine::resources::ResourceManager* resourceManager);
    ~RenderSystem() = default;

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    const char* GetName() const override { return "RenderSystem"; }

    // 渲染统计信息
    size_t GetRenderedSpriteCount() const { return renderedSpriteCount_; }
    void ResetStats();

private:
    struct RenderableSprite {
        EntityID entityId;
        Transform2D* transform;
        Sprite2D* sprite;
        
        // 用于排序：renderLayer越大越后渲染（显示在上层）
        bool operator<(const RenderableSprite& other) const {
            return sprite->renderLayer < other.sprite->renderLayer;
        }
    };

    void CollectRenderableSprites(std::vector<RenderableSprite>& renderables);
    void RenderSprite(const RenderableSprite& renderable);

    engine::graphics::SpriteRenderer* spriteRenderer_;
    engine::resources::ResourceManager* resourceManager_;
    
    size_t renderedSpriteCount_ = 0;
};

} // namespace engine::ECS
