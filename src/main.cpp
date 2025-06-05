#include "graphics/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "scenes/SceneManager.hpp"
#include "scenes/TestScene.hpp"
#include "scenes/WhiteScene.hpp"
#include <iostream>

int main() {
    graphics::Renderer renderer;
    if (!renderer.Init("Jeff2DEngine", 800, 600)) {
        return 1;
    }

    scene::SceneManager sceneManager;
    sceneManager.RegisterScene("TestScene", []() {
        return std::make_unique<scene::TestScene>();
    });
    
    sceneManager.RegisterScene("WhiteScene", []() {
        return std::make_unique<scene::WhiteScene>();
    });
    sceneManager.RequestSceneChange("TestScene");

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

        sceneManager.Update(deltaTime);

        renderer.BeginFrame();
        sceneManager.Render(renderer.GetSDLRenderer());
        renderer.EndFrame();
    }

    renderer.Shutdown();
    return 0;
}
