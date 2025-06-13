//src//scenes/SpirteScene.hpp

#pragma once

#include "engine/core/scene/Scene.hpp"
#include "engine/resource/ResourceManager.hpp"
#include <SDL3/SDL.h>
#include <string>

namespace sandbox::testbed {

class SpriteScene : public engine::scene::Scene {
public:
    SpriteScene(const std::string& id, SDL_Renderer* renderer);
    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    void HandleEvent(const SDL_Event& event) override;
    std::string GetSceneId() const override;

private:
    std::string sceneId_;
    engine::resources::ResourceManager resourceManager_;
    SDL_Texture* sprite_ = nullptr;
    SDL_FRect spriteRect_ = { 100.0f, 100.0f, 128.0f, 128.0f }; // location +
    float moveSpeed_ = 200.0f; // pixel / s

    engine::event::EventManager* eventManager_ = nullptr;
    engine::input::InputManager* inputManager_ = nullptr;
};

} // namespace sandbox::testbed
