// src/scene/Scene.hpp

#pragma once

#include <SDL3/SDL.h>
#include <string>

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

protected:
    // Scene可以请求场景切换（通过事件系统）
    void RequestSceneChange(const std::string& sceneId);
};


} // namespace scene
