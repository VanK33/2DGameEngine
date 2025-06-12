// src / engine / GameApp.hpp

#pragma once

#include "graphics/Renderer.hpp"
#include "graphics/SpriteRenderer.hpp"
#include "scenes/SceneManager.hpp"
#include "input/InputManager.hpp"
#include "events/EventManager.hpp"
#include <memory>

namespace engine {

class GameApp {
public:
    GameApp();
    ~GameApp();

    bool Init();
    void Run();
    void Shutdown();

private:
    bool isRunning_ = false;

    graphics::Renderer renderer_;
    std::unique_ptr<graphics::SpriteRenderer> spriteRenderer_;
    scene::SceneManager sceneManager_;
    input::InputManager inputManager_;
    game::events::EventManager eventManager_;

    void RegisterScenes();
};

}
