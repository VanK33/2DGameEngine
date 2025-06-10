// src/scene/Scene.hpp

#pragma once

#include <SDL3/SDL.h>
#include <string>
#include "events/EventManager.hpp"

namespace scene {

class Scene {
public:
    virtual ~Scene() = default;
    virtual void Load() = 0;
    virtual void Unload() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(SDL_Renderer* renderer) = 0;
    virtual void HandleEvent(const SDL_Event& event) = 0;
    virtual std::string GetSceneId() const = 0;  // 场景标识符
    // Abstract base class for all scenes. Requires derived classes to override all virtual methods.
    virtual void SetEventManager(game::events::EventManager* manager) = 0;


protected:
    game::events::EventManager* eventManager_ = nullptr; 
};


} // namespace scene
