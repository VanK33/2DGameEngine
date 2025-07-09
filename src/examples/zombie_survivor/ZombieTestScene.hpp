#pragma once

#include "engine/core/scene/Scene.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "engine/input/InputManager.hpp"

// 系统包含
#include "ecs/systems/HealthSystem.hpp"
#include "ecs/systems/DamageSystem.hpp"
#include "ecs/systems/ExperienceSystem.hpp"
#include "ecs/systems/UpgradeSystem.hpp"
#include "ecs/systems/InputSystem.hpp"
#include "ecs/systems/MovementSystem.hpp"
#include "ecs/systems/WeaponSystem.hpp"
#include "ecs/systems/AmmoSystem.hpp"

// 组件包含
#include "ecs/components/HealthComponent.hpp"
#include "ecs/components/ExperienceComponent.hpp"
#include "ecs/components/UpgradeComponent.hpp"
#include "ecs/components/WeaponComponent.hpp"
#include "ecs/components/AmmoComponent.hpp"
#include "ecs/components/MovementComponent.hpp"
#include "ecs/components/InputComponent.hpp"
#include "ecs/components/CombatStatsComponent.hpp"

// 事件包含
#include "events/GameEventUtils.hpp"

#include <memory>

namespace ZombieSurvivor {

class ZombieTestScene : public engine::scene::Scene {
public:
    ZombieTestScene(const std::string& id, SDL_Renderer* renderer, 
                   engine::input::InputManager* inputManager,
                   engine::event::EventManager* eventManager);
    ~ZombieTestScene() = default;

    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    void HandleEvent(const SDL_Event& event) override;
    std::string GetSceneId() const override;
    
    // Scene 纯虚方法实现
    void SetEventManager(engine::event::EventManager* manager) override;
    void SetInputManager(engine::input::InputManager* manager) override;

private:
    std::string sceneId_;
    engine::input::InputManager* inputManager_;
    engine::event::EventManager* eventManager_;
    
    // ECS World
    engine::ECS::World* world_; 
    
    // 系统
    std::unique_ptr<System::HealthSystem> healthSystem_;
    std::unique_ptr<System::DamageSystem> damageSystem_;
    std::unique_ptr<System::ExperienceSystem> experienceSystem_;
    std::unique_ptr<System::UpgradeSystem> upgradeSystem_;
    std::unique_ptr<System::InputSystem> inputSystem_;
    std::unique_ptr<System::MovementSystem> movementSystem_;
    std::unique_ptr<System::WeaponSystem> weaponSystem_;
    std::unique_ptr<System::AmmoSystem> ammoSystem_;
    
    // 测试实体
    uint32_t playerId_;
    uint32_t enemyId_;
    
    // 测试状态
    float testTimer_;
    int testPhase_;
    bool testCompleted_;
    
    void InitializeSystems();
    void CreateTestEntities();
    void RunSystemTests(float deltaTime);
    void TestDamageSystem();
    void TestExperienceSystem();
    void TestUpgradeSystem();
    void PrintTestResults();

    bool experienceTestPending_ = false;
};

} // namespace ZombieSurvivor
