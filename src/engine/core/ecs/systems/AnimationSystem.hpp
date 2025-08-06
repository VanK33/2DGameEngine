// src/engine/core/ecs/systems/AnimationSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/resource/ResourceManager.hpp"
#include "engine/core/Types.hpp"

namespace engine::ECS {

using engine::EntityID;

class AnimationSystem : public System {
public:
    AnimationSystem(engine::resources::ResourceManager* resourceManager);
    ~AnimationSystem() = default;

    void Update(float deltaTime) override;
    const char* GetName() const override { return "AnimationSystem"; }

private:
    void UpdateAnimationFrame(engine::EntityID entityId);
    void CalculateFrameDimensions(const std::string& texturePath, int framesPerRow, int& frameWidth, int& frameHeight);

    engine::resources::ResourceManager* resourceManager_;
};

} // namespace engine::ECS