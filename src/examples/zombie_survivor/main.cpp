#include "engine/Engine.hpp"
#include "ZombieTestScene.hpp"
#include <iostream>

int main() {
    std::cout << "=== Zombie Survivor Systems Test ===" << std::endl;
    
    // 创建引擎
    engine::Engine gameEngine;
    
    // 配置引擎
    engine::EngineConfig config;
    config.windowTitle = "Zombie Survivor Test";
    config.windowWidth = 800;
    config.windowHeight = 600;
    config.targetFPS = 60;
    
    // 初始化引擎
    if (!gameEngine.Initialize(config)) {
        std::cerr << "❌ Failed to initialize engine!" << std::endl;
        return -1;
    }
    
    std::cout << "✅ Engine initialized successfully" << std::endl;
    
    // 注册测试场景
    auto& sceneManager = gameEngine.GetSceneManager();
    auto& inputManager = gameEngine.GetInputManager();
    auto& eventManager = gameEngine.GetEventManager();
    auto& renderer = gameEngine.GetRenderer();
    
    // 创建并注册测试场景
    gameEngine.RegisterScene<ZombieSurvivor::ZombieTestScene>(
        "zombie_test", 
        "zombie_test",
        renderer.GetSDLRenderer(),
        &inputManager,
        &eventManager
    );
    
    // 切换到测试场景
    sceneManager.RequestSceneChange("zombie_test");
    
    std::cout << "✅ Test scene loaded, starting engine..." << std::endl;
    std::cout << "📋 Controls: SPACE=Skip test phase, R=Restart tests, ESC=Quit" << std::endl;
    
    // 运行引擎
    gameEngine.Run();
    
    std::cout << "🧹 Engine shutting down..." << std::endl;
    gameEngine.Shutdown();
    
    return 0;
}
