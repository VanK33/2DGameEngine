#include "../src/graphics/Renderer.hpp"
#include "../src/resources/ResourceManager.hpp"
#include "../src/scenes/SceneManager.hpp"
#include "../src/scenes/TestScene.hpp"
#include "../src/scenes/WhiteScene.hpp"
#include "../src/events/EventManager.hpp"
#include "../src/events/EventListener.hpp"
#include "../src/events/EventType.hpp"
#include "../src/events/Event.hpp"
#include <iostream>

using namespace game::events;

// payload 示例
struct KeyData {
    int keycode;
};

// 临时监听器类
class KeyboardLogger : public EventListener {
public:
    void onEvent(std::shared_ptr<Event> event) override {
        if (event->getType() == EventType::KEY_DOWN) {
            auto data = std::static_pointer_cast<KeyData>(event->getData());
            std::cout << "[KeyboardLogger] Key pressed: " << data->keycode << "\n";
        }
    }
};

int main() {
    graphics::Renderer renderer;
    if (!renderer.Init("Jeff2DEngine", 800, 600)) {
        return 1;
    }

    EventManager eventManager;
    KeyboardLogger logger;
    eventManager.subscribe(EventType::KEY_DOWN, &logger);

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
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                auto keycode = static_cast<int>(event.key.key);
                auto payload = std::make_shared<KeyData>(KeyData{keycode});
                eventManager.publish(std::make_shared<Event>(EventType::KEY_DOWN, payload));
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

