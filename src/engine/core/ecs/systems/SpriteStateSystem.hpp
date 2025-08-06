// src/engine/core/ecs/systems/SpriteStateSystem.hpp

#pragma once
#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/SpriteStateComponent.hpp"
#include "engine/resource/ResourceManager.hpp"
#include "engine/core/Types.hpp"

namespace engine::ECS {

class SpriteStateSystem : public System {
public:
    SpriteStateSystem(engine::resources::ResourceManager* resourceManager);
    ~SpriteStateSystem() = default;

    void Update(float deltaTime) override;
    const char* GetName() const override { return "SpriteStateSystem"; }

private:
    // Calculate direction based on velocity
    SpriteStateComponent::Direction CalculateDirection(float vx, float vy) const;
    
    // Update sprite based on current state
    void UpdateEntitySprite(EntityID entityId);
    
    // Reset animation when sprite changes
    void ResetAnimation(EntityID entityId);
    
    engine::resources::ResourceManager* resourceManager_;
    
    // Threshold for considering movement (to avoid jitter)
    static constexpr float MOVEMENT_THRESHOLD = 10.0f;
};

} // namespace engine::ECS