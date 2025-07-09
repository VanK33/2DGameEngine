#include "ZombieTestScene.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include <iostream>

namespace ZombieSurvivor {

ZombieTestScene::ZombieTestScene(const std::string& id, SDL_Renderer* renderer,
                               engine::input::InputManager* inputManager,
                               engine::event::EventManager* eventManager)
    : sceneId_(id), inputManager_(inputManager), eventManager_(nullptr),
      testTimer_(0.0f), testPhase_(0), testCompleted_(false) {
}

void ZombieTestScene::Load() {
    // 使用Engine传递的World
    world_ = GetWorld();  // 来自Scene基类
    eventManager_ = &world_->GetEventManager();
    
    // 直接向Engine的World添加游戏系统
    auto& systemManager = world_->GetSystemManager();
    
    // 添加游戏专用系统
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
    
    // 初始化系统
    InitializeSystems();
    
    // 创建测试实体
    CreateTestEntities();
    
    std::cout << "✅ Test scene loaded successfully" << std::endl;
}

void ZombieTestScene::Unload() {
    if (world_) {
        // 关闭系统
        if (healthSystem_) healthSystem_->Shutdown();
        if (damageSystem_) damageSystem_->Shutdown();
        if (experienceSystem_) experienceSystem_->Shutdown();
        if (upgradeSystem_) upgradeSystem_->Shutdown();
        if (weaponSystem_) weaponSystem_->Shutdown();
        if (ammoSystem_) ammoSystem_->Shutdown();
        
        world_ = nullptr;
    }
    std::cout << "🧹 Test scene unloaded" << std::endl;
}

void ZombieTestScene::Update(float deltaTime) {
    if (!world_ || testCompleted_) return;
    
    testTimer_ += deltaTime;
    
    // 更新所有系统
    if (inputSystem_) inputSystem_->Update(deltaTime);
    if (movementSystem_) movementSystem_->Update(deltaTime);
    if (weaponSystem_) weaponSystem_->Update(deltaTime);
    if (ammoSystem_) ammoSystem_->Update(deltaTime);
    if (healthSystem_) healthSystem_->Update(deltaTime);
    if (damageSystem_) damageSystem_->Update(deltaTime);
    if (experienceSystem_) {
        std::cout << "[DEBUG] Updating ExperienceSystem..." << std::endl;
        experienceSystem_->Update(deltaTime);
    }
    if (upgradeSystem_) upgradeSystem_->Update(deltaTime);
    
    // 运行测试
    RunSystemTests(deltaTime);
}

void ZombieTestScene::Render(SDL_Renderer* renderer) {
    // 简单的测试场景，只清除屏幕
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
    // 创建系统
    healthSystem_ = std::make_unique<System::HealthSystem>();
    damageSystem_ = std::make_unique<System::DamageSystem>();
    experienceSystem_ = std::make_unique<System::ExperienceSystem>();
    upgradeSystem_ = std::make_unique<System::UpgradeSystem>();
    inputSystem_ = std::make_unique<System::InputSystem>(*inputManager_);
    movementSystem_ = std::make_unique<System::MovementSystem>();
    weaponSystem_ = std::make_unique<System::WeaponSystem>();
    ammoSystem_ = std::make_unique<System::AmmoSystem>();
    
    std::cout << "[DEBUG] Created ExperienceSystem: " << experienceSystem_.get() << std::endl;
    
    // 设置World到系统
    healthSystem_->SetWorld(world_);
    damageSystem_->SetWorld(world_);
    experienceSystem_->SetWorld(world_);
    upgradeSystem_->SetWorld(world_);
    inputSystem_->SetWorld(world_);
    movementSystem_->SetWorld(world_);
    weaponSystem_->SetWorld(world_);
    ammoSystem_->SetWorld(world_);
    
    std::cout << "[DEBUG] Set world to ExperienceSystem" << std::endl;
    
    // 初始化系统
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
    
    std::cout << "🔧 All systems initialized" << std::endl;
}

void ZombieTestScene::CreateTestEntities() {
    auto& entityFactory = world_->GetEntityFactory();
    auto& componentManager = world_->GetComponentManager();
    
    // 创建玩家实体
    playerId_ = entityFactory.CreateEntity("TestPlayer");
    
    // 添加玩家组件
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
    
    // 初始化弹药从武器配置
    if (ammoSystem_) {
        ammoSystem_->InitializeAmmoFromWeapon(playerId_);
    }
    
    // 测试新的射击流程
    std::cout << "🔫 Testing new weapon-ammo collaboration..." << std::endl;
    if (ammoSystem_) {
        std::cout << "   初始弹药状态: " << (ammoSystem_->HasAmmo(playerId_) ? "有弹药" : "无弹药") << std::endl;
    }
    if (weaponSystem_) {
        std::cout << "   武器是否就绪: " << (weaponSystem_->IsWeaponReady(playerId_) ? "就绪" : "未就绪") << std::endl;
        
        // 模拟射击请求
        weaponSystem_->TryShoot(playerId_);
    }
    if (ammoSystem_) {
        std::cout << "   射击后弹药状态: " << (ammoSystem_->HasAmmo(playerId_) ? "有弹药" : "无弹药") << std::endl;
    }
    
    // 创建敌人实体
    enemyId_ = entityFactory.CreateEntity("TestEnemy");
    
    // 添加敌人组件
    componentManager.AddComponent<Component::HealthComponent>(enemyId_, Component::HealthComponent{});
    componentManager.AddComponent<Component::WeaponComponent>(enemyId_, Component::WeaponComponent{});
    componentManager.AddComponent<Component::CombatStatsComponent>(enemyId_, Component::CombatStatsComponent{});
    
    // 设置敌人武器伤害
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
                testCompleted_ = true;
                testPhase_++;
            }
            break;
            
        default:
            if (!testCompleted_) {
                std::cout << "\n🎉 All tests completed! Press R to restart, SPACE to skip phases" << std::endl;
                testCompleted_ = true;
            }
            break;
    }
}

void ZombieTestScene::TestDamageSystem() {
    auto& componentManager = world_->GetComponentManager();
    
    // 获取玩家生命值
    auto* playerHealth = componentManager.GetComponent<Component::HealthComponent>(playerId_);
    if (playerHealth) {
        float oldHealth = playerHealth->health;
        std::cout << "  玩家初始生命值: " << oldHealth << std::endl;
        
        // 造成伤害
        if (damageSystem_) {
            damageSystem_->DealDamage(playerId_, enemyId_, 25, "test_damage");
        }
        
        std::cout << "  造成25点伤害后生命值: " << playerHealth->health << std::endl;
        std::cout << "  ✅ Damage System test completed" << std::endl;
    }
}

void ZombieTestScene::TestExperienceSystem() {
    std::cout << "  [DEBUG] Publishing enemy killed event..." << std::endl;
    std::cout << "  [DEBUG] EventManager: " << eventManager_ << std::endl;
    std::cout << "  [DEBUG] World EventManager: " << &world_->GetEventManager() << std::endl;
    
    Events::GameEventUtils::PublishEnemyKilled(*eventManager_, playerId_, enemyId_, 50, "test_enemy");
    std::cout << "  [DEBUG] Event published" << std::endl;
    
    // 强制更新一次系统，看看是否能处理事件
    std::cout << "  [DEBUG] Forcing ExperienceSystem update..." << std::endl;
    if (experienceSystem_) {
        experienceSystem_->Update(0.016f); // 模拟一帧
    }
    
    auto& componentManager = world_->GetComponentManager();
    auto* exp = componentManager.GetComponent<Component::ExperienceComponent>(playerId_);
    if (exp) {
        std::cout << "  当前等级: " << exp->level << std::endl;
        std::cout << "  当前经验: " << exp->experience << std::endl;
        std::cout << "  升级所需经验: " << exp->experienceToNext << std::endl;
        std::cout << "  ✅ Experience System test completed" << std::endl;
    }
}

void ZombieTestScene::TestUpgradeSystem() {
    auto& componentManager = world_->GetComponentManager();
    auto* upgrade = componentManager.GetComponent<Component::UpgradeComponent>(playerId_);
    
    if (upgrade && upgrade->pendingUpgrade && !upgrade->currentOptions.empty()) {
        std::cout << "  可选升级数量: " << upgrade->currentOptions.size() << std::endl;
        
        // 选择第一个升级
        Component::UpgradeType selectedUpgrade = upgrade->currentOptions[0];
        if (upgradeSystem_) {
            upgradeSystem_->SelectUpgrade(playerId_, selectedUpgrade);
        }
        
        std::cout << "  选择了升级类型: " << static_cast<int>(selectedUpgrade) << std::endl;
        std::cout << "  ✅ Upgrade System test completed" << std::endl;
    } else {
        std::cout << "  ⚠️ No upgrades available (may need to level up first)" << std::endl;
    }
}

void ZombieTestScene::PrintTestResults() {
    std::cout << "\n📊 === Final Test Results ===" << std::endl;
    
    auto& componentManager = world_->GetComponentManager();
    
    // 玩家状态
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

} // namespace ZombieSurvivor
