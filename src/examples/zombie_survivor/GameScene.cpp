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
                     engine::input::InputManager* inputManager,
                     engine::resources::ResourceManager* resourceManager) 
    : sceneId_(id), inputManager_(inputManager), resourceManager_(resourceManager) {
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
    
    gameEntityFactory_ = std::make_unique<ZombieSurvivor::ECS::GameEntityFactory>(world_, resourceManager_);

    InitializeSystems();
    CreateEntities();
    
    std::cout << "[GameScene] Scene loaded successfully!" << std::endl;
}

void GameScene::Unload() {
    std::cout << "[GameScene] Unloading scene..." << std::endl;
    
    if (world_) {
        world_->GetSystemManager().ClearAllSystems();
        world_->ClearAllEntities();
    }
    
    std::cout << "[GameScene] Scene unloaded." << std::endl;
}

void GameScene::Update(float deltaTime) {
    if (!world_) return;
    world_->GetSystemManager().Update(deltaTime);
}

void GameScene::Render(SDL_Renderer* renderer) {
    if (!world_) return;
    
    // Background is already cleared by Renderer::BeginFrame()
    // Don't clear again here - it would wipe out all sprite rendering!
    
}

void GameScene::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.key == SDLK_ESCAPE) {
            std::cout << "[GameScene] ESC pressed - requesting quit" << std::endl;
            
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
    systemManager.AddSystem(std::move(groundSystem), 15);
    
    // 注意：Engine已经在核心系统中添加了RenderSystem（Engine.cpp:117-118）
    // GameScene不需要创建自己的RenderSystem，Engine的RenderSystem会处理所有Sprite2D组件
    
    // ✅ 3. 未来添加需要InputManager的系统
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
    
    gameEntityFactory_->CreatePlayer();
    
    std::cout << "[GameScene] Game entities created!" << std::endl;
}

} // namespace ZombieSurvivor
