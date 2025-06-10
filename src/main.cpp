#include "graphics/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "scenes/SceneManager.hpp"
#include "events/EventManager.hpp"
#include "scenes/DebugScene.hpp"
#include <iostream>

int main() {
    graphics::Renderer renderer;
    if (!renderer.Init("Jeff2DEngine", 800, 600)) {
        return 1;
    }

    // ✅ 创建 ResourceManager（依赖 renderer 的 SDL_Renderer）
    resources::ResourceManager resourceManager(renderer.GetSDLRenderer());

    // 测试加载纹理
    SDL_Texture* tex = resourceManager.LoadTexture("assets/test.jpg"); // 替换为你的图片路径
    if (!tex) {
        std::cerr << "[main] Failed to load texture.\n";
    } else {
        std::cout << "[main] Texture loaded successfully.\n";
    }

    scene::SceneManager sceneManager;
    game::events::EventManager eventManager;
    sceneManager.SetEventManager(&eventManager);

    sceneManager.RegisterScene("DebugA", [&renderer]() {
    return std::make_unique<scene::DebugScene>("DebugA", renderer.GetSDLRenderer());
    });

    sceneManager.RegisterScene("DebugB", [&renderer]() {
    return std::make_unique<scene::DebugScene>("DebugB", renderer.GetSDLRenderer());
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

        sceneManager.Update(deltaTime);

        renderer.BeginFrame();

        if (tex) {
            SDL_FRect dst = { 0.0f, 0.0f, 128.0f, 128.0f };
            SDL_RenderTexture(renderer.GetSDLRenderer(), tex, nullptr, &dst);
        }

        sceneManager.Render(renderer.GetSDLRenderer());
        renderer.EndFrame();
    }

    resourceManager.UnloadAll();  // 清理
    renderer.Shutdown();
    return 0;
}
