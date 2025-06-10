#include "graphics/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "scenes/SceneManager.hpp"
#include "scenes/DebugScene.hpp"
#include <iostream>

int main() {
    graphics::Renderer renderer;
    if (!renderer.Init("Jeff2DEngine", 800, 600)) {
        return 1;
    }

    game::events::EventManager eventManager;
    scene::SceneManager sceneManager;
    sceneManager.SetEventManager(&eventManager); 

    sceneManager.RegisterScene("DebugA", []() {
        return std::make_unique<scene::DebugScene>("DebugA");
    });
    sceneManager.RegisterScene("DebugB", []() {
        return std::make_unique<scene::DebugScene>("DebugB");
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
