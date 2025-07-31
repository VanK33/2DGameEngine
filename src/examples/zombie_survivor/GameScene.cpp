// src/examples/zombie_survivor/GameScene.cpp

#include "GameScene.hpp"
#include "engine/core/ecs/SystemManager.hpp"
#include "engine/core/ecs/ComponentManager.hpp"
#include "engine/core/ecs/EntityFactory.hpp"
#include "engine/core/ecs/systems/RenderSystem.hpp"
#include "examples/zombie_survivor/ecs/systems/InputSystem.hpp"
#include "examples/zombie_survivor/ecs/systems/MovementSystem.hpp"
#include "examples/zombie_survivor/ecs/systems/BoundarySystem.hpp"
#include "examples/zombie_survivor/ecs/systems/RotationSystem.hpp"
#include "examples/zombie_survivor/ecs/systems/WeaponFollowSystem.hpp"
#include "examples/zombie_survivor/ecs/systems/HUDRenderSystem.hpp"
#include "examples/zombie_survivor/ecs/systems/HUDDataSystem.hpp"
#include "examples/zombie_survivor/ecs/systems/EnemySpawnSystem.hpp"
#include "examples/zombie_survivor/ecs/systems/ZombieAISystem.hpp"
#include "examples/zombie_survivor/ecs/systems/DamageSystem.hpp"
#include "examples/zombie_survivor/ecs/systems/HealthSystem.hpp"
#include "examples/zombie_survivor/ecs/systems/ExperienceSystem.hpp"
#include "engine/core/ecs/systems/CollisionSystem.hpp"

// 组件
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Sprite2D.hpp"
#include "examples/zombie_survivor/ecs/components/AimingComponent.hpp"
#include "examples/zombie_survivor/ecs/components/InputComponent.hpp"

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
    SetupGameWorldViewport();
    
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
}

void GameScene::Render(SDL_Renderer* renderer) {
    if (!world_) return;
    
    // Debug: Draw weapon aim direction and mouse position (disabled to remove yellow square)
    // RenderDebugAiming(renderer);
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
    
    // Configure collision rules
    auto* collisionSystem = dynamic_cast<engine::ECS::CollisionSystem*>(
        systemManager.GetSystem("engine::ECS::CollisionSystem"));
    if (collisionSystem) {
        collisionSystem->AddCollisionLayer("player", true);
        collisionSystem->AddCollisionLayer("enemy", true);
        collisionSystem->AddCollisionLayer("projectile", true);
        
        // Enable collisions between layers
        collisionSystem->SetCollisionRule("player", "enemy", true);
        collisionSystem->SetCollisionRule("projectile", "enemy", true);
        collisionSystem->SetCollisionRule("player", "projectile", false);
        
        std::cout << "[GameScene] Collision rules configured!" << std::endl;
    }
    
    auto groundSystem = std::make_unique<System::GroundRenderSystem>();
    systemManager.AddSystem(std::move(groundSystem), 15);
    
    auto inputSystem = std::make_unique<ZombieSurvivor::System::InputSystem>(*inputManager_);
    systemManager.AddSystem(std::move(inputSystem), 25);

    auto movementSystem = std::make_unique<ZombieSurvivor::System::MovementSystem>();
    systemManager.AddSystem(std::move(movementSystem), 30);

    auto boundarySystem = std::make_unique<ZombieSurvivor::System::BoundarySystem>();
    systemManager.AddSystem(std::move(boundarySystem), 32);

    auto weaponFollowSystem = std::make_unique<ZombieSurvivor::System::WeaponFollowSystem>();
    systemManager.AddSystem(std::move(weaponFollowSystem), 35);

    auto aimingSystem = std::make_unique<ZombieSurvivor::System::AimingSystem>();
    systemManager.AddSystem(std::move(aimingSystem), 38);

    auto rotationSystem = std::make_unique<ZombieSurvivor::System::RotationSystem>();
    systemManager.AddSystem(std::move(rotationSystem), 40);

    auto weaponInputSystem = std::make_unique<ZombieSurvivor::System::WeaponInputSystem>();
    systemManager.AddSystem(std::move(weaponInputSystem), 42);

    auto weaponSystem = std::make_unique<ZombieSurvivor::System::WeaponSystem>();
    systemManager.AddSystem(std::move(weaponSystem), 43);

    auto ammoSystem = std::make_unique<ZombieSurvivor::System::AmmoSystem>();
    systemManager.AddSystem(std::move(ammoSystem), 44);

    auto weaponFireSystem = std::make_unique<ZombieSurvivor::System::WeaponFireSystem>();
    systemManager.AddSystem(std::move(weaponFireSystem), 45);

    auto projectileSystem = std::make_unique<ZombieSurvivor::System::ProjectileSystem>();
    systemManager.AddSystem(std::move(projectileSystem), 48);  // Before RenderSystem(50) to ensure cleanup

    auto enemySpawnSystem = std::make_unique<ZombieSurvivor::System::EnemySpawnSystem>();
    enemySpawnSystem->SetViewportSize(850.0f, 850.0f);  // Set game world viewport size
    enemySpawnSystem->SetEntityFactory(gameEntityFactory_.get());  // Inject factory dependency
    systemManager.AddSystem(std::move(enemySpawnSystem), 49);  // After projectiles, before rendering

    auto zombieAISystem = std::make_unique<ZombieSurvivor::System::ZombieAISystem>();
    systemManager.AddSystem(std::move(zombieAISystem), 46);  // Before projectiles for AI decisions

    auto damageSystem = std::make_unique<ZombieSurvivor::System::DamageSystem>();
    systemManager.AddSystem(std::move(damageSystem), 47);  // Process damage after AI, before projectiles

    auto healthSystem = std::make_unique<ZombieSurvivor::System::HealthSystem>();
    systemManager.AddSystem(std::move(healthSystem), 51);  // Process health changes after damage
    
    auto experienceSystem = std::make_unique<ZombieSurvivor::System::ExperienceSystem>();
    systemManager.AddSystem(std::move(experienceSystem), 52);  // Process experience after health

    auto hudDataSystem = std::make_unique<ZombieSurvivor::System::HUDDataSystem>();
    systemManager.AddSystem(std::move(hudDataSystem), 53);  // Update HUD data before rendering

    auto hudRenderSystem = std::make_unique<ZombieSurvivor::System::HUDRenderSystem>();
    hudRenderSystem->SetScreenSize(1512, 982); // Set correct window dimensions
    systemManager.AddSystem(std::move(hudRenderSystem), 55); // After HUDDataSystem and RenderSystem
    
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

        hudId_ = gameEntityFactory_->CreatePlayerHUD(playerId_);
        std::cout << "[GameScene] Created HUD for player " << playerId_ 
                  << ", HUD ID: " << hudId_ << std::endl;
    }
    
    std::cout << "[GameScene] Game entities created!" << std::endl;
}

void GameScene::SetupGameWorldViewport() {
    if (!world_) return;
    
    const float WINDOW_WIDTH = 1512.0f;
    const float WINDOW_HEIGHT = 982.0f;
    const float GAME_WORLD_WIDTH = 850.0f;
    const float GAME_WORLD_HEIGHT = 850.0f;
    
    float offsetX = (WINDOW_WIDTH - GAME_WORLD_WIDTH) / 2.0f;   // (1512-850)/2 = 331
    float offsetY = (WINDOW_HEIGHT - GAME_WORLD_HEIGHT) / 2.0f; // (982-850)/2 = 66
    
    auto* renderSystem = static_cast<engine::ECS::RenderSystem*>(
        world_->GetSystemManager().GetSystem("RenderSystem")
    );
    
    if (renderSystem) {
        renderSystem->SetGameWorldViewport(offsetX, offsetY, GAME_WORLD_WIDTH, GAME_WORLD_HEIGHT);
        std::cout << "[GameScene] Game world viewport set: offset(" << offsetX << ", " << offsetY 
                  << "), size(" << GAME_WORLD_WIDTH << "x" << GAME_WORLD_HEIGHT << ")" << std::endl;
    } else {
        std::cout << "[GameScene] WARNING: RenderSystem not found, viewport not set!" << std::endl;
    }
}

void GameScene::RenderDebugAiming(SDL_Renderer* renderer) {
    if (!world_ || !weaponId_ || !playerId_) return;
    
    auto& componentManager = world_->GetComponentManager();
    
    // Get player components
    auto* playerTransform = componentManager.GetComponent<engine::ECS::Transform2D>(playerId_);
    auto* playerAiming = componentManager.GetComponent<Component::AimingComponent>(playerId_);
    
    // Get weapon components
    auto* weaponTransform = componentManager.GetComponent<engine::ECS::Transform2D>(weaponId_);
    auto* weaponAiming = componentManager.GetComponent<Component::AimingComponent>(weaponId_);
    auto* weaponInput = componentManager.GetComponent<Component::InputComponent>(playerId_); // Player has input, not weapon
    
    if (!playerTransform || !weaponTransform || !weaponAiming || !weaponInput) return;
    
    // === PLAYER DEBUG VISUALIZATION ===
    float playerX = playerTransform->x;
    float playerY = playerTransform->y;
    
    // Draw player center as BLUE cross (8px)
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue
    SDL_RenderLine(renderer, playerX - 8, playerY, playerX + 8, playerY);
    SDL_RenderLine(renderer, playerX, playerY - 8, playerX, playerY + 8);
    
    // Draw player rotation direction (30px line from center)
    float playerRotX = std::cos(playerTransform->rotation) * 30.0f;
    float playerRotY = std::sin(playerTransform->rotation) * 30.0f;
    SDL_SetRenderDrawColor(renderer, 0, 150, 255, 255); // Light blue
    SDL_RenderLine(renderer, playerX, playerY, playerX + playerRotX, playerY + playerRotY);
    
    // === WEAPON DEBUG VISUALIZATION ===
    float weaponX = weaponTransform->x;
    float weaponY = weaponTransform->y;
    
    // Draw weapon position as YELLOW cross (6px)
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
    SDL_RenderLine(renderer, weaponX - 6, weaponY, weaponX + 6, weaponY);
    SDL_RenderLine(renderer, weaponX, weaponY - 6, weaponX, weaponY + 6);
    
    // Draw weapon direction line (50 pixels long)
    float aimX = weaponAiming->aimDirection.x * 50.0f;
    float aimY = weaponAiming->aimDirection.y * 50.0f;
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green
    SDL_RenderLine(renderer, weaponX, weaponY, weaponX + aimX, weaponY + aimY);
    
    // Draw line connecting player center to weapon position
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
    SDL_RenderLine(renderer, playerX, playerY, weaponX, weaponY);
    
    // === MOUSE DEBUG VISUALIZATION ===
    // Draw mouse position as RED cross (10px)
    float mouseX = weaponInput->mousePosition.x;
    float mouseY = weaponInput->mousePosition.y;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
    SDL_RenderLine(renderer, mouseX - 10, mouseY, mouseX + 10, mouseY);
    SDL_RenderLine(renderer, mouseX, mouseY - 10, mouseX, mouseY + 10);
    
}

} // namespace ZombieSurvivor
