// src/examples/zombie_survivor/GameScene.cpp

#include "GameScene.hpp"
#include "engine/core/ecs/SystemManager.hpp"
#include "engine/core/ecs/ComponentManager.hpp"
#include "engine/core/ecs/EntityFactory.hpp"

// 组件
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Sprite2D.hpp"

// 系统
#include "ecs/systems/GroundRenderSystem.hpp"

#include <iostream>

namespace ZombieSurvivor {

GameScene::GameScene(const std::string& id, 
                     engine::input::InputManager* inputManager) 
    : sceneId_(id), inputManager_(inputManager) {  // ✅ 初始化inputManager_
    std::cout << "[GameScene] Created with ID: " << sceneId_ << std::endl;
}

void GameScene::Load() {
    std::cout << "[GameScene] Loading scene..." << std::endl;
    
    // ✅ 从基类获取Engine传入的World
    world_ = GetWorld();  
    if (!world_) {
        std::cout << "[GameScene] ERROR: No World provided by Engine!" << std::endl;
        return;
    }
    
    // 现在可以安全地使用world_了
    InitializeSystems();
    CreateEntities();
    
    std::cout << "[GameScene] Scene loaded successfully!" << std::endl;
}

void GameScene::Unload() {
    std::cout << "[GameScene] Unloading scene..." << std::endl;
    
    // SystemManager 会自动处理所有系统的 Shutdown
    if (world_) {
        world_->GetSystemManager().ClearAllSystems();
        world_->ClearAllEntities();
    }
    
    std::cout << "[GameScene] Scene unloaded." << std::endl;
}

void GameScene::Update(float deltaTime) {
    if (!world_) return;
    
    // ✅ 一行代码搞定所有系统更新
    world_->GetSystemManager().Update(deltaTime);
}

void GameScene::Render(SDL_Renderer* renderer) {
    if (!world_) return;
    
    // 清空屏幕（基础背景色）
    SDL_SetRenderDrawColor(renderer, 20, 40, 20, 255);
    SDL_RenderClear(renderer);
    
    // 渲染已经在SystemManager.Update()中处理了
    // RenderSystem 会在其 Update() 方法中进行实际渲染
}

void GameScene::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.key == SDLK_ESCAPE) {
            std::cout << "[GameScene] ESC pressed - requesting quit" << std::endl;
            
            // ✅ 发送SDL_QUIT事件，引擎会自动处理
            SDL_Event quitEvent;
            quitEvent.type = SDL_EVENT_QUIT;
            quitEvent.common.timestamp = SDL_GetTicks();
            SDL_PushEvent(&quitEvent);
        }
    }
}

std::string GameScene::GetSceneId() const {
    return sceneId_;
}

void GameScene::SetEventManager(engine::event::EventManager* manager) {
    // 基类Scene已经处理了，这里可能不需要额外实现
}

void GameScene::SetInputManager(engine::input::InputManager* manager) {
    inputManager_ = manager;  // ✅ 实现Setter
}

void GameScene::InitializeSystems() {
    std::cout << "[GameScene] Initializing systems..." << std::endl;
    
    // ✅ 从基类获取Engine传入的World
    world_ = GetWorld();  
    if (!world_) {
        std::cout << "[GameScene] ERROR: No World provided by Engine!" << std::endl;
        return;
    }
    
    auto& systemManager = world_->GetSystemManager();
    
    // 1. 背景渲染系统（创建背景实体）
    auto groundSystem = std::make_unique<System::GroundRenderSystem>();
    systemManager.AddSystem(std::move(groundSystem), 10);
    
    // ✅ 2. 未来添加需要InputManager的系统
    // auto inputSystem = std::make_unique<System::InputSystem>();
    // inputSystem->SetInputManager(inputManager_);  // 传递InputManager
    // systemManager.AddSystem(std::move(inputSystem), 20);
    
    std::cout << "[GameScene] Systems initialized successfully!" << std::endl;
    std::cout << "[GameScene] InputManager available: " << (inputManager_ ? "YES" : "NO") << std::endl;
}

void GameScene::CreateEntities() {
    std::cout << "[GameScene] Creating game entities..." << std::endl;
    
    if (!world_) {
        std::cout << "[GameScene] Error: World not available!" << std::endl;
        return;
    }
    
    auto& entityFactory = world_->GetEntityFactory();
    auto& componentManager = world_->GetComponentManager();
    
    // ✅ 未来在这里创建玩家
    // CreatePlayer(entityFactory, componentManager);
    
    std::cout << "[GameScene] Game entities created!" << std::endl;
}

} // namespace ZombieSurvivor
