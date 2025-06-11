#include "scenes/SceneManager.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/SpriteRenderer.hpp"
#include "scenes/SceneManager.hpp"
#include "events/EventManager.hpp"
#include "scenes/DebugScene.hpp"
#include <iostream>

int main() {
    graphics::Renderer renderer;
    if (!renderer.Init("Jeff2DEngine", 800, 600)) {
        return 1;
    }

    scene::SceneManager sceneManager;
    game::events::EventManager eventManager;

    auto spriteRenderer = std::make_unique<graphics::SpriteRenderer>(renderer.GetSDLRenderer());

    std::cout << "[Main] Listeners for SCENE_CHANGE: " 
          << eventManager.getListenerCount(game::events::EventType::SCENE_CHANGE) << std::endl;
    sceneManager.SetEventManager(&eventManager);

    sceneManager.RegisterScene("DebugA", [&renderer, &spriteRenderer]() {
    return std::make_unique<scene::DebugScene>("DebugA", renderer.GetSDLRenderer(), spriteRenderer.get());
    });

    sceneManager.RegisterScene("DebugB", [&renderer, &spriteRenderer]() {
    return std::make_unique<scene::DebugScene>("DebugB", renderer.GetSDLRenderer(), spriteRenderer.get());
    });

    sceneManager.RequestSceneChange("DebugA");

    bool running = true;
    SDL_Event event;
    Uint64 lastTime = SDL_GetTicks();
    
    while (running) {
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            sceneManager.HandleEvent(event);
        }

        eventManager.update();
        sceneManager.Update(deltaTime);

        renderer.BeginFrame();

        sceneManager.Render(renderer.GetSDLRenderer());
        renderer.EndFrame();
    }

    renderer.Shutdown();
    return 0;
}