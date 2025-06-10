// src/scenes/DebugScene.hpp

#pragma once

#include "Scene.hpp"
#include "events/EventManager.hpp"
#include <string>

namespace scene {

class DebugScene : public Scene {
public:
    explicit DebugScene(std::string id);
    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    void HandleEvent(const SDL_Event& event) override;
    std::string GetSceneId() const override;

private:
    std::string sceneId_;
};

} // namespace scene
