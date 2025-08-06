#include "EnemySpawnSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "examples/zombie_survivor/ecs/GameEntityFactory.hpp"
#include "examples/zombie_survivor/ecs/components/EnemyComponent.hpp"
#include "examples/zombie_survivor/ecs/components/HealthComponent.hpp"
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
    if (!gameEntityFactory_) {
        std::cerr << "[EnemySpawnSystem] Error: GameEntityFactory not set!" << std::endl;
        return;
    }
    
    Vector2 spawnPos = GetRandomSpawnPosition();
    EntityID zombie = gameEntityFactory_->CreateZombie(spawnPos);
    
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
