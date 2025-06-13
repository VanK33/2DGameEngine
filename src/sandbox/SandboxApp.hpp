// src / engine / GameApp.hpp

#pragma once

#include "engine/graphics/renderer/Renderer.hpp"
#include "engine/graphics/sprite/SpriteRenderer.hpp"
#include "engine/core/scene/SceneManager.hpp"
#include "engine/input/InputManager.hpp"
#include "engine/core/event/EventManager.hpp"
#include <memory>

namespace sandbox {

class SandboxApp {
public:
    SandboxApp();
    ~SandboxApp();

    bool Init();
    void Run();
    void Shutdown();

private:
    bool isRunning_ = false;

    engine::graphics::Renderer renderer_;
    std::unique_ptr<graphics::SpriteRenderer> spriteRenderer_;
    engine::scene::SceneManager sceneManager_;
    engine::input::InputManager inputManager_;
    engine::event::EventManager eventManager_;

    void RegisterScenes();
};

}
