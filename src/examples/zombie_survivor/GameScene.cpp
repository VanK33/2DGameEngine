// src/examples/zombie_survivor/GameScene.cpp

#include "GameScene.hpp"
#include "engine/core/ecs/SystemManager.hpp"
#include "engine/core/ecs/ComponentManager.hpp"
#include "engine/core/ecs/EntityFactory.hpp"
#include "examples/zombie_survivor/ecs/systems/InputSystem.hpp"
#include "examples/zombie_survivor/ecs/systems/MovementSystem.hpp"
#include "examples/zombie_survivor/ecs/systems/RotationSystem.hpp"
#include "examples/zombie_survivor/ecs/systems/WeaponFollowSystem.hpp"

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
}

void GameScene::SetInputManager(engine::input::InputManager* manager) {
    inputManager_ = manager;
}

void GameScene::InitializeSystems() {
    std::cout << "[GameScene] Initializing systems..." << std::endl;
    
    world_ = GetWorld();  
    if (!world_) {
        std::cout << "[GameScene] ERROR: No World provided by Engine!" << std::endl;
        return;
    }
    
    auto& systemManager = world_->GetSystemManager();
    
    auto groundSystem = std::make_unique<System::GroundRenderSystem>();
    systemManager.AddSystem(std::move(groundSystem), 15);
    
    auto inputSystem = std::make_unique<ZombieSurvivor::System::InputSystem>(*inputManager_);
    systemManager.AddSystem(std::move(inputSystem), 25);

    auto movementSystem = std::make_unique<ZombieSurvivor::System::MovementSystem>();
    systemManager.AddSystem(std::move(movementSystem), 30);

    auto weaponFollowSystem = std::make_unique<ZombieSurvivor::System::WeaponFollowSystem>();
    systemManager.AddSystem(std::move(weaponFollowSystem), 35);

    auto aimingSystem = std::make_unique<ZombieSurvivor::System::AimingSystem>();
    systemManager.AddSystem(std::move(aimingSystem), 38);

    auto rotationSystem = std::make_unique<ZombieSurvivor::System::RotationSystem>();
    systemManager.AddSystem(std::move(rotationSystem), 40);
    
    std::cout << "[GameScene] Systems initialized successfully!" << std::endl;
}

void GameScene::CreateEntities() {
    std::cout << "[GameScene] Creating game entities..." << std::endl;
    
    if (!world_) {
        std::cout << "[GameScene] Error: World not available!" << std::endl;
        return;
    }
    
    auto& entityFactory = world_->GetEntityFactory();
    auto& componentManager = world_->GetComponentManager();
    
    // Create player and store ID
    playerId_ = gameEntityFactory_->CreatePlayer();
    
    // Create weapon that follows the player
    if (playerId_ != 0) {
        weaponId_ = gameEntityFactory_->CreateWeapon(playerId_);
    }
    
    std::cout << "[GameScene] Game entities created!" << std::endl;
}

} // namespace ZombieSurvivor
