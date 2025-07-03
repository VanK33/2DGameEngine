// src/engine/core/ecs/systems/DebugRenderSystem.hpp
#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Sprite2D.hpp"
#include "engine/input/InputManager.hpp"
#include "engine/core/Types.hpp"
#include <SDL3/SDL.h>


namespace engine::ECS {

using engine::EntityID;

class DebugRenderSystem : public System {
public:
    DebugRenderSystem(SDL_Renderer* renderer, engine::input::InputManager* inputManager);

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    const char* GetName() const override { return "DebugRenderSystem"; }

    bool IsDebugModeEnabled() const { return debugModeEnabled_; }

private:

    void HandleInput();
    void RenderDebugInfo();

    void RenderGrid(int gridSize = 50);
    void RenderMouseCoordinates();
    void RenderEntityBoundingBoxes();
    void RenderEntityInfo(EntityID entity, const Transform2D* transform, const Sprite2D* sprite);

    SDL_Renderer* renderer_;
    engine::input::InputManager* inputManager_;
    bool debugModeEnabled_ = false;
    bool f1KeyWasPressed_ = false;
};

} // namespace engine::ECS 
