// src/scene/TestScene.hpp

#pragma once
#include "Scene.hpp"
#include <SDL3/SDL.h>
#include <string>

namespace scene {

class TestScene : public Scene {
public:
    TestScene() = default;
    ~TestScene() override = default;
    
    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    void HandleEvent(const SDL_Event& event) override;
    
    // 实现引擎要求的场景标识符
    std::string GetSceneId() const override { return "TestScene"; }

private:
    SDL_Color bgColor_ = {255, 165, 0, 255}; // Orange
};

} // namespace scene