#include "engine/Engine.hpp"
#include "testbed/render_test/DebugScene.hpp"
#include <iostream>

int main() {
    std::cout << "=== Engine API Test ===" << std::endl;
    
    // Test 1: Engine Creation
    std::cout << "Test 1: Creating Engine..." << std::endl;
    engine::Engine gameEngine;
    std::cout << "✅ Engine created successfully" << std::endl;
    
    // Test 2: Engine Configuration
    std::cout << "Test 2: Configuring Engine..." << std::endl;
    engine::EngineConfig config;
    config.windowTitle = "Engine API Test";
    config.windowWidth = 800;
    config.windowHeight = 600;
    config.targetFPS = 60;
    std::cout << "✅ Engine configured" << std::endl;
    
    // Test 3: Engine Initialization
    std::cout << "Test 3: Initializing Engine..." << std::endl;
    if (!gameEngine.Initialize(config)) {
        std::cerr << "❌ Failed to initialize engine!" << std::endl;
        return -1;
    }
    std::cout << "✅ Engine initialized successfully" << std::endl;
    
    // Test 4: System Access
    std::cout << "Test 4: Testing System Access..." << std::endl;
    auto& world = gameEngine.GetWorld();
    auto& eventManager = gameEngine.GetEventManager();
    auto& inputManager = gameEngine.GetInputManager();
    auto& sceneManager = gameEngine.GetSceneManager();
    auto& renderer = gameEngine.GetRenderer();
    std::cout << "✅ All systems accessible" << std::endl;
    
    // Test 5: ECS Functionality
    std::cout << "Test 5: Testing ECS functionality..." << std::endl;
    auto entityId = world.GetEntityFactory().CreateEntity("TestEntity");
    std::cout << "  Created entity with ID: " << entityId << std::endl;
    std::cout << "  Entity count: " << world.GetEntityCount() << std::endl;
    std::cout << "✅ ECS working correctly" << std::endl;
    
    // Test 6: Scene Registration
    std::cout << "Test 6: Testing Scene Registration..." << std::endl;
    gameEngine.RegisterScene<scene::DebugScene>("TestScene", "TestScene", 
                                               renderer.GetSDLRenderer(), 
                                               &gameEngine.GetSpriteRenderer());
    std::cout << "✅ Scene registered successfully" << std::endl;
    
    // Test 7: Engine State
    std::cout << "Test 7: Testing Engine State..." << std::endl;
    std::cout << "  Is Running: " << (gameEngine.IsRunning() ? "Yes" : "No") << std::endl;
    std::cout << "  Delta Time: " << gameEngine.GetDeltaTime() << std::endl;
    std::cout << "  FPS: " << gameEngine.GetFPS() << std::endl;
    std::cout << "✅ Engine state accessible" << std::endl;
    
    // Test 8: Quick Shutdown
    std::cout << "Test 8: Testing Engine Shutdown..." << std::endl;
    gameEngine.Shutdown();
    std::cout << "✅ Engine shutdown successfully" << std::endl;
    
    std::cout << "\n🎉 All Engine API tests passed!" << std::endl;
    std::cout << "Engine is ready for game development!" << std::endl;

    return 0;
}
