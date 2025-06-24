#include "DebugScene.hpp"
#include "engine/core/event/Event.hpp"
#include "engine/core/event/EventType.hpp"
#include "engine/core/event/SceneEvents.hpp"
#include "engine/utils/PathUtils.hpp"
#include <iostream>
#include <cassert>

namespace scene {

DebugScene::DebugScene(const std::string& id, SDL_Renderer* renderer, graphics::SpriteRenderer* spriteRenderer) : sceneId_(id), resourceManager_(renderer), spriteRenderer_(spriteRenderer) {}

void DebugScene::Load() {
    std::cout << "[DebugScene] Loaded: " << sceneId_ << "\n";
    assert(spriteRenderer_ && "SpriteRenderer not set!");

    imagePaths_ = {
        "img1.jpg",
        "img2.jpg",
        "img3.jpg"
    };

    for (const auto& path : imagePaths_) {
        SDL_Texture* tex = resourceManager_.LoadTexture(path);
        if (tex) {
            textures_.push_back(tex);
            SDL_Log("[DebugScene] Loaded texture: %s", path.c_str());
        }
    }

    if (!textures_.empty()) {
        currentTextureIndex_ = 0;
    }

    dstRect_ = {100.0f, 100.0f, 256.0f, 256.0f}; 
}

void DebugScene::Unload() {
    // release ex
    // resourceManager->ReleaseTexture("example.png");

    for (const auto& path : imagePaths_) {
        resourceManager_.ReleaseTexture(path);
    }

    textures_.clear();
    imagePaths_.clear();
    currentTextureIndex_ = 0;

    resourceManager_.PrintCacheStatus(); // debugger
}

void DebugScene::Update(float deltaTime) {
    if (!textures_.empty() && inputManager_) {
        if (inputManager_->IsKeyDown(SDLK_RIGHT)) {
            currentTextureIndex_ = (currentTextureIndex_ + 1) % textures_.size();
            SDL_Log("[DebugScene] Switched to texture index: %d", currentTextureIndex_);
        }

        if (inputManager_->IsKeyDown(SDLK_LEFT)) {
            currentTextureIndex_ = (currentTextureIndex_ - 1 + textures_.size()) % textures_.size();
            SDL_Log("[DebugScene] Switched to texture index: %d", currentTextureIndex_);
        }
    }

    if (inputTestState_.isTestingKeyboard) {
        inputTestState_.keyboardTestTimer += deltaTime;
        if (inputTestState_.keyboardTestTimer >= 5.0f) {
            inputTestState_.isTestingKeyboard = false;
            if (eventManager_) {
                eventManager_->Unsubscribe(engine::event::EventType::KEY_DOWN, keyboardListener_.get());
            }
            
            inputTestState_.keyboardEventCount = keyboardListener_ ? keyboardListener_->eventCount : 0;
            bool passed = inputTestState_.keyboardEventCount > 0;
            inputTestState_.keyboardTestPassed = passed;
            
            inputTestState_.testLog.push_back(
                "Keyboard Test: " + std::string(passed ? "PASSED" : "FAILED") + 
                " (Events: " + std::to_string(inputTestState_.keyboardEventCount) + ")"
            );
            
            StartMouseTest();
        }
    }
    
    if (inputTestState_.isTestingMouse) {
        inputTestState_.mouseTestTimer += deltaTime;
        if (inputTestState_.mouseTestTimer >= 5.0f) {
            inputTestState_.isTestingMouse = false;
            if (eventManager_) {
                eventManager_->Unsubscribe(engine::event::EventType::MOUSE_MOVE, mouseListener_.get());
                eventManager_->Unsubscribe(engine::event::EventType::MOUSE_CLICK, mouseListener_.get());
            }
            
            inputTestState_.mouseEventCount = mouseListener_ ? mouseListener_->eventCount : 0;
            bool passed = inputTestState_.mouseEventCount > 0;
            inputTestState_.mouseTestPassed = passed;
            
            inputTestState_.testLog.push_back(
                "Mouse Test: " + std::string(passed ? "PASSED" : "FAILED") + 
                " (Events: " + std::to_string(inputTestState_.mouseEventCount) + ")"
            );
            
            TestEventIntegration();
            DisplayInputTestResults();
        }
    }
}

void DebugScene::Render(SDL_Renderer* /*renderer*/) {
    if (!textures_.empty() && spriteRenderer_) {
        spriteRenderer_->Draw(textures_[currentTextureIndex_],
                              dstRect_.x, dstRect_.y, dstRect_.w, dstRect_.h);
    }
}

void DebugScene::SetEventManager(engine::event::EventManager* manager) {
    eventManager_ = manager;
    std::cout << "[DebugScene] EventManager set to: " << (manager ? "valid" : "null") << std::endl;
}

std::string DebugScene::GetSceneId() const {
    return sceneId_;
}


void DebugScene::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
            case SDLK_1:
                ecsTestMode_ = true;
                currentTest_ = 0;
                RunECSTests();
                break;
            case SDLK_2:
                if (ecsTestMode_) {
                    ecsTestMode_ = false;
                    std::cout << "\n[DebugScene] ECS Test Mode EXITED\n";
                    std::cout << "Press 1 to restart ECS tests, or use arrow keys to navigate textures.\n";
                } else {
                    std::cout << "\n[DebugScene] ECS Test Mode already OFF\n";
                    std::cout << "Press 1 to start ECS tests.\n";
                }
                break;
            case SDLK_3:
                if (!inputTestMode_) {
                    inputTestMode_ = true;
                    std::cout << "\n[DebugScene] Input Test Mode STARTED\n";
                    std::cout << "Press SPACE to run tests, 3 to exit test mode\n";
                    inputTestState_ = InputTestState();
                } else {
                    inputTestMode_ = false;
                    inputTestState_.isTestingKeyboard = false;
                    inputTestState_.isTestingMouse = false;
                    if (eventManager_) {
                        if (keyboardListener_) {
                            eventManager_->Unsubscribe(engine::event::EventType::KEY_DOWN, keyboardListener_.get());
                        }
                        if (mouseListener_) {
                            eventManager_->Unsubscribe(engine::event::EventType::MOUSE_MOVE, mouseListener_.get());
                            eventManager_->Unsubscribe(engine::event::EventType::MOUSE_CLICK, mouseListener_.get());
                        }
                    }
                    std::cout << "\n[DebugScene] Input Test Mode EXITED\n";
                }
                break;
            case SDLK_SPACE:
                if (inputTestMode_ && !inputTestState_.isTestingKeyboard && !inputTestState_.isTestingMouse) {
                    RunInputTests();
                } else if (ecsTestMode_) {
                    currentTest_++;
                    RunECSTests();
                }
                break;
            case SDLK_RIGHT:
                if (!ecsTestMode_) {
                    currentTextureIndex_ = (currentTextureIndex_ + 1) % textures_.size();
                }
                break;
            case SDLK_LEFT:
                if (!ecsTestMode_) {
                    currentTextureIndex_ = (currentTextureIndex_ - 1 + textures_.size()) % textures_.size();
                }
                break;
        }
    }
}

void DebugScene::RunECSTests() {
    std::cout << "\n=== ECS Test " << currentTest_ << " ===\n";

    switch (currentTest_) {
        case 0:
            TestEntityFactory();
            break;
        case 1:
            TestComponentManager();
            break;
        case 2:
            TestWorldIntegration();
            break;
        default:
            std::cout << "ALL ECS tests completed!\n";
            ecsTestMode_ = false;
            break;
    }

    DisplayECSTestResults();
}

void DebugScene::TestEntityFactory() {
    std::cout << "[ECS Test] Testing EntityFactory...\n";
    
    auto id1 = world_.GetEntityFactory().CreateEntity("TestEntity1");
    auto id2 = world_.GetEntityFactory().CreateEntity("TestEntity2");
    
    testEntities_.push_back(id1);
    testEntities_.push_back(id2);
    
    std::cout << "Created entities: " << id1 << ", " << id2 << "\n";
    std::cout << "Entity count: " << world_.GetEntityCount() << "\n";
}

void DebugScene::TestComponentManager() {
    std::cout << "[ECS Test] Testing ComponentManager...\n";
    
    if (!testEntities_.empty()) {
        auto& cm = world_.GetComponentManager();
        
        // Test Transform2D
        engine::ECS::Transform2D transform{100.0f, 200.0f, 0.0f, 1.0f, 1.0f};
        cm.AddComponent(testEntities_[0], transform);
        
        // Test Sprite2D
        engine::ECS::Sprite2D sprite{"img1.jpg", {0, 0, 64, 64}, true};
        cm.AddComponent(testEntities_[0], sprite);
        
        // 
        auto entitiesWithTransform = cm.GetEntitiesWithComponent<engine::ECS::Transform2D>();
        std::cout << "Entities with Transform2D: " << entitiesWithTransform.size() << "\n";
    }
}

void DebugScene::TestWorldIntegration() {
    std::cout << "[ECS Test] Testing World Integration...\n";

    // Test World's getter interface
    auto& entityFactory = world_.GetEntityFactory();
    auto& componentManager = world_.GetComponentManager();
    auto& systemManager = world_.GetSystemManager();

    // Test entity management
    auto entityId = entityFactory.CreateEntity("WorldTestEntity");
    testEntities_.push_back(entityId);

    // Test Component Management
    engine::ECS::Transform2D transform{300.0f, 400.0f, 45.0f, 2.0f, 2.0f};
    componentManager.AddComponent(entityId, transform);

    std::cout << "World entity count: " << world_.GetEntityCount() << "\n";
    std::cout << "World has entity " << entityId << ": " << (world_.HasEntity(entityId) ? "YES" : "NO") << "\n";

    // test for pause/resume
    world_.Pause();
    std::cout << "World paused: " << (world_.IsPaused() ? "YES" : "NO") << "\n";
    world_.Resume();
    std::cout << "World resumed: " << (world_.IsPaused() ? "YES" : "NO") << "\n";
}

void DebugScene::TestSystemManager() {
    std::cout << "[ECS Test] Testing SystemManager...\n";

    auto& systemManager = world_.GetSystemManager();

    // Small Mock System for testing purpose
    class TestSystem : public engine::ECS::System {
    public:
        const char* GetName() const override { return "TestSystem"; }
        void Update(float deltaTime) override {
            std::cout << "[TestSystem] Update called with deltaTime: " << deltaTime << "\n";
        }
    };
    // Added the system
    systemManager.AddSystem(std::make_unique<TestSystem>(), 1);
    std::cout << "Added TestSystem\n";
    std::cout << "Has TestSystem: " << (systemManager.HasSystem("TestSystem") ? "YES" : "NO") << "\n";

    systemManager.PauseSystem("TestSystem");
    std::cout << "Paused TestSystem\n";
    systemManager.ResumeSystem("TestSystem");
    std::cout << "Resume TestSystem\n";

    systemManager.Update(0.016f); // mimicing 60 fps

    systemManager.RemoveSystem("TestSystem");
    std::cout << "Removed TestSystem";
}

void DebugScene::DisplayECSTestResults() {
    std::cout << "ECS Test Results:\n";
    std::cout << "- Entity count: " << world_.GetEntityCount() << "\n";
    std::cout << "- Test entities: " << testEntities_.size() << "\n";
    std::cout << "- ECS mode: " << (ecsTestMode_ ? "ON" : "OFF") << "\n";
    
    if (ecsTestMode_) {
        std::cout << "Press SPACE for next test, 2 to exit ECS mode\n";
    } else {
        std::cout << "Press SPACE to restart ECS tests, 2 to confirm exit, 1 to restart\n";
    }
}

// ************** Input Tests **************

void DebugScene::RunInputTests() {
    if (!inputManager_) {
        std::cout << "Error: InputManager not set!\n";
        return;
    }
    
    // 清除之前的测试结果
    inputTestState_.testLog.clear();
    
    // 开始键盘测试
    StartKeyboardTest();
}

void DebugScene::StartKeyboardTest() {
    std::cout << "\n=== Testing Keyboard Input ===\n";
    std::cout << "Press any key (5 seconds)...\n";
    
    if (!keyboardListener_) {
        keyboardListener_ = std::make_shared<KeyboardTestListener>();
    }
    keyboardListener_->eventCount = 0;
    
    if (eventManager_) {
        eventManager_->Subscribe(engine::event::EventType::KEY_DOWN, keyboardListener_.get());
    }
    
    // 重置并启动测试计时器
    inputTestState_.keyboardTestTimer = 0.0f;
    inputTestState_.isTestingKeyboard = true;
}

void DebugScene::StartMouseTest() {
    std::cout << "\n=== Testing Mouse Input ===\n";
    std::cout << "Move mouse and click (5 seconds)...\n";
    
    if (!mouseListener_) {
        mouseListener_ = std::make_shared<MouseTestListener>();
    }
    mouseListener_->eventCount = 0;
    
    if (eventManager_) {
        eventManager_->Subscribe(engine::event::EventType::MOUSE_MOVE, mouseListener_.get());
        eventManager_->Subscribe(engine::event::EventType::MOUSE_CLICK, mouseListener_.get());
    }
    
    // 重置并启动测试计时器
    inputTestState_.mouseTestTimer = 0.0f;
    inputTestState_.isTestingMouse = true;
}

void DebugScene::TestEventIntegration() {
    std::cout << "\n=== Testing Event Integration ===\n";
    
    bool isConnected = inputManager_ && eventManager_;
    inputTestState_.eventIntegrationPassed = isConnected;
    inputTestState_.testLog.push_back(
        "Event Integration: " + std::string(isConnected ? "PASSED" : "FAILED")
    );
}

void DebugScene::DisplayInputTestResults() {
    std::cout << "\n=== Input Test Results ===\n";
    for (const auto& log : inputTestState_.testLog) {
        std::cout << log << "\n";
    }
    std::cout << "\nOverall Status: " 
              << (inputTestState_.keyboardTestPassed && 
                  inputTestState_.mouseTestPassed && 
                  inputTestState_.eventIntegrationPassed ? "PASSED" : "FAILED")
              << "\n";
}

}  // namespace scene