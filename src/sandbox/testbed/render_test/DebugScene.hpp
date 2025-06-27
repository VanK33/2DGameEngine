// src/scenes/DebugScene.hpp

#pragma once

#include "engine/core/scene/Scene.hpp"
#include "engine/core/event/EventManager.hpp"
#include "engine/resource/ResourceManager.hpp"
#include "engine/graphics/sprite/SpriteRenderer.hpp"
#include "engine/input/InputManager.hpp"
#include <SDL3/SDL.h>
#include <string>
#include <vector>

// ECS test modification
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Sprite2D.hpp"
#include "engine/core/ecs/components/Collider2D.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "engine/core/ecs/components/Lifetime.hpp"
#include "engine/core/ecs/components/Tag.hpp"

namespace scene {

class DebugScene : public engine::scene::Scene {
public:
    DebugScene(const std::string& id, SDL_Renderer* renderer, engine::graphics::SpriteRenderer* spriteRenderer);
    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    void HandleEvent(const SDL_Event& event) override;
    void SetEventManager(engine::event::EventManager* manager) override;
    void SetInputManager(engine::input::InputManager* manager) override {
    inputManager_ = manager;
    }
    std::string GetSceneId() const override;

private:
    std::string sceneId_;
    engine::event::EventManager* eventManager_ = nullptr;
    engine::resources::ResourceManager resourceManager_;
    engine::graphics::SpriteRenderer* spriteRenderer_ = nullptr;

    SDL_Texture* texture_ = nullptr;
    SDL_FRect dstRect_;

    std::vector<SDL_Texture*> textures_;
    std::vector<std::string> imagePaths_;
    int currentTextureIndex_ = 0;

    engine::input::InputManager* inputManager_ = nullptr;

    // ECS tests
    engine::ECS::World world_;
    std::vector<engine::ECS::EntityID> testEntities_;
    bool ecsTestMode_ = false;
    int currentTest_ = 0;

    void RunECSTests();
    void TestEntityFactory();
    void TestComponentManager();
    void TestWorldIntegration();
    void TestSystemManager();
    void DisplayECSTestResults();

    // Input tests
    bool inputTestMode_ = false;

    // Input test state
    struct InputTestState {
        bool isTestingKeyboard = false;
        bool isTestingMouse = false;
        bool keyboardTestPassed = false;
        bool mouseTestPassed = false;
        bool eventIntegrationPassed = false;
        float keyboardTestTimer = 0.0f;
        float mouseTestTimer = 0.0f;
        int keyboardEventCount = 0;
        int mouseEventCount = 0;
        std::vector<std::string> testLog;
        bool combinedKeyTestPassed = false;
        bool mouseHeldTestPassed = false;
        bool mouseDeltaTestPassed = false;
        glm::vec2 lastMousePos{0.0f};
        float totalMouseMovement = 0.0f;
    };

    // Input test listener class definitions
    class KeyboardTestListener : public engine::event::EventListener {
    public:
        DebugScene* parentScene = nullptr;
        int eventCount = 0;
        void onEvent(const std::shared_ptr<engine::event::Event>& event) override {
            if (event->GetType() == engine::event::EventType::KEY_DOWN) {
                eventCount++;
            }
        }
    };

    class MouseTestListener : public engine::event::EventListener {
    public:
        DebugScene* parentScene = nullptr;
        int eventCount = 0;
        void onEvent(const std::shared_ptr<engine::event::Event>& event) override {
            if (event->GetType() == engine::event::EventType::MOUSE_MOVE ||
                event->GetType() == engine::event::EventType::MOUSE_CLICK) {
                eventCount++;
            }
        }
    };

    // Listener members
    std::shared_ptr<KeyboardTestListener> keyboardListener_;
    std::shared_ptr<MouseTestListener> mouseListener_;

    // Input test state instance
    InputTestState inputTestState_;

    // Test helper methods
    void StartKeyboardTest();
    void StartMouseTest();
    void RunInputTests();
    void TestKeyboardInput();
    void TestMouseInput();
    void TestEventIntegration();
    void DisplayInputTestResults();

    void TestMouseDelta();
};

}  // namespace scene

