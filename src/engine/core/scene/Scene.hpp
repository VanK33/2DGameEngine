// src/scene/Scene.hpp

#pragma once

#include <SDL3/SDL.h>
#include <string>
#include "engine/core/event/EventManager.hpp"
#include "engine/input/InputManager.hpp"
#include "engine/core/ecs/World.hpp"

namespace engine::scene {

class Scene {
public:
    virtual ~Scene() = default;
    virtual void Load() = 0;
    virtual void Unload() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(SDL_Renderer* renderer) = 0;
    virtual void HandleEvent(const SDL_Event& event) = 0;
    virtual std::string GetSceneId() const = 0;  // Scene identifier
    // Abstract base class for all scenes. Requires derived classes to override all virtual methods.
    virtual void SetEventManager(engine::event::EventManager* manager) = 0;
    virtual void SetInputManager(input::InputManager* manager) = 0;

    virtual void SetWorld(ECS::World* world) { world_ = world; }
    ECS::World* GetWorld() const { return world_; }

    virtual ECS::EntityID CreateSceneEntity (const std::string& name = "");
    virtual void DestroySceneEntity(ECS::EntityID id);
    virtual void ClearSceneEntities();


protected:
    engine::event::EventManager* eventManager_ = nullptr; 

    ECS::World* world_ = nullptr;
    std::vector<ECS::EntityID> sceneEntities_;
};


} // namespace scene
