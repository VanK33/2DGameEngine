// src/scenes/DebugScene.hpp

#pragma once

#include "Scene.hpp"
#include "events/EventManager.hpp"
#include "resources/ResourceManager.hpp"
#include <SDL3/SDL.h>
#include <string>

namespace scene {

class DebugScene : public Scene {
public:
    DebugScene(const std::string& id, SDL_Renderer* renderer);
    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    void HandleEvent(const SDL_Event& event) override;
    void SetEventManager(game::events::EventManager* manager) override;
    std::string GetSceneId() const override;

private:
    std::string sceneId_;
    game::events::EventManager* eventManager_ = nullptr;
    resources::ResourceManager resourceManager_;

    SDL_Texture* texture_ = nullptr;
    SDL_FRect dstRect_ = { 100.0f, 100.0f, 128.0f, 128.0f };
};

}  // namespace scene

