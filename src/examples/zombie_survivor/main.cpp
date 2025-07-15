#include "engine/Engine.hpp"
#include "GameScene.hpp"
#include <iostream>

int main() {
    std::cout << "=== Game Scene Test - Background Rendering ===" << std::endl;
    
    // 创建引擎
    engine::Engine gameEngine;
    
    // 配置引擎
    engine::EngineConfig config;
    config.windowTitle = "Game Scene - Background Test";
    config.windowWidth = 800;
    config.windowHeight = 600;
    config.targetFPS = 60;
    
    // 初始化引擎
    if (!gameEngine.Initialize(config)) {
        std::cerr << "❌ Failed to initialize engine!" << std::endl;
        return -1;
    }
    
    std::cout << "✅ Engine initialized successfully" << std::endl;
    
    // 注册游戏场景
    auto& sceneManager = gameEngine.GetSceneManager();
    auto& inputManager = gameEngine.GetInputManager();
    auto& resourceManager = gameEngine.GetResourceManager();
    auto& eventManager = gameEngine.GetEventManager();
    auto& renderer = gameEngine.GetRenderer();
    
    // 创建并注册游戏场景（传入InputManager和ResourceManager）
    gameEngine.RegisterScene<ZombieSurvivor::GameScene>(
        "game_scene", 
        "game_scene",
        &inputManager,
        &resourceManager
    );
    
    // 切换到游戏场景
    sceneManager.RequestSceneChange("game_scene");
    
    std::cout << "✅ Game scene loaded, starting engine..." << std::endl;
    std::cout << "📋 Controls: ESC=Quit" << std::endl;
    std::cout << "🎯 Current Goal: Test background rendering with input support" << std::endl;
    
    // 运行引擎
    gameEngine.Run();
    
    std::cout << "🧹 Engine shutting down..." << std::endl;
    return 0;
}
