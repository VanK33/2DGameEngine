// src/examples/zombie_survivor/ecs/GameEntityFactory.cpp
#include "GameEntityFactory.hpp"
#include "engine/core/ecs/components/PhysicsMode.hpp"
#include "engine/core/ecs/components/Tag.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "engine/core/ecs/components/Collider2D.hpp"
#include "engine/core/ecs/components/AIComponent.hpp"
#include "examples/zombie_survivor/ecs/components/InputComponent.hpp"
#include "examples/zombie_survivor/ecs/components/MovementComponent.hpp"
#include "examples/zombie_survivor/ecs/components/BoundaryComponent.hpp"
#include "examples/zombie_survivor/ecs/components/FollowComponent.hpp"
#include "examples/zombie_survivor/ecs/components/AimingComponent.hpp"
#include "examples/zombie_survivor/ecs/components/WeaponComponent.hpp"
#include "examples/zombie_survivor/ecs/components/AmmoComponent.hpp"
#include "examples/zombie_survivor/ecs/components/HUDComponent.hpp"
#include "examples/zombie_survivor/ecs/components/HealthComponent.hpp"
#include "examples/zombie_survivor/ecs/components/ExperienceComponent.hpp"
#include "examples/zombie_survivor/ecs/components/EnemyComponent.hpp"
#include "examples/zombie_survivor/ecs/components/CombatStatsComponent.hpp"
#include "examples/zombie_survivor/ecs/components/TargetComponent.hpp"
#include "examples/zombie_survivor/ecs/RenderLayer.hpp"
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
    
    auto& entityFactory = world_->GetEntityFactory();
    auto& componentManager = world_->GetComponentManager();
    
    uint32_t playerId = entityFactory.CreateEntity("Player");
    
    componentManager.AddComponent<engine::ECS::Transform2D>(playerId,
        engine::ECS::Transform2D{position.x, position.y, 0.0f, 1.0f, 1.0f});
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(playerId,
        engine::ECS::Sprite2D{
            "pixel.png",                    // 16x16 white texture
            {0, 0, 64, 64},                 // 64x64 pixel display
            true,                           // visible
            {255, 0, 0, 255},               // RED tint to make visible
            ToInt(RenderLayer::ENTITIES),   // ENTITIES layer (10) - player should be below weapon
            {0.5f, 0.5f}                    // pivotOffset: center of sprite for proper rotation
        });
    
    componentManager.AddComponent<engine::ECS::Collider2D>(playerId,
        engine::ECS::Collider2D{
            {-32, -32, 64, 64},    // 64x64碰撞框，中心对齐
            false,                  // 不是触发器
            "player"               // 玩家层
        });
    
    componentManager.AddComponent<engine::ECS::Velocity2D>(playerId,
        engine::ECS::Velocity2D{0.0f, 0.0f, 250.0f}); 

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
    
    // Add AimingComponent to player for mouse-based rotation
    componentManager.AddComponent<Component::AimingComponent>(playerId,
        Component::AimingComponent{
            {1.0f, 0.0f},    // aimDirection (pointing right initially)
            {0.0f, 0.0f},    // mouseWorldPos
            true,            // showAimLine
            250.0f           // maxAimRange
        });
    
    // TODO: 这里需要修复一下,应该是从config读取而不是hardcode
    // Add WeaponComponent to player for WeaponInputSystem to find
    componentManager.AddComponent<ZombieSurvivor::Component::WeaponComponent>(playerId,
        ZombieSurvivor::Component::WeaponComponent{
            ZombieSurvivor::Component::WeaponType::PISTOL,  // type
            15.0f,   // damage
            250.0f,  // range
            3.0f,    // fireRate
            2.0f,    // reloadTime
            false,   // isReloading
            0.0f,    // lastFireTime
            12,      // magazineCapacity
            120,     // defaultTotalAmmo
            300,     // maxTotalAmmo
            ZombieSurvivor::Component::AmmoType::PISTOL     // currentAmmoType
        });
    
    // Add AmmoComponent for ammunition management
    componentManager.AddComponent<ZombieSurvivor::Component::AmmoComponent>(playerId,
        ZombieSurvivor::Component::AmmoComponent{
            12,     // currentAmmo (matches weapon magazine capacity)
            120,    // totalAmmo
            300,    // maxTotalAmmo
            false,  // isReloading
            0.0f    // reloadProgress
        });

    // Add HealthComponent for health
    componentManager.AddComponent<ZombieSurvivor::Component::HealthComponent>(playerId,
        ZombieSurvivor::Component::HealthComponent{
            100.0f,  // health
            100.0f,  // maxHealth  
            true     // isAlive
        });
    
    componentManager.AddComponent<ZombieSurvivor::Component::ExperienceComponent>(playerId,
        ZombieSurvivor::Component::ExperienceComponent{
            1,     // level
            0,     // experience
            100,   // experienceToNext
            0,     // skillPoints
            false  // canLevelUp
        });
    
    auto* healthCheck = componentManager.GetComponent<ZombieSurvivor::Component::HealthComponent>(playerId);
    auto* expCheck = componentManager.GetComponent<ZombieSurvivor::Component::ExperienceComponent>(playerId);
    
    if (healthCheck) {
        std::cout << "[GameEntityFactory] Player " << playerId << " health: " 
                  << healthCheck->health << "/" << healthCheck->maxHealth << std::endl;
    }
    
    if (expCheck) {
        std::cout << "[GameEntityFactory] Player " << playerId << " experience: " 
                  << expCheck->experience << "/" << expCheck->experienceToNext 
                  << " (Level " << expCheck->level << ")" << std::endl;
    }
    
    // Adding "Player" Tag to player
    componentManager.AddComponent<engine::ECS::Tag>(playerId, 
        engine::ECS::Tag{"player"});
    
    // Add boundary constraint for screen bounds
    componentManager.AddComponent<ZombieSurvivor::Component::BoundaryComponent>(playerId,
        ZombieSurvivor::Component::BoundaryComponent{
            ZombieSurvivor::Component::BoundaryType::SCREEN_BOUNDS,  // Screen boundaries
            64.0f,  // Entity size (64x64 pixels)
            0.0f, 850.0f, 0.0f, 850.0f,  // Game world boundary parameters (850x850)
            true    // Enable boundary constraint
        });
    
    // DEBUG: Verify ammo was set correctly
    auto* ammoCheck = componentManager.GetComponent<ZombieSurvivor::Component::AmmoComponent>(playerId);
    if (ammoCheck) {
        std::cout << "[GameEntityFactory] Player " << playerId << " ammo after creation: " 
                  << ammoCheck->currentAmmo << "/" << ammoCheck->totalAmmo << " (max: " << ammoCheck->maxTotalAmmo << ")" << std::endl;
    } else {
        std::cout << "[GameEntityFactory] ERROR: Player " << playerId << " has no AmmoComponent after creation!" << std::endl;
    }
    
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
            {0.25f, 0.5f}                 // pivotOffset - grip point at 1/4 from left edge, vertical center
        });
    
    // Add FollowComponent to make weapon follow player (position AND rotation)
    componentManager.AddComponent<Component::FollowComponent>(weaponId,
        Component::FollowComponent{
            playerEntityId,  // follow the player
            offset,          // maintain offset
            true,            // follow player rotation (NEW: synchronized rotation)
            50.0f            // follow distance
        });
    
    // Add InputComponent and AimingComponent to weapon for independent aiming
    componentManager.AddComponent<Component::InputComponent>(weaponId,
        Component::InputComponent{
            {0.0f, 0.0f},    // movementInput
            {0.0f, 0.0f},    // mousePosition (will be copied from player)
            false,           // leftMousePressed
            false,           // rightMousePressed
            -1               // lastInputFrame
        });
    
    componentManager.AddComponent<Component::AimingComponent>(weaponId,
        Component::AimingComponent{
            {1.0f, 0.0f},    // aimDirection (pointing right initially)
            {0.0f, 0.0f},    // mouseWorldPos
            true,            // showAimLine
            250.0f           // maxAimRange
        });
    
    // Add WeaponComponent for firing system
    componentManager.AddComponent<Component::WeaponComponent>(weaponId,
        Component::WeaponComponent{
            Component::WeaponType::PISTOL,  // type
            15.0f,                          // damage
            250.0f,                         // range
            3.0f,                           // fireRate
            2.0f,                           // reloadTime
            false,                          // isReloading
            0.0f,                           // lastFireTime
            12,                             // magazineCapacity
            120,                            // defaultTotalAmmo
            300,                            // maxTotalAmmo
            Component::AmmoType::PISTOL     // currentAmmoType
        });
    
    // Add "Weapon" tag for weapon
    componentManager.AddComponent<engine::ECS::Tag>(weaponId, 
        engine::ECS::Tag{"weapon"});
    
    std::cout << "[GameEntityFactory] Created weapon entity: " << weaponId << " for player: " << playerEntityId << std::endl;
    return weaponId;
}

uint32_t GameEntityFactory::CreatePlayerHUD(engine::EntityID playerEntityId) {
    std::cout << "[GameEntityFactory] *** CreatePlayerHUD called for player " << playerEntityId << " ***" << std::endl;
    
    if (!ValidateWorld()) {
        std::cout << "[GameEntityFactory] ERROR: World validation failed!" << std::endl;
        return 0;
    }
    
    std::cout << "[GameEntityFactory] World validation passed" << std::endl;
    
    if (!ValidateWorld()) return 0;

    auto& entityFactory = world_->GetEntityFactory();
    auto& componentManager = world_->GetComponentManager();

    engine::EntityID healthHudId = entityFactory.CreateEntity("PlayerHealthHUD");

    Component::HUDComponent healthHUD; 
    healthHUD.type = Component::HUDElementType::HEALTH_BAR;
    healthHUD.position = Component::HUDPosition::CUSTOM;
    healthHUD.bounds = {231, 935, 120, 6};  // 游戏窗口左下角：x=231, y=905+30=935 (向下移动30px)
    healthHUD.visible = true;
    healthHUD.renderLayer = ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::UI);

    healthHUD.targetEntityId = playerEntityId;
    healthHUD.componentProperty = "health";

    healthHUD.backgroundColor = {80, 80, 80, 120};
    healthHUD.foregroundColor = {255, 0, 0, 255};     // 红色前景色
    healthHUD.criticalColor = {255, 100, 100, 255};   // 危险时稍亮的红色
    healthHUD.criticalThreshold = 0.25f;   

    auto* playerHealth = componentManager.GetComponent<Component::HealthComponent>(playerEntityId);
    if (playerHealth) {
        healthHUD.currentValue = playerHealth->health;
        healthHUD.maxValue = playerHealth->maxHealth;
    } else {
        healthHUD.currentValue = 100.0f;
        healthHUD.maxValue = 100.0f;
    }

    healthHUD.showNumbers = true;
    healthHUD.showPercentage = false;
    healthHUD.textFormat = "{0}/{1}";

    healthHUD.animateChanges = true;
    healthHUD.animationSpeed = 50.0f;

    componentManager.AddComponent<Component::HUDComponent>(healthHudId, healthHUD);
    componentManager.AddComponent<engine::ECS::Tag>(healthHudId, 
        engine::ECS::Tag{"player_health_hud"});
    
    std::cout << "[GameEntityFactory] Created player health HUD: " << healthHudId << std::endl;

    engine::EntityID expHudId = entityFactory.CreateEntity("PlayerExperienceHUD");
    
    Component::HUDComponent expHUD;
    expHUD.type = Component::HUDElementType::EXPERIENCE_BAR;
    expHUD.position = Component::HUDPosition::CUSTOM;
    expHUD.bounds = {231, 927, 120, 6};  // 游戏窗口左下角血量条上方：x=231, y=935-8=927 (向下移动30px)
    expHUD.visible = true;
    expHUD.renderLayer = ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::UI);
    
    expHUD.targetEntityId = playerEntityId;
    expHUD.componentProperty = "experience";
    
    expHUD.backgroundColor = {80, 80, 80, 120};        // 半透明灰色（点阵背景效果）
    expHUD.foregroundColor = {0, 100, 255, 255};       // 蓝色前景（经验条）
    expHUD.criticalColor = {100, 150, 255, 255};       // 接近升级时稍亮的蓝色
    expHUD.criticalThreshold = 0.9f;                   // 90%以上显示亮色
    

    auto* playerExp = componentManager.GetComponent<Component::ExperienceComponent>(playerEntityId);
    if (playerExp) {
        expHUD.currentValue = static_cast<float>(playerExp->experience);
        expHUD.maxValue = static_cast<float>(playerExp->experienceToNext);
    } else {
        expHUD.currentValue = 0.0f;
        expHUD.maxValue = 100.0f;
    }

    expHUD.showNumbers = true;
    expHUD.showPercentage = true;      // 也显示百分比
    expHUD.textFormat = "{0}/{1} ({2}%)";  // "30/100 (30%)" 格式
    
    expHUD.animateChanges = true;
    expHUD.animationSpeed = 25.0f;
    
    componentManager.AddComponent<Component::HUDComponent>(expHudId, expHUD);
    componentManager.AddComponent<engine::ECS::Tag>(expHudId, 
        engine::ECS::Tag{"player_experience_hud"});
    
    std::cout << "[GameEntityFactory] Created player experience HUD: " << expHudId << std::endl;

    // Create Ammo Counter HUD (positioned in bottom-right area)
    engine::EntityID ammoHudId = entityFactory.CreateEntity("PlayerAmmoHUD");
    
    Component::HUDComponent ammoHUD;
    ammoHUD.type = Component::HUDElementType::AMMO_COUNTER;
    ammoHUD.position = Component::HUDPosition::CUSTOM;
    ammoHUD.bounds = {1081, 935, 120, 6};  // 游戏窗口右下角：x=1081 (右边界), y=905+30=935 (右对齐，向左生长，向下移动30px)
    ammoHUD.visible = true;
    ammoHUD.renderLayer = ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::UI);
    
    ammoHUD.targetEntityId = playerEntityId;
    ammoHUD.componentProperty = "ammo";
    
    ammoHUD.backgroundColor = {80, 80, 80, 120};        // 半透明灰色背景
    ammoHUD.foregroundColor = {255, 255, 255, 255};     // 白色文字
    ammoHUD.criticalColor = {255, 165, 0, 255};         // 弹药不足时橙色警告
    ammoHUD.criticalThreshold = 0.25f;                  // 25%以下显示警告色
    
    // Set ammo values from player's ammo component
    auto* playerAmmo = componentManager.GetComponent<Component::AmmoComponent>(playerEntityId);
    if (playerAmmo) {
        ammoHUD.currentValue = static_cast<float>(playerAmmo->currentAmmo);
        ammoHUD.maxValue = static_cast<float>(playerAmmo->totalAmmo);
    } else {
        ammoHUD.currentValue = 12.0f;  // Default pistol ammo
        ammoHUD.maxValue = 120.0f;     // Default total ammo
    }
    
    ammoHUD.showNumbers = true;
    ammoHUD.showPercentage = false;
    ammoHUD.textFormat = "Ammo: {0}/{1}";  // "Ammo: 12/120" format
    
    ammoHUD.animateChanges = true;
    ammoHUD.animationSpeed = 15.0f;
    
    componentManager.AddComponent<Component::HUDComponent>(ammoHudId, ammoHUD);
    componentManager.AddComponent<engine::ECS::Tag>(ammoHudId, 
        engine::ECS::Tag{"player_ammo_hud"});
    
    std::cout << "[GameEntityFactory] Created player ammo HUD: " << ammoHudId 
              << " at position (" << ammoHUD.bounds.x << ", " << ammoHUD.bounds.y << ")" << std::endl;
    
    std::cout << "[GameEntityFactory] All HUD elements created successfully!" << std::endl;
    std::cout << "[GameEntityFactory] Health HUD created with bounds: " 
    << healthHUD.bounds.x << "," << healthHUD.bounds.y << " " 
    << healthHUD.bounds.w << "x" << healthHUD.bounds.h << std::endl;
    std::cout << "[GameEntityFactory] Health HUD values: " 
    << healthHUD.currentValue << "/" << healthHUD.maxValue << std::endl;
    return healthHudId;
}

uint32_t GameEntityFactory::CreateZombie(const engine::Vector2& position) {
    if (!ValidateWorld()) return 0;
    
    auto& entityFactory = world_->GetEntityFactory();
    auto& componentManager = world_->GetComponentManager();
    
    engine::EntityID zombie = entityFactory.CreateEntity("Zombie");
    
    componentManager.AddComponent<engine::ECS::Transform2D>(zombie, 
        engine::ECS::Transform2D{position.x, position.y, 0.0f, 1.0f, 1.0f});
    componentManager.AddComponent<engine::ECS::Velocity2D>(zombie, 
        engine::ECS::Velocity2D{0.0f, 0.0f, 100.0f});
    
    // Add PhysicsModeComponent for PhysicsSystem to process movement
    componentManager.AddComponent<engine::ECS::PhysicsModeComponent>(zombie,
        engine::ECS::PhysicsModeComponent{
            engine::ECS::PhysicsMode::TOP_DOWN,  // 2D physics mode
            0.0f, 0.0f, 0.0f,                    // no gravity
            false,                               // disable gravity
            false,                               // disable friction for smooth movement
            1.0f                                 // no friction factor
        });
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(zombie, 
        engine::ECS::Sprite2D{
            "pixel.png",                                           // texturePath - use same texture as player
            {0, 0, 30, 30},                                        // sourceRect
            true,                                                   // visible
            {0, 255, 0, 255},                                      // tint (green for zombie)
            ECS::ToInt(ECS::RenderLayer::ENTITIES),                // renderLayer
            {0.5f, 0.5f}                                           // pivotOffset (use center)
        });
    
    componentManager.AddComponent<engine::ECS::Collider2D>(zombie, 
        engine::ECS::Collider2D{{-15, -15, 30, 30}, false, "enemy"});
    
    componentManager.AddComponent<engine::ECS::AIComponent>(zombie, 
        engine::ECS::AIComponent{
            engine::ECS::AIState::ACTIVE,  // State
            0,                              // targetEntity (will be set by AI system)
            {0, 0},                        // targetPosition
            50.0f,                         // speed
            150.0f,                        // detectionRadius
            0.0f,                          // updateTimer
            0.1f                           // updateInterval
        });
    
    componentManager.AddComponent<Component::EnemyComponent>(zombie, Component::EnemyComponent{});
    auto* enemy = componentManager.GetComponent<Component::EnemyComponent>(zombie);
    if (enemy) {
        enemy->type = Component::EnemyType::ZOMBIE_BASIC;
        enemy->damage = 10.0f;
        enemy->damageCooldown = 1.0f;
        enemy->expValue = 10.0f;
    }
    
    componentManager.AddComponent<Component::HealthComponent>(zombie, Component::HealthComponent{});
    auto* health = componentManager.GetComponent<Component::HealthComponent>(zombie);
    if (health) {
        health->health = 50.0f;
        health->maxHealth = 50.0f;
        health->isAlive = true;
    }
    
    // Add CombatStatsComponent to track damage sources
    componentManager.AddComponent<Component::CombatStatsComponent>(zombie, Component::CombatStatsComponent{});
    
    componentManager.AddComponent<Component::TargetComponent>(zombie, Component::TargetComponent{});
    componentManager.AddComponent<engine::ECS::Tag>(zombie, engine::ECS::Tag{"enemy"});
    
    std::cout << "[GameEntityFactory] Created complete zombie entity: " << zombie << std::endl;
    return zombie;
}

uint32_t GameEntityFactory::CreateProjectile(const engine::Vector2& position, 
                                            const engine::Vector2& direction, 
                                            float speed) {
    if (!ValidateWorld()) return 0;
    
    auto& entityFactory = world_->GetEntityFactory();
    auto& componentManager = world_->GetComponentManager();
    
    uint32_t projectileId = entityFactory.CreateEntity("Projectile");
    
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
    
    // Add PhysicsModeComponent for PhysicsSystem to process movement
    componentManager.AddComponent<engine::ECS::PhysicsModeComponent>(projectileId,
        engine::ECS::PhysicsModeComponent{
            engine::ECS::PhysicsMode::TOP_DOWN,  // 2D physics mode
            0.0f, 0.0f, 0.0f,                    // no gravity
            false,                               // disable gravity
            false,                               // disable friction
            1.0f                                 // no friction factor
        });
    
    std::cout << "[GameEntityFactory] Created projectile: " << projectileId << std::endl;
    return projectileId;
}

bool GameEntityFactory::ValidateWorld() const {
    return world_ != nullptr;
}

} // namespace ZombieSurvivor::ECS
