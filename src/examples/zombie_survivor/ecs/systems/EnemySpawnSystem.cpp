#include "EnemySpawnSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "engine/core/ecs/components/Collider2D.hpp"
#include "engine/core/ecs/components/Tag.hpp"
#include "engine/core/ecs/components/AIComponent.hpp"
#include "examples/zombie_survivor/ecs/components/EnemyComponent.hpp"
#include "examples/zombie_survivor/ecs/components/HealthComponent.hpp"
#include "examples/zombie_survivor/ecs/components/TargetComponent.hpp"
#include <iostream>

namespace ZombieSurvivor::System {

using engine::Vector2;
using engine::ECS::EntityID;

void EnemySpawnSystem::Init() {
    gen_ = std::mt19937(rd_());
    std::cout << "[EnemySpawnSystem] Initialized with viewport: " 
              << viewportWidth_ << "x" << viewportHeight_ 
              << ", spawn margin: " << spawnMargin_ << std::endl;
}

void EnemySpawnSystem::Update(float deltaTime) {
    UpdateEnemyCount();
    
    if (currentEnemyCount_ >= maxEnemies_) {
        return;
    }
    
    spawnTimer_ += deltaTime;
    
    if (spawnTimer_ >= spawnInterval_) {
        SpawnZombie();
        spawnTimer_ = 0.0f;
        
        spawnInterval_ = std::max(0.5f, spawnInterval_ - 0.01f);
    }
}

void EnemySpawnSystem::Shutdown() {
    std::cout << "[EnemySpawnSystem] Shutdown. Total spawned: " << totalSpawned_ << std::endl;
}

void EnemySpawnSystem::SetViewportSize(float width, float height) {
    viewportWidth_ = width;
    viewportHeight_ = height;
    std::cout << "[EnemySpawnSystem] Viewport size updated to: " 
              << width << "x" << height << std::endl;
}

void EnemySpawnSystem::SpawnZombie() {
    Vector2 spawnPos = GetRandomSpawnPosition();
    EntityID zombie = CreateZombieEntity(spawnPos);
    
    if (zombie != 0) {
        totalSpawned_++;
        currentEnemyCount_++;
        std::cout << "[EnemySpawnSystem] Spawned zombie #" << totalSpawned_ 
                  << " at position (" << spawnPos.x << ", " << spawnPos.y << ")" << std::endl;
    }
}

Vector2 EnemySpawnSystem::GetRandomSpawnPosition() {
    SpawnEdge edge = GetRandomEdge();
    Vector2 position;
    
    std::uniform_real_distribution<float> widthDist(0, viewportWidth_);
    std::uniform_real_distribution<float> heightDist(0, viewportHeight_);
    
    switch (edge) {
        case SpawnEdge::TOP:
            position.x = widthDist(gen_);
            position.y = -spawnMargin_;
            break;
            
        case SpawnEdge::BOTTOM:
            position.x = widthDist(gen_);
            position.y = viewportHeight_ + spawnMargin_;
            break;
            
        case SpawnEdge::LEFT:
            position.x = -spawnMargin_;
            position.y = heightDist(gen_);
            break;
            
        case SpawnEdge::RIGHT:
            position.x = viewportWidth_ + spawnMargin_;
            position.y = heightDist(gen_);
            break;
    }
    
    return position;
}

EnemySpawnSystem::SpawnEdge EnemySpawnSystem::GetRandomEdge() {
    return static_cast<SpawnEdge>(edgeDist_(gen_));
}

EntityID EnemySpawnSystem::CreateZombieEntity(const Vector2& position) {
    auto* world = GetWorld();
    if (!world) {
        std::cerr << "[EnemySpawnSystem] Error: No world available" << std::endl;
        return 0;
    }
    
    auto& entityFactory = world->GetEntityFactory();
    auto& componentManager = world->GetComponentManager();
    
    EntityID zombie = entityFactory.CreateEntity("Zombie");
    
    componentManager.AddComponent<engine::ECS::Transform2D>(zombie, 
        engine::ECS::Transform2D{position.x, position.y, 0.0f, 1.0f, 1.0f});
    componentManager.AddComponent<engine::ECS::Velocity2D>(zombie, 
        engine::ECS::Velocity2D{0.0f, 0.0f, 100.0f});
    
    componentManager.AddComponent<engine::ECS::Collider2D>(zombie, 
        engine::ECS::Collider2D{{0, 0, 30, 30}, false, "enemy"});
    
    componentManager.AddComponent<engine::ECS::AIComponent>(zombie, 
        engine::ECS::AIComponent{
            engine::ECS::AIState::ACTIVE,  // 修正：没有CHASING状态
            0,                              // targetEntity (稍后由AI系统设置)
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
    
    componentManager.AddComponent<Component::TargetComponent>(zombie, Component::TargetComponent{});
    componentManager.AddComponent<engine::ECS::Tag>(zombie, engine::ECS::Tag{"enemy"});
    
    return zombie;
}

void EnemySpawnSystem::UpdateEnemyCount() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto enemies = componentManager.GetEntitiesWithComponent<Component::EnemyComponent>();
    currentEnemyCount_ = 0;
    for (EntityID enemy : enemies) {
        auto* health = componentManager.GetComponent<Component::HealthComponent>(enemy);
        if (health && health->isAlive) {
            currentEnemyCount_++;
        }
    }
}

void EnemySpawnSystem::ClearAllEnemies() {
    auto* world = GetWorld();
    if (!world) {
        std::cout << "[EnemySpawnSystem] Warning: No world available for cleanup" << std::endl;
        return;
    }
    
    auto& componentManager = world->GetComponentManager();
    auto& entityFactory = world->GetEntityFactory();
    auto enemies = componentManager.GetEntitiesWithComponent<Component::EnemyComponent>();
    
    int clearedCount = 0;
    for (EntityID enemy : enemies) {
        entityFactory.DestroyEntity(enemy);
        clearedCount++;
    }
    
    currentEnemyCount_ = 0;
    totalSpawned_ = 0;
    
    std::cout << "[EnemySpawnSystem] Cleared " << clearedCount << " enemies and reset counters" << std::endl;
}

} // namespace ZombieSurvivor::System
