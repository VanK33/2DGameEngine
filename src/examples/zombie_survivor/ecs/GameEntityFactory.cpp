// src/examples/zombie_survivor/ecs/GameEntityFactory.cpp
#include "GameEntityFactory.hpp"
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
            ToInt(RenderLayer::UI)          // HIGHEST layer (20) - should be on top of everything
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
    
    std::cout << "[GameEntityFactory] Created player entity: " << playerId << std::endl;
    return playerId;
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
            {255, 0, 0, 255},              // 红色僵尸
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
            {255, 255, 0, 255},            // 黄色子弹
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
