#include "GameApp.hpp"
#include "scenes/DebugScene.hpp"
#include <SDL3/SDL.h>
#include <iostream>

namespace engine {

GameApp::GameApp() = default;
GameApp::~GameApp() = default;

bool GameApp::Init() {
    if (!renderer_.Init("Jeff2DEngine", 800, 600)) {
        return false;
    }

    spriteRenderer_ = std::make_unique<graphics::SpriteRenderer>(renderer_.GetSDLRenderer());

    sceneManager_.SetEventManager(&eventManager_);
    sceneManager_.SetInputManager(&inputManager_);

    RegisterScenes();
    sceneManager_.RequestSceneChange("DebugA");

    isRunning_ = true;
    return true;
}

void GameApp::Run() {
    Uint64 lastTime = SDL_GetTicks();

    while (isRunning_) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning_ = false;
            }
            inputManager_.HandleEvent(event);
            sceneManager_.HandleEvent(event);
        }

        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        inputManager_.Update();
        eventManager_.update();
        sceneManager_.Update(deltaTime);

        renderer_.BeginFrame();
        sceneManager_.Render(renderer_.GetSDLRenderer());
        renderer_.EndFrame();
    }
}

void GameApp::Shutdown() {
    renderer_.Shutdown();
}

void GameApp::RegisterScenes() {
    sceneManager_.RegisterScene("DebugA", [this]() {
        return std::make_unique<scene::DebugScene>("DebugA", renderer_.GetSDLRenderer(), spriteRenderer_.get());
    });

    sceneManager_.RegisterScene("DebugB", [this]() {
        return std::make_unique<scene::DebugScene>("DebugB", renderer_.GetSDLRenderer(), spriteRenderer_.get());
    });
}

}