// src/examples/zombie_survivor/ecs/systems/GroundRenderSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Sprite2D.hpp"
#include "examples/zombie_survivor/ecs/RenderLayer.hpp"
#include <vector>

namespace ZombieSurvivor::System {

class GroundRenderSystem : public engine::ECS::System {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "GroundRenderSystem"; }

    void CreateGroundBackground();
    void AddGroundDecorations();

private:
    uint32_t groundEntityId_ = 0;
    std::vector<uint32_t> decorationEntities_;

};
} // namespace ZombieSurvivor::System