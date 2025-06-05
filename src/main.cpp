#include "graphics/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include <iostream>

int main() {
    graphics::Renderer renderer;
    if (!renderer.Init("Jeff2DEngine", 800, 600)) {
        return 1;
    }

    resources::ResourceManager resourceManager(renderer.GetSDLRenderer());
    SDL_Texture* texture = resourceManager.LoadTexture("../assets/test.jpg");

    if (!texture) {
        return 1;
    }

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        renderer.BeginFrame();
        renderer.DrawTexture(texture, 100, 100, 64, 64);
        renderer.EndFrame();
    }

    resourceManager.UnloadAll();
    renderer.Shutdown();
    return 0;
}
