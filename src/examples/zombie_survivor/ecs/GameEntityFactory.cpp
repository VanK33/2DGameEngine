// src/examples/zombie_survivor/ecs/GameEntityFactory.cpp
#include "GameEntityFactory.hpp"
#include "engine/core/ecs/components/PhysicsMode.hpp"
#include "examples/zombie_survivor/ecs/components/InputComponent.hpp"
#include "examples/zombie_survivor/ecs/components/MovementComponent.hpp"
#include "examples/zombie_survivor/ecs/components/FollowComponent.hpp"
#include "examples/zombie_survivor/ecs/components/AimingComponent.hpp"
#include <iostream>

namespace ZombieSurvivor::ECS {

GameEntityFactory::GameEntityFactory(engine::ECS::World* world, engine::resources::ResourceManager* resourceManager) 
    : world_(world), resourceManager_(resourceManager) {}

uint32_t GameEntityFactory::CreatePlayer(const engine::Vector2& position) {
    if (!ValidateWorld()) return 0;
    
    // Load texture for player sprite
    if (resourceManager_) {
        resourceManager_->LoadTexture("pixel.png");
    }
    
    // ✅ 使用现有的 EntityFactory 获取ID
    auto& entityFactory = world_->GetEntityFactory();
    auto& componentManager = world_->GetComponentManager();
    
    uint32_t playerId = entityFactory.CreateEntity("Player");
    
    // 添加Transform组件 - center of screen
    componentManager.AddComponent<engine::ECS::Transform2D>(playerId,
        engine::ECS::Transform2D{position.x, position.y, 0.0f, 1.0f, 1.0f});
    
    // 添加Sprite组件 - large WHITE square for testing (no color tint)
    componentManager.AddComponent<engine::ECS::Sprite2D>(playerId,
        engine::ECS::Sprite2D{
            "pixel.png",                    // 16x16 white texture
            {0, 0, 64, 64},                 // 64x64 pixel display
            true,                           // visible
            {255, 0, 0, 255},               // RED tint to make visible
            ToInt(RenderLayer::ENTITIES)    // ENTITIES layer (10) - player should be below weapon
        });
    
    // 添加s碰撞组件
    componentManager.AddComponent<engine::ECS::Collider2D>(playerId,
        engine::ECS::Collider2D{
            {-16, -16, 32, 32},    // 32x32碰撞框
            false,                  // 不是触发器
            "player"               // 玩家层
        });
    
    // 添加移动组件
    componentManager.AddComponent<engine::ECS::Velocity2D>(playerId,
        engine::ECS::Velocity2D{0.0f, 0.0f, 200.0f});  // 最大速度200

    // 添加物理模式组件（top-down 2D游戏）
    componentManager.AddComponent<engine::ECS::PhysicsModeComponent>(playerId,
        engine::ECS::PhysicsModeComponent{
            engine::ECS::PhysicsMode::TOP_DOWN,
            0.0f,    // gravityX
            0.0f,    // gravityY  
            0.0f,    // gravityZ
            false,   // enableGravity
            true,    // enableFriction
            0.98f    // frictionFactor
        });
    
    componentManager.AddComponent<ZombieSurvivor::Component::InputComponent>(playerId,
        ZombieSurvivor::Component::InputComponent{
            {0.0f, 0.0f},
            {0.0f, 0.0f},
            false,
            false,
            -1
        });
    
    componentManager.AddComponent<ZombieSurvivor::Component::MovementComponent>(playerId,
        ZombieSurvivor::Component::MovementComponent{
            150.0f,  // speed
            800.0f,  // acceleration  
            false,   // instantResponse
            true     // canMove
        });
      
    
    std::cout << "[GameEntityFactory] Created player entity: " << playerId << std::endl;
    return playerId;
}

uint32_t GameEntityFactory::CreateWeapon(engine::EntityID playerEntityId, const engine::Vector2& offset) {
    if (!ValidateWorld()) return 0;
    
    auto& componentManager = world_->GetComponentManager();
    auto weaponId = world_->GetEntityFactory().CreateEntity();
    
    // Get player position for initial weapon position
    auto* playerTransform = componentManager.GetComponent<engine::ECS::Transform2D>(playerEntityId);
    if (!playerTransform) {
        std::cout << "[GameEntityFactory] ERROR: Player entity not found for weapon creation" << std::endl;
        return 0;
    }
    
    // Add Transform2D component at player position + offset
    componentManager.AddComponent<engine::ECS::Transform2D>(weaponId,
        engine::ECS::Transform2D{
            playerTransform->x + offset.x,
            playerTransform->y + offset.y,
            0.0f,  // rotation
            1.0f,  // scaleX
            1.0f   // scaleY
        });
    
    // Add Sprite2D component for visual representation
    componentManager.AddComponent<engine::ECS::Sprite2D>(weaponId,
        engine::ECS::Sprite2D{
            "pixel.png",                  // texturePath
            {0, 0, 40, 12},               // sourceRect - 40x12 weapon sprite (bigger)
            true,                         // visible
            {0, 255, 0, 255},             // tint - green to make it visible
            ToInt(RenderLayer::WEAPON),   // renderLayer - convert enum to int
            {1.0f, 0.5f}                  // pivotOffset - right edge center for clock hand behavior
        });
    
    // Add FollowComponent to make weapon follow player
    componentManager.AddComponent<Component::FollowComponent>(weaponId,
        Component::FollowComponent{
            playerEntityId,  // follow the player
            offset,          // maintain offset
            false,           // don't follow player rotation
            50.0f            // follow distance
        });
    
    // Add InputComponent for AimingSystem compatibility (will be updated by WeaponFollowSystem)
    componentManager.AddComponent<Component::InputComponent>(weaponId,
        Component::InputComponent{
            {0.0f, 0.0f},    // moveInput (not used)
            {0.0f, 0.0f},    // mousePosition (will be copied from player)
            false,           // shootButtonPressed (not used)
            false,           // reloadButtonPressed (not used)
            -1               // targetEntityId (not used)
        });
    
    // Add AimingComponent for mouse-based rotation
    componentManager.AddComponent<Component::AimingComponent>(weaponId,
        Component::AimingComponent{
            {1.0f, 0.0f},    // aimDirection (pointing right initially)
            {0.0f, 0.0f},    // mouseWorldPos
            true,            // showAimLine
            250.0f           // maxAimRange
        });
    
    std::cout << "[GameEntityFactory] Created weapon entity: " << weaponId << " for player: " << playerEntityId << std::endl;
    return weaponId;
}

uint32_t GameEntityFactory::SpawnZombie(const engine::Vector2& position) {
    if (!ValidateWorld()) return 0;
    
    auto& entityFactory = world_->GetEntityFactory();
    auto& componentManager = world_->GetComponentManager();
    
    uint32_t zombieId = entityFactory.CreateEntity("Zombie");
    
    // 添加组件...
    componentManager.AddComponent<engine::ECS::Transform2D>(zombieId,
        engine::ECS::Transform2D{position.x, position.y, 0.0f, 1.0f, 1.0f});
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(zombieId,
        engine::ECS::Sprite2D{
            "pixel.png",
            {0, 0, 30, 30},
            true,
            {255, 0, 0, 255},              
            ToInt(RenderLayer::ENTITIES)
        });
    
    std::cout << "[GameEntityFactory] Spawned zombie: " << zombieId << std::endl;
    return zombieId;
}

uint32_t GameEntityFactory::CreateProjectile(const engine::Vector2& position, 
                                            const engine::Vector2& direction, 
                                            float speed) {
    if (!ValidateWorld()) return 0;
    
    auto& entityFactory = world_->GetEntityFactory();
    auto& componentManager = world_->GetComponentManager();
    
    uint32_t projectileId = entityFactory.CreateEntity("Projectile");
    
    // 添加组件...
    componentManager.AddComponent<engine::ECS::Transform2D>(projectileId,
        engine::ECS::Transform2D{position.x, position.y, 0.0f, 1.0f, 1.0f});
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(projectileId,
        engine::ECS::Sprite2D{
            "pixel.png",
            {0, 0, 4, 4},
            true,
            {255, 255, 0, 255},           
            ToInt(RenderLayer::ENTITIES)
        });
    
    engine::Vector2 velocity = direction * speed;
    componentManager.AddComponent<engine::ECS::Velocity2D>(projectileId,
        engine::ECS::Velocity2D{velocity.x, velocity.y, speed});
    
    std::cout << "[GameEntityFactory] Created projectile: " << projectileId << std::endl;
    return projectileId;
}

bool GameEntityFactory::ValidateWorld() const {
    return world_ != nullptr;
}

} // namespace ZombieSurvivor::ECS
