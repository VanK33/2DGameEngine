// src/scene/whiteScene.hpp

#pragma once
#include "Scene.hpp"
#include <SDL3/SDL.h>
#include <string>

namespace scene {

class WhiteScene : public Scene {
public:
    WhiteScene() = default;
    ~WhiteScene() override = default;
    
    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    void HandleEvent(const SDL_Event& event) override;
    
    // 实现引擎要求的场景标识符
    std::string GetSceneId() const override { return "whiteScene"; }

private:
    SDL_Color bgColor_ = {255, 255, 255, 255}; // White
};

} // namespace scene