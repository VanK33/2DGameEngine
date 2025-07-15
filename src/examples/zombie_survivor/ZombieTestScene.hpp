// #pragma once

// #include "engine/core/scene/Scene.hpp"
// #include "engine/core/ecs/World.hpp"
// #include "engine/core/event/EventManager.hpp"
// #include "engine/input/InputManager.hpp"

// // System includes
// #include "ecs/systems/HealthSystem.hpp"
// #include "ecs/systems/DamageSystem.hpp"
// #include "ecs/systems/ExperienceSystem.hpp"
// #include "ecs/systems/UpgradeSystem.hpp"
// #include "ecs/systems/InputSystem.hpp"
// #include "ecs/systems/MovementSystem.hpp"
// #include "ecs/systems/WeaponSystem.hpp"
// #include "ecs/systems/AmmoSystem.hpp"
// #include "ecs/systems/AimingSystem.hpp"
// #include "ecs/systems/RotationSystem.hpp"
// #include "ecs/systems/WeaponFireSystem.hpp"
// #include "ecs/systems/WeaponInputSystem.hpp"
// #include "ecs/systems/PlayerStatsSystem.hpp"
// #include "ecs/systems/EnemySpawnSystem.hpp"
// #include "ecs/systems/ProjectileSystem.hpp"

// // Component includes
// #include "ecs/components/HealthComponent.hpp"
// #include "ecs/components/ExperienceComponent.hpp"
// #include "ecs/components/UpgradeComponent.hpp"
// #include "ecs/components/WeaponComponent.hpp"
// #include "ecs/components/AmmoComponent.hpp"
// #include "ecs/components/MovementComponent.hpp"
// #include "ecs/components/InputComponent.hpp"
// #include "ecs/components/CombatStatsComponent.hpp"
// #include "ecs/components/AimingComponent.hpp"
// #include "ecs/components/PlayerStatsComponent.hpp"

// // Event includes
// #include "events/GameEventUtils.hpp"

// #include <memory>

// namespace ZombieSurvivor {

// class ZombieTestScene : public engine::scene::Scene {
// public:
//     ZombieTestScene(const std::string& id, SDL_Renderer* renderer, 
//                    engine::input::InputManager* inputManager,
//                    engine::event::EventManager* eventManager);
//     ~ZombieTestScene() = default;

//     void Load() override;
//     void Unload() override;
//     void Update(float deltaTime) override;
//     void Render(SDL_Renderer* renderer) override;
//     void HandleEvent(const SDL_Event& event) override;
//     std::string GetSceneId() const override;
    
//     // Scene pure virtual method implementations
//     void SetEventManager(engine::event::EventManager* manager) override;
//     void SetInputManager(engine::input::InputManager* manager) override;

// private:
//     std::string sceneId_;
//     engine::input::InputManager* inputManager_;
//     engine::event::EventManager* eventManager_;
    
//     // ECS World
//     engine::ECS::World* world_; 
    
//     // Systems
//     std::unique_ptr<System::HealthSystem> healthSystem_;
//     std::unique_ptr<System::DamageSystem> damageSystem_;
//     std::unique_ptr<System::ExperienceSystem> experienceSystem_;
//     std::unique_ptr<System::UpgradeSystem> upgradeSystem_;
//     std::unique_ptr<System::InputSystem> inputSystem_;
//     std::unique_ptr<System::MovementSystem> movementSystem_;
//     std::unique_ptr<System::WeaponSystem> weaponSystem_;
//     std::unique_ptr<System::AmmoSystem> ammoSystem_;
    
//     // Test entities
//     uint32_t playerId_;
//     uint32_t enemyId_;
    
//     // Test state
//     float testTimer_;
//     int testPhase_;
//     bool testCompleted_;
    
//     void InitializeSystems();
//     void CreateTestEntities();
//     void RunSystemTests(float deltaTime);
//     void TestDamageSystem();
//     void TestExperienceSystem();
//     void TestUpgradeSystem();
//     void PrintTestResults();

//     bool experienceTestPending_ = false;

//     // New systems
//     std::unique_ptr<System::AimingSystem> aimingSystem_;
//     std::unique_ptr<System::RotationSystem> rotationSystem_;
//     std::unique_ptr<System::WeaponFireSystem> weaponFireSystem_;
//     std::unique_ptr<System::WeaponInputSystem> weaponInputSystem_;
//     std::unique_ptr<System::PlayerStatsSystem> playerStatsSystem_;
//     std::unique_ptr<System::EnemySpawnSystem> enemySpawnSystem_;
//     std::unique_ptr<System::ProjectileSystem> projectileSystem_;

//     // New test methods
//     void TestAimingAndRotation();
//     void TestWeaponFireSystem();
//     void TestReloadSystem();
//     void TestPlayerStatsSystem();
//     void TestEnemySpawnSystem();
//     void TestProjectileSystem();
// };

// } // namespace ZombieSurvivor
