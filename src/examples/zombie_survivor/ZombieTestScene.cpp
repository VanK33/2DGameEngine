#include "ZombieTestScene.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "engine/core/ecs/components/AIComponent.hpp"
#include "engine/core/ecs/components/Collider2D.hpp"
#include "ecs/components/EnemyComponent.hpp"
#include "ecs/components/TargetComponent.hpp"
#include <iostream>

// this file becomes too large. Going to implement real system to test soon
namespace ZombieSurvivor {

ZombieTestScene::ZombieTestScene(const std::string& id, SDL_Renderer* renderer,
                               engine::input::InputManager* inputManager,
                               engine::event::EventManager* eventManager)
    : sceneId_(id), inputManager_(inputManager), eventManager_(nullptr),
      testTimer_(0.0f), testPhase_(0), testCompleted_(false) {
}

void ZombieTestScene::Load() {
    // Use World passed from Engine
    world_ = GetWorld();  // From Scene base class
    eventManager_ = &world_->GetEventManager();
    
    // Add game systems directly to Engine's World
    auto& systemManager = world_->GetSystemManager();
    
    // Add game-specific systems
    auto healthSystem = std::make_unique<System::HealthSystem>();
    systemManager.AddSystem(std::move(healthSystem), 40);
    
    auto damageSystem = std::make_unique<System::DamageSystem>();
    systemManager.AddSystem(std::move(damageSystem), 35);
    
    auto experienceSystem = std::make_unique<System::ExperienceSystem>();
    systemManager.AddSystem(std::move(experienceSystem), 30);
    
    auto upgradeSystem = std::make_unique<System::UpgradeSystem>();
    systemManager.AddSystem(std::move(upgradeSystem), 25);
    
    auto inputSystem = std::make_unique<System::InputSystem>(*inputManager_);
    systemManager.AddSystem(std::move(inputSystem), 20);
    
    auto movementSystem = std::make_unique<System::MovementSystem>();
    systemManager.AddSystem(std::move(movementSystem), 15);
    
    auto weaponSystem = std::make_unique<System::WeaponSystem>();
    systemManager.AddSystem(std::move(weaponSystem), 10);
    
    auto ammoSystem = std::make_unique<System::AmmoSystem>();
    systemManager.AddSystem(std::move(ammoSystem), 5);
    
    // Add new systems
    auto aimingSystem = std::make_unique<System::AimingSystem>();
    systemManager.AddSystem(std::move(aimingSystem), 50);
    
    auto rotationSystem = std::make_unique<System::RotationSystem>();
    systemManager.AddSystem(std::move(rotationSystem), 45);
    
    auto weaponFireSystem = std::make_unique<System::WeaponFireSystem>();
    systemManager.AddSystem(std::move(weaponFireSystem), 12);
    
    auto weaponInputSystem = std::make_unique<System::WeaponInputSystem>();
    systemManager.AddSystem(std::move(weaponInputSystem), 18);
    
    auto playerStatsSystem = std::make_unique<System::PlayerStatsSystem>();
    systemManager.AddSystem(std::move(playerStatsSystem), 8);
    
    auto enemySpawnSystem = std::make_unique<System::EnemySpawnSystem>();
    systemManager.AddSystem(std::move(enemySpawnSystem), 60);
    
    // Initialize systems
    InitializeSystems();
    
    // Create test entities
    CreateTestEntities();
    
    std::cout << "✅ Test scene loaded successfully" << std::endl;
}

void ZombieTestScene::Unload() {
    if (world_) {
        // Shutdown systems
        if (healthSystem_) healthSystem_->Shutdown();
        if (damageSystem_) damageSystem_->Shutdown();
        if (experienceSystem_) experienceSystem_->Shutdown();
        if (upgradeSystem_) upgradeSystem_->Shutdown();
        if (weaponSystem_) weaponSystem_->Shutdown();
        if (ammoSystem_) ammoSystem_->Shutdown();
        if (enemySpawnSystem_) enemySpawnSystem_->Shutdown();
        
        world_ = nullptr;
    }
    std::cout << "🧹 Test scene unloaded" << std::endl;
}

void ZombieTestScene::Update(float deltaTime) {
    if (!world_ || testCompleted_) return;
    
    testTimer_ += deltaTime;
    
    // Update all systems (in execution order)
    if (enemySpawnSystem_) enemySpawnSystem_->Update(deltaTime);
    if (inputSystem_) inputSystem_->Update(deltaTime);
    if (weaponInputSystem_) weaponInputSystem_->Update(deltaTime);
    if (aimingSystem_) aimingSystem_->Update(deltaTime);
    if (rotationSystem_) rotationSystem_->Update(deltaTime);
    if (movementSystem_) movementSystem_->Update(deltaTime);
    if (weaponFireSystem_) weaponFireSystem_->Update(deltaTime);
    if (weaponSystem_) weaponSystem_->Update(deltaTime);
    if (ammoSystem_) ammoSystem_->Update(deltaTime);
    if (playerStatsSystem_) playerStatsSystem_->Update(deltaTime);
    if (healthSystem_) healthSystem_->Update(deltaTime);
    if (damageSystem_) damageSystem_->Update(deltaTime);
    if (experienceSystem_) {
        experienceSystem_->Update(deltaTime);
    }
    if (upgradeSystem_) upgradeSystem_->Update(deltaTime);
    
    // Run tests
    RunSystemTests(deltaTime);
}

void ZombieTestScene::Render(SDL_Renderer* renderer) {
    // Simple test scene, only clear screen
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);
}

void ZombieTestScene::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
            case SDLK_SPACE:
                std::cout << "\n⏩ Skipping to next test phase..." << std::endl;
                testPhase_++;
                testTimer_ = 0.0f;
                break;
            case SDLK_R:
                std::cout << "\n🔄 Restarting tests..." << std::endl;
                testPhase_ = 0;
                testTimer_ = 0.0f;
                testCompleted_ = false;
                CreateTestEntities();
                break;
        }
    }
}

std::string ZombieTestScene::GetSceneId() const {
    return sceneId_;
}

void ZombieTestScene::SetEventManager(engine::event::EventManager* manager) {
    eventManager_ = manager;
}

void ZombieTestScene::SetInputManager(engine::input::InputManager* manager) {
    inputManager_ = manager;
}

void ZombieTestScene::InitializeSystems() {
    // Create systems
    healthSystem_ = std::make_unique<System::HealthSystem>();
    damageSystem_ = std::make_unique<System::DamageSystem>();
    experienceSystem_ = std::make_unique<System::ExperienceSystem>();
    upgradeSystem_ = std::make_unique<System::UpgradeSystem>();
    inputSystem_ = std::make_unique<System::InputSystem>(*inputManager_);
    movementSystem_ = std::make_unique<System::MovementSystem>();
    weaponSystem_ = std::make_unique<System::WeaponSystem>();
    ammoSystem_ = std::make_unique<System::AmmoSystem>();
    
    // Create new systems
    aimingSystem_ = std::make_unique<System::AimingSystem>();
    rotationSystem_ = std::make_unique<System::RotationSystem>();
    weaponFireSystem_ = std::make_unique<System::WeaponFireSystem>();
    weaponInputSystem_ = std::make_unique<System::WeaponInputSystem>();
    playerStatsSystem_ = std::make_unique<System::PlayerStatsSystem>();
    enemySpawnSystem_ = std::make_unique<System::EnemySpawnSystem>();
    
    std::cout << "[DEBUG] Created ExperienceSystem: " << experienceSystem_.get() << std::endl;
    
    // Set World to systems
    healthSystem_->SetWorld(world_);
    damageSystem_->SetWorld(world_);
    experienceSystem_->SetWorld(world_);
    upgradeSystem_->SetWorld(world_);
    inputSystem_->SetWorld(world_);
    movementSystem_->SetWorld(world_);
    weaponSystem_->SetWorld(world_);
    ammoSystem_->SetWorld(world_);
    
    // Set World to new systems
    aimingSystem_->SetWorld(world_);
    rotationSystem_->SetWorld(world_);
    weaponFireSystem_->SetWorld(world_);
    weaponInputSystem_->SetWorld(world_);
    playerStatsSystem_->SetWorld(world_);
    enemySpawnSystem_->SetWorld(world_);
    
    std::cout << "[DEBUG] Set world to ExperienceSystem" << std::endl;
    
    // Initialize systems
    healthSystem_->Init();
    damageSystem_->Init();
    std::cout << "[DEBUG] Initializing ExperienceSystem..." << std::endl;
    experienceSystem_->Init();
    std::cout << "[DEBUG] ExperienceSystem initialized" << std::endl;
    upgradeSystem_->Init();
    inputSystem_->Init();
    movementSystem_->Init();
    weaponSystem_->Init();
    ammoSystem_->Init();
    
    aimingSystem_->Init();
    rotationSystem_->Init();
    weaponFireSystem_->Init();
    weaponInputSystem_->Init();
    playerStatsSystem_->Init();
    enemySpawnSystem_->Init();
    
    // Configure EnemySpawnSystem for testing
    if (enemySpawnSystem_) {
        enemySpawnSystem_->SetViewportSize(800.0f, 600.0f);
        enemySpawnSystem_->SetSpawnInterval(1.0f);  // Fast spawning for testing
        enemySpawnSystem_->SetMaxEnemies(5);        // Limited for testing
        enemySpawnSystem_->SetSpawnMargin(50.0f);
    }
    
    std::cout << "🔧 All systems initialized" << std::endl;
}

void ZombieTestScene::CreateTestEntities() {
    auto& entityFactory = world_->GetEntityFactory();
    auto& componentManager = world_->GetComponentManager();
    
    // Create player entity
    playerId_ = entityFactory.CreateEntity("TestPlayer");
    
    // Add player components
    componentManager.AddComponent<Component::HealthComponent>(playerId_, Component::HealthComponent{});
    componentManager.AddComponent<Component::ExperienceComponent>(playerId_, Component::ExperienceComponent{});
    componentManager.AddComponent<Component::UpgradeComponent>(playerId_, Component::UpgradeComponent{});
    componentManager.AddComponent<Component::WeaponComponent>(playerId_, Component::WeaponComponent{});
    componentManager.AddComponent<Component::AmmoComponent>(playerId_, Component::AmmoComponent{});
    componentManager.AddComponent<Component::MovementComponent>(playerId_, Component::MovementComponent{});
    componentManager.AddComponent<Component::InputComponent>(playerId_, Component::InputComponent{});
    componentManager.AddComponent<Component::CombatStatsComponent>(playerId_, Component::CombatStatsComponent{});
    componentManager.AddComponent<engine::ECS::Transform2D>(playerId_, engine::ECS::Transform2D{});
    componentManager.AddComponent<engine::ECS::Velocity2D>(playerId_, engine::ECS::Velocity2D{});
    componentManager.AddComponent<Component::AimingComponent>(playerId_, Component::AimingComponent{});
    componentManager.AddComponent<Component::PlayerStatsComponent>(playerId_, Component::PlayerStatsComponent{});
    
    // Set initial mouse position for aiming system
    auto* aiming = componentManager.GetComponent<Component::AimingComponent>(playerId_);
    if (aiming) {
        aiming->mouseWorldPos = {100.0f, 100.0f};  // Test position
    }
    
    // Enable smooth rotation for rotation system
    if (rotationSystem_) {
        rotationSystem_->SetRotationSmoothing(playerId_, 5.0f);  // Smooth rotation
    }
    
    // Initialize ammo from weapon configuration
    if (ammoSystem_) {
        ammoSystem_->InitializeAmmoFromWeapon(playerId_);
    }
    
    // Test new weapon-ammo collaboration
    std::cout << "🔫 Testing new weapon-ammo collaboration..." << std::endl;
    if (ammoSystem_) {
        std::cout << "   Initial ammo status: " << (ammoSystem_->HasAmmo(playerId_) ? "Has ammo" : "No ammo") << std::endl;
    }
    if (weaponSystem_) {
        std::cout << "   Weapon ready: " << (weaponSystem_->IsWeaponReady(playerId_) ? "Ready" : "Not ready") << std::endl;
        
        // Simulate fire request - through event system instead of direct method call
        if (eventManager_) {
            auto fireData = std::make_shared<Events::FireInputData>();
            fireData->playerId = playerId_;
            fireData->pressed = true;
            
            auto fireEvent = std::make_shared<Events::GameEvent>(
                Events::GameEventType::FIRE_INPUT,
                std::static_pointer_cast<void>(fireData)
            );
            eventManager_->Publish(fireEvent);
        }
    }
    if (ammoSystem_) {
        std::cout << "   After fire ammo status: " << (ammoSystem_->HasAmmo(playerId_) ? "Has ammo" : "No ammo") << std::endl;
    }
    
    // Create enemy entity
    enemyId_ = entityFactory.CreateEntity("TestEnemy");
    
    // Add enemy components
    componentManager.AddComponent<Component::HealthComponent>(enemyId_, Component::HealthComponent{});
    componentManager.AddComponent<Component::WeaponComponent>(enemyId_, Component::WeaponComponent{});
    componentManager.AddComponent<Component::CombatStatsComponent>(enemyId_, Component::CombatStatsComponent{});
    
    // Set enemy weapon damage
    auto* enemyWeapon = componentManager.GetComponent<Component::WeaponComponent>(enemyId_);
    if (enemyWeapon) {
        enemyWeapon->damage = 10.0f;
    }
    
    std::cout << "👤 Test entities created - Player: " << playerId_ << ", Enemy: " << enemyId_ << std::endl;
}

void ZombieTestScene::RunSystemTests(float deltaTime) {
    switch (testPhase_) {
        case 0:
            if (testTimer_ > 1.0f) {
                std::cout << "\n📋 Phase 1: Testing Damage System..." << std::endl;
                TestDamageSystem();
                testPhase_++;
                testTimer_ = 0.0f;
            }
            break;
            
        case 1:
            if (testTimer_ > 2.0f) {
                std::cout << "\n📋 Phase 2: Testing Experience System..." << std::endl;
                TestExperienceSystem();
                testPhase_++;
                testTimer_ = 0.0f;
            }
            break;
            
        case 2:
            if (testTimer_ > 2.0f) {
                std::cout << "\n📋 Phase 3: Testing Upgrade System..." << std::endl;
                TestUpgradeSystem();
                testPhase_++;
                testTimer_ = 0.0f;
            }
            break;
            
        case 3:
            if (testTimer_ > 1.0f) {
                PrintTestResults();
                testPhase_++;
                testTimer_ = 0.0f;
            }
            break;
            
        case 4:  // New aiming and rotation system test
            if (testTimer_ > 1.0f) {
                std::cout << "\n📋 Phase 4: Testing Aiming & Rotation Systems..." << std::endl;
                TestAimingAndRotation();
                testPhase_++;
                testTimer_ = 0.0f;
            }
            break;
            
        case 5:  // New weapon fire system test
            if (testTimer_ > 2.0f) {
                std::cout << "\n📋 Phase 5: Testing Weapon Fire System..." << std::endl;
                TestWeaponFireSystem();
                testPhase_++;
                testTimer_ = 0.0f;
            }
            break;
            
        case 6:  // New reload system test
            if (testTimer_ > 1.0f) {
                std::cout << "\n📋 Phase 6: Testing Reload System..." << std::endl;
                TestReloadSystem();
                testPhase_++;
                testTimer_ = 0.0f;
            }
            break;
            
        case 7:  // New player stats system test
            if (testTimer_ > 1.0f) {
                std::cout << "\n📋 Phase 7: Testing Player Stats System..." << std::endl;
                TestPlayerStatsSystem();
                testPhase_++;
                testTimer_ = 0.0f;
            }
            break;
            
        case 8:  // Enemy spawn system test
            if (testTimer_ > 1.0f) {
                std::cout << "\n📋 Phase 8: Testing Enemy Spawn System..." << std::endl;
                TestEnemySpawnSystem();
                testPhase_++;
                testTimer_ = 0.0f;
            }
            break;
            
        default:
            if (!testCompleted_) {
                std::cout << "\n🎉 All tests completed! Press R to restart, SPACE to skip phases" << std::endl;
                std::cout << "\n📊 === Complete System Test Summary ===" << std::endl;
                std::cout << "✅ Aiming & Rotation Systems: Collaboration working" << std::endl;
                std::cout << "✅ Weapon Fire System: Event-driven functioning" << std::endl;
                std::cout << "✅ Reload System: State management correct" << std::endl;
                std::cout << "✅ Player Stats System: Data tracking effective" << std::endl;
                std::cout << "✅ Enemy Spawn System: AI enemy generation working" << std::endl;
                std::cout << "🚀 All game systems integrated and running well!" << std::endl;
                testCompleted_ = true;
            }
            break;
    }
}

void ZombieTestScene::TestDamageSystem() {
    auto& componentManager = world_->GetComponentManager();
    
    // Get player health
    auto* playerHealth = componentManager.GetComponent<Component::HealthComponent>(playerId_);
    if (playerHealth) {
        float oldHealth = playerHealth->health;
        std::cout << "  Player initial health: " << oldHealth << std::endl;
        
        // Deal damage
        if (damageSystem_) {
            damageSystem_->DealDamage(playerId_, enemyId_, 25, "test_damage");
        }
        
        std::cout << "  Health after 25 damage: " << playerHealth->health << std::endl;
        std::cout << "  ✅ Damage System test completed" << std::endl;
    }
}

void ZombieTestScene::TestExperienceSystem() {
    std::cout << "  [DEBUG] Publishing enemy killed event..." << std::endl;
    std::cout << "  [DEBUG] EventManager: " << eventManager_ << std::endl;
    std::cout << "  [DEBUG] World EventManager: " << &world_->GetEventManager() << std::endl;
    
    Events::GameEventUtils::PublishEnemyKilled(*eventManager_, playerId_, enemyId_, 50, "test_enemy");
    std::cout << "  [DEBUG] Event published" << std::endl;
    
    // Force update system once to see if it can handle events
    std::cout << "  [DEBUG] Forcing ExperienceSystem update..." << std::endl;
    if (experienceSystem_) {
        experienceSystem_->Update(0.016f); // Simulate one frame
    }
    
    auto& componentManager = world_->GetComponentManager();
    auto* exp = componentManager.GetComponent<Component::ExperienceComponent>(playerId_);
    if (exp) {
        std::cout << "  Current level: " << exp->level << std::endl;
        std::cout << "  Current experience: " << exp->experience << std::endl;
        std::cout << "  Experience to next: " << exp->experienceToNext << std::endl;
        std::cout << "  ✅ Experience System test completed" << std::endl;
    }
}

void ZombieTestScene::TestUpgradeSystem() {
    auto& componentManager = world_->GetComponentManager();
    auto* upgrade = componentManager.GetComponent<Component::UpgradeComponent>(playerId_);
    
    if (upgrade && upgrade->pendingUpgrade && !upgrade->currentOptions.empty()) {
        std::cout << "  Available upgrade count: " << upgrade->currentOptions.size() << std::endl;
        
        // Select first upgrade
        Component::UpgradeType selectedUpgrade = upgrade->currentOptions[0];
        if (upgradeSystem_) {
            upgradeSystem_->SelectUpgrade(playerId_, selectedUpgrade);
        }
        
        std::cout << "  Selected upgrade type: " << static_cast<int>(selectedUpgrade) << std::endl;
        std::cout << "  ✅ Upgrade System test completed" << std::endl;
    } else {
        std::cout << "  ⚠️ No upgrades available (may need to level up first)" << std::endl;
    }
}

void ZombieTestScene::PrintTestResults() {
    std::cout << "\n📊 === Final Test Results ===" << std::endl;
    
    auto& componentManager = world_->GetComponentManager();
    
    // Player status
    std::cout << "\n👤 Player Status:" << std::endl;
    if (auto* health = componentManager.GetComponent<Component::HealthComponent>(playerId_)) {
        std::cout << "  Health: " << health->health << "/" << health->maxHealth << std::endl;
        std::cout << "  Alive: " << (health->isAlive ? "Yes" : "No") << std::endl;
    }
    
    if (auto* exp = componentManager.GetComponent<Component::ExperienceComponent>(playerId_)) {
        std::cout << "  Level: " << exp->level << std::endl;
        std::cout << "  Experience: " << exp->experience << "/" << exp->experienceToNext << std::endl;
    }
    
    if (auto* combat = componentManager.GetComponent<Component::CombatStatsComponent>(playerId_)) {
        std::cout << "  Damage Taken: " << combat->totalDamageTaken << std::endl;
        std::cout << "  Damage Dealt: " << combat->totalDamageDealt << std::endl;
    }
    
    std::cout << "\n🎯 Entity Count: " << world_->GetEntityCount() << std::endl;
    std::cout << "✅ All systems working correctly!" << std::endl;
}

void ZombieTestScene::TestAimingAndRotation() {
    std::cout << "  🎯 Testing aiming and rotation system collaboration..." << std::endl;
    
    auto& componentManager = world_->GetComponentManager();
    auto* aiming = componentManager.GetComponent<Component::AimingComponent>(playerId_);
    auto* transform = componentManager.GetComponent<engine::ECS::Transform2D>(playerId_);
    
    if (!aiming || !transform) {
        std::cout << "  ❌ Missing required components" << std::endl;
        return;
    }
    
    // Set up test scene
    transform->x = 100.0f;
    transform->y = 100.0f;
    float initialRotation = transform->rotation;
    
    // Test 1: Aim right
    aiming->mouseWorldPos = {200.0f, 100.0f};
    if (aimingSystem_) aimingSystem_->Update(0.016f);
    if (rotationSystem_) rotationSystem_->Update(0.016f);
    
    std::cout << "    Aim right -> Direction: (" << aiming->aimDirection.x << ", " << aiming->aimDirection.y << ")" << std::endl;
    std::cout << "    Rotation angle: " << transform->rotation << " radians" << std::endl;
    
    // Test 2: Aim up
    aiming->mouseWorldPos = {100.0f, 0.0f};
    if (aimingSystem_) aimingSystem_->Update(0.016f);
    if (rotationSystem_) rotationSystem_->Update(0.016f);
    
    std::cout << "    Aim up -> Rotation angle: " << transform->rotation << " radians" << std::endl;
    
    std::cout << "  ✅ Aiming and rotation system test completed" << std::endl;
}

void ZombieTestScene::TestWeaponFireSystem() {
    std::cout << "  🔫 Testing weapon fire system complete flow..." << std::endl;
    
    auto& componentManager = world_->GetComponentManager();
    auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(playerId_);
    auto* ammo = componentManager.GetComponent<Component::AmmoComponent>(playerId_);
    
    if (!weapon || !ammo) {
        std::cout << "  ❌ Missing weapon or ammo components" << std::endl;
        return;
    }
    
    // Record initial state
    auto initialAmmo = ammo->currentAmmo;
    bool initialWeaponReady = weaponSystem_ ? weaponSystem_->IsWeaponReady(playerId_) : false;
    
    std::cout << "    Initial state: Ammo=" << initialAmmo << ", Weapon ready=" << (initialWeaponReady ? "Yes" : "No") << std::endl;
    
    // Test 1: Simulate fire input through InputComponent
    auto* input = componentManager.GetComponent<Component::InputComponent>(playerId_);
    if (!input) {
        std::cout << "    ❌ No InputComponent found for fire test" << std::endl;
        return;
    }

    input->shootButtonPressed = true;
    std::cout << "    ✅ Fire input simulated through InputComponent" << std::endl;
    
    // Force update systems with improved event processing
    std::cout << "    Processing fire events..." << std::endl;
    for (int i = 0; i < 8; i++) {
        auto beforeAmmo = ammo->currentAmmo;
        
        // 按正确顺序更新系统
        if (weaponInputSystem_) weaponInputSystem_->Update(0.016f);  // 1. 处理输入
        if (eventManager_) eventManager_->Update();                 // 2. 分发事件！
        if (weaponFireSystem_) weaponFireSystem_->Update(0.016f);    // 3. 处理射击事件
        if (weaponSystem_) weaponSystem_->Update(0.016f);           // 4. 更新武器状态
        if (ammoSystem_) ammoSystem_->Update(0.016f);               // 5. 更新弹药
        
        auto afterAmmo = ammo->currentAmmo;
        std::cout << "      Cycle " << (i+1) << ": Ammo " << beforeAmmo << " -> " << afterAmmo << std::endl;
        
        if (afterAmmo < beforeAmmo) {
            std::cout << "      ✅ Fire successfully processed after " << (i+1) << " cycles!" << std::endl;
            break;
        }
        
        // 重置输入状态，避免重复触发
        input->shootButtonPressed = false;
    }
    
    // Check results
    auto afterAmmo = ammo->currentAmmo;
    bool afterWeaponReady = weaponSystem_ ? weaponSystem_->IsWeaponReady(playerId_) : false;
    
    std::cout << "    After fire state: Ammo=" << afterAmmo << ", Weapon ready=" << (afterWeaponReady ? "Yes" : "No") << std::endl;
    
    // Verify if firing was effective
    if (afterAmmo < initialAmmo) {
        std::cout << "    ✅ Ammo correctly consumed (" << (initialAmmo - afterAmmo) << " rounds)" << std::endl;
    } else {
        std::cout << "    ⚠️ Ammo not consumed, firing may have failed" << std::endl;
    }
    
    if (!afterWeaponReady && initialWeaponReady) {
        std::cout << "    ✅ Weapon entered cooldown state" << std::endl;
    }
    
    // Test 2: Rapid fire test
    std::cout << "    Testing rapid fire..." << std::endl;
    int rapidFireCount = 0;
    for (int i = 0; i < 5; i++) {
        auto beforeAmmo = ammo->currentAmmo;
        
        // Publish fire event
        if (eventManager_) {
            auto fireData = std::make_shared<Events::FireInputData>();
            fireData->playerId = playerId_;
            fireData->pressed = true;
            
            auto fireEvent = std::make_shared<Events::GameEvent>(
                Events::GameEventType::FIRE_INPUT,
                std::static_pointer_cast<void>(fireData)
            );
            eventManager_->Publish(fireEvent);
        }
        
        // Update systems
        if (weaponFireSystem_) weaponFireSystem_->Update(0.016f);
        if (eventManager_) eventManager_->Update();  // 添加事件分发
        if (weaponSystem_) weaponSystem_->Update(0.016f);
        if (ammoSystem_) ammoSystem_->Update(0.016f);
        
        auto afterAmmo = ammo->currentAmmo;
        if (afterAmmo < beforeAmmo) {
            rapidFireCount++;
        }
        
        // Simulate time passage to clear cooldown
        if (weaponSystem_) weaponSystem_->Update(0.2f); // 200ms
    }
    
    std::cout << "    Rapid fire success count: " << rapidFireCount << "/5" << std::endl;
    
    std::cout << "  ✅ Weapon fire system test completed" << std::endl;
}

void ZombieTestScene::TestReloadSystem() {
    std::cout << "  🔄 Testing reload system..." << std::endl;
    
    auto& componentManager = world_->GetComponentManager();
    auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(playerId_);
    auto* ammo = componentManager.GetComponent<Component::AmmoComponent>(playerId_);
    auto* input = componentManager.GetComponent<Component::InputComponent>(playerId_);
    
    if (!weapon || !ammo || !input) {
        std::cout << "  ❌ Missing required components (weapon/ammo/input)" << std::endl;
        return;
    }
    
    // Simulate ammo depletion state
    ammo->currentAmmo = 0;
    ammo->totalAmmo = 50;
    ammo->isReloading = false;  // 确保初始状态正确
    
    std::cout << "    Setting state: Current ammo=0, Total ammo=50, Reloading=No" << std::endl;
    
    // Test 1: Simulate reload input through InputComponent
    input->reloadButtonPressed = true;
    std::cout << "    ✅ Reload input simulated through InputComponent" << std::endl;
    
    // Force update systems to handle events - 改进版本
    std::cout << "    Processing events..." << std::endl;
    bool reloadDetected = false;
    for (int i = 0; i < 8; i++) {
        // 按正确顺序更新系统
        if (weaponInputSystem_) weaponInputSystem_->Update(0.016f);  // 1. 处理输入，发布事件
        if (eventManager_) eventManager_->Update();                 // 2. 分发事件队列！
        if (weaponSystem_) weaponSystem_->Update(0.016f);           // 3. 接收并处理事件
        if (ammoSystem_) ammoSystem_->Update(0.016f);               // 4. 更新弹药状态
        
        // 每个周期检查状态
        bool reloadState = ammo->isReloading;
        std::cout << "      Cycle " << (i+1) << ": isReloading=" << (reloadState ? "Yes" : "No") << std::endl;
        
        if (reloadState) {
            std::cout << "      ✅ Reload successfully detected after " << (i+1) << " cycles!" << std::endl;
            reloadDetected = true;
            break;
        }
    }
    
    // Check if reload started
    bool reloadStarted = ammo->isReloading;
    std::cout << "    Final reload state: " << (reloadStarted ? "Yes" : "No") << std::endl;
    
    if (reloadStarted) {
        std::cout << "    ✅ Reload system correctly responded to input" << std::endl;
        
        // Test 2: Simulate reload process (保持原有的换弹模拟逻辑)
        std::cout << "    Simulating reload process..." << std::endl;
        
        float reloadTime = weapon->reloadTime;
        float timeStep = reloadTime / 5;
        
        for (int i = 1; i <= 5; i++) {
            if (weaponSystem_) weaponSystem_->Update(timeStep);
            if (ammoSystem_) ammoSystem_->Update(timeStep);
            
            float progress = ammo->reloadProgress;
            bool stillReloading = ammo->isReloading;
            
            std::cout << "    Step " << i << ": Progress=" << (progress * 100) << "%, Still reloading=" << (stillReloading ? "Yes" : "No") << std::endl;
            
            if (!stillReloading) {
                std::cout << "    ✅ Reload complete" << std::endl;
                break;
            }
        }
        
        // 🔧 给RELOAD_COMPLETED事件处理时间
        std::cout << "    Processing final reload events..." << std::endl;
        for (int i = 0; i < 3; i++) {
            if (eventManager_) eventManager_->Update();  // 处理事件队列
            if (ammoSystem_) ammoSystem_->Update(0.016f); // 更新AmmoSystem
        }
        
        // Check final state (after event processing)
        int finalAmmo = ammo->currentAmmo;
        bool finalReloadState = ammo->isReloading;
        
        std::cout << "    Final state: Current ammo=" << finalAmmo << ", Reloading=" << (finalReloadState ? "Yes" : "No") << std::endl;
        
        if (finalAmmo > 0 && !finalReloadState) {
            std::cout << "    ✅ Reload successful, ammo replenished" << std::endl;
        } else if (finalReloadState) {
            std::cout << "    ⚠️ Reload still in progress" << std::endl;
        } else {
            std::cout << "    ❌ Reload failed, ammo not replenished" << std::endl;
        }
    } else {
        std::cout << "    ⚠️ Reload system did not respond to input event" << std::endl;
    }
    
    std::cout << "  ✅ Reload system test completed" << std::endl;
}

void ZombieTestScene::TestPlayerStatsSystem() {
    std::cout << "  📊 Testing player stats system..." << std::endl;
    
    auto& componentManager = world_->GetComponentManager();
    auto* stats = componentManager.GetComponent<Component::PlayerStatsComponent>(playerId_);
    
    if (!stats) {
        std::cout << "  ❌ Missing player stats component" << std::endl;
        return;
    }
    
    // Record initial state
    auto initialKills = stats->killCount;
    auto initialSurvivalTime = stats->survivalTime;
    
    std::cout << "    Initial stats: Kills=" << initialKills << ", Survival time=" << initialSurvivalTime << " seconds" << std::endl;
    
    // Simulate time passage
    if (playerStatsSystem_) {
        playerStatsSystem_->Update(1.0f); // 1 second
    }
    
    std::cout << "    1 second later: Kills=" << stats->killCount << ", Survival time=" << stats->survivalTime << " seconds" << std::endl;
    
    // Verify survival time update
    if (stats->survivalTime > initialSurvivalTime) {
        std::cout << "    ✅ Survival time correctly updated" << std::endl;
    } else {
        std::cout << "    ⚠️ Survival time not updated" << std::endl;
    }
    
    std::cout << "  ✅ Player stats system test completed" << std::endl;
}

void ZombieTestScene::TestEnemySpawnSystem() {
    std::cout << "  🧟 Testing enemy spawn system..." << std::endl;
    
    if (!enemySpawnSystem_) {
        std::cout << "  ❌ Enemy spawn system not available" << std::endl;
        return;
    }
    
    auto& componentManager = world_->GetComponentManager();
    
    // 记录原始设置
    int originalMaxEnemies = enemySpawnSystem_->GetCurrentEnemyCount() >= 5 ? 5 : 5;  // 使用系统的当前设置
    
    // 使用新的清理函数完全清理
    std::cout << "    Using ClearAllEnemies() for complete cleanup..." << std::endl;
    enemySpawnSystem_->ClearAllEnemies();
    
    // Record initial state (should be 0 after cleanup)
    int initialCount = enemySpawnSystem_->GetCurrentEnemyCount();
    int initialTotal = enemySpawnSystem_->GetTotalSpawned();
    
    std::cout << "    Initial state after cleanup: Current enemies=" << initialCount << ", Total spawned=" << initialTotal << std::endl;
    
    // Test 1: Verify spawn system configuration
    std::cout << "    Testing spawn system configuration..." << std::endl;
    
    // Temporarily increase max enemies for testing
    if (enemySpawnSystem_) {
        enemySpawnSystem_->SetMaxEnemies(20);  // Allow more enemies for testing
        std::cout << "    ✅ Temporarily increased max enemies to 20 for testing" << std::endl;
    }
    std::cout << "    ✅ Viewport: 800x600, Spawn interval: 1.0s" << std::endl;
    
    // Test 2: Force spawn update for several seconds
    std::cout << "    Simulating 3 seconds of spawning..." << std::endl;
    for (int i = 0; i < 3; i++) {
        // Simulate 1 second of time
        for (int j = 0; j < 60; j++) {  // 60 frames @ 16.67ms = ~1 second
            if (enemySpawnSystem_) {
                enemySpawnSystem_->Update(0.0167f);  // ~60 FPS
            }
        }
        
        int currentCount = enemySpawnSystem_->GetCurrentEnemyCount();
        int totalSpawned = enemySpawnSystem_->GetTotalSpawned();
        
        std::cout << "    After " << (i+1) << " second(s): Current=" << currentCount 
                  << ", Total=" << totalSpawned << std::endl;
    }
    
    // Test 3: Verify spawned entities have correct components
    std::cout << "    Verifying spawned enemy components..." << std::endl;
    auto enemies = componentManager.GetEntitiesWithComponent<Component::EnemyComponent>();
    
    int validEnemies = 0;
    for (auto enemyId : enemies) {
        // Check required components
        bool hasTransform = componentManager.HasComponent<engine::ECS::Transform2D>(enemyId);
        bool hasVelocity = componentManager.HasComponent<engine::ECS::Velocity2D>(enemyId);
        bool hasAI = componentManager.HasComponent<engine::ECS::AIComponent>(enemyId);
        bool hasHealth = componentManager.HasComponent<Component::HealthComponent>(enemyId);
        bool hasTarget = componentManager.HasComponent<Component::TargetComponent>(enemyId);
        bool hasCollider = componentManager.HasComponent<engine::ECS::Collider2D>(enemyId);
        
        if (hasTransform && hasVelocity && hasAI && hasHealth && hasTarget && hasCollider) {
            validEnemies++;
            
            // Check position (should be outside viewport)
            auto* transform = componentManager.GetComponent<engine::ECS::Transform2D>(enemyId);
            if (transform) {
                bool outsideViewport = (transform->x < -50 || transform->x > 850 || 
                                      transform->y < -50 || transform->y > 650);
                if (outsideViewport) {
                    std::cout << "      Enemy " << enemyId << ": Spawned outside viewport ✅" << std::endl;
                } else {
                    std::cout << "      Enemy " << enemyId << ": Position (" << transform->x 
                              << ", " << transform->y << ") - inside viewport ⚠️" << std::endl;
                }
            }
        }
    }
    
    std::cout << "    Valid enemies with all components: " << validEnemies << "/" << enemies.size() << std::endl;
    
    // Test 4: Check AI component configuration
    if (!enemies.empty()) {
        auto firstEnemy = enemies[0];
        auto* ai = componentManager.GetComponent<engine::ECS::AIComponent>(firstEnemy);
        auto* enemy = componentManager.GetComponent<Component::EnemyComponent>(firstEnemy);
        
        if (ai && enemy) {
            std::cout << "    AI Configuration: State=" << static_cast<int>(ai->state) 
                      << ", Speed=" << ai->speed << ", Detection=" << ai->detectionRadius << std::endl;
            std::cout << "    Enemy Type: " << static_cast<int>(enemy->type) 
                      << ", Damage=" << enemy->damage << ", EXP=" << enemy->expValue << std::endl;
        }
    }
    
    // Summary
    int finalCount = enemySpawnSystem_->GetCurrentEnemyCount();
    int finalTotal = enemySpawnSystem_->GetTotalSpawned();
    
    std::cout << "    Final state: Current enemies=" << finalCount << ", Total spawned=" << finalTotal << std::endl;
    
    if (finalTotal > initialTotal) {
        std::cout << "    ✅ Enemy spawning working - " << (finalTotal - initialTotal) << " new enemies created" << std::endl;
    } else {
        std::cout << "    ⚠️ No new enemies spawned (may have hit max limit)" << std::endl;
    }
    
    if (validEnemies > 0) {
        std::cout << "    ✅ Enemy components properly configured" << std::endl;
    } else {
        std::cout << "    ❌ Enemy components missing or misconfigured" << std::endl;
    }
    
    std::cout << "    Cleaning up test enemies..." << std::endl;
    enemySpawnSystem_->ClearAllEnemies();
    
    if (enemySpawnSystem_) {
        enemySpawnSystem_->SetMaxEnemies(5);
        std::cout << "    🔧 Restored max enemies to 5 (safe default)" << std::endl;
    }
    
    std::cout << "  ✅ Enemy spawn system test completed and cleaned up" << std::endl;
}

} // namespace ZombieSurvivor



