// src/scenes/DebugScene.hpp

#pragma once

#include "engine/core/scene/Scene.hpp"
#include "engine/core/event/EventManager.hpp"
#include "engine/resource/ResourceManager.hpp"
#include "engine/graphics/sprite/SpriteRenderer.hpp"
#include "engine/input/InputManager.hpp"
#include <SDL3/SDL.h>
#include <string>
#include <vector>

namespace scene {

class DebugScene : public engine::scene::Scene {
public:
    DebugScene(const std::string& id, SDL_Renderer* renderer, graphics::SpriteRenderer* spriteRenderer);
    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    void HandleEvent(const SDL_Event& event) override;
    void SetEventManager(engine::event::EventManager* manager) override;
    void SetInputManager(engine::input::InputManager* manager) override {
    inputManager_ = manager;
    }
    std::string GetSceneId() const override;

private:
    std::string sceneId_;
    engine::event::EventManager* eventManager_ = nullptr;
    engine::resources::ResourceManager resourceManager_;
    graphics::SpriteRenderer* spriteRenderer_ = nullptr;

    SDL_Texture* texture_ = nullptr;
    SDL_FRect dstRect_;

    std::vector<SDL_Texture*> textures_;
    std::vector<std::string> imagePaths_;
    int currentTextureIndex_ = 0;

    engine::input::InputManager* inputManager_ = nullptr;
};

}  // namespace scene

