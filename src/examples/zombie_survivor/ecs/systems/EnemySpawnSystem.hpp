#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/Types.hpp"
#include <random>

// Forward declaration
namespace ZombieSurvivor::ECS {
    class GameEntityFactory;
}

namespace ZombieSurvivor::System {

using EntityID = engine::EntityID;
using Vector2 = engine::Vector2;

class EnemySpawnSystem : public engine::ECS::System {
public:
    EnemySpawnSystem() = default;
    ~EnemySpawnSystem() = default;

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "EnemySpawnSystem"; }

    void SetSpawnInterval(float interval) { spawnInterval_ = interval; }
    void SetSpawnMargin(float margin) { spawnMargin_ = margin; }
    void SetMaxEnemies(int max) { maxEnemies_ = max; }
    void SetViewportSize(float width, float height);
    
    // Dependency injection for factory
    void SetEntityFactory(ZombieSurvivor::ECS::GameEntityFactory* factory) { gameEntityFactory_ = factory; }
    
    void ClearAllEnemies();
    void ResetCounters() { totalSpawned_ = 0; currentEnemyCount_ = 0; }
    
    int GetTotalSpawned() const { return totalSpawned_; }
    int GetCurrentEnemyCount() const { return currentEnemyCount_; }

private:
    enum class SpawnEdge {
        TOP,
        BOTTOM,
        LEFT,
        RIGHT
    };

    void SpawnZombie();
    engine::Vector2 GetRandomSpawnPosition();
    SpawnEdge GetRandomEdge();
    
    void UpdateEnemyCount();
    
    // Factory dependency
    ZombieSurvivor::ECS::GameEntityFactory* gameEntityFactory_ = nullptr;
    
    float viewportWidth_ = 800.0f;
    float viewportHeight_ = 600.0f;
    float spawnMargin_ = 100.0f;
    
    // spawn control
    float spawnTimer_ = 0.0f;
    float spawnInterval_ = 2.0f;
    int maxEnemies_ = 150;
    int currentEnemyCount_ = 0;
    int totalSpawned_ = 0;
    
    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_int_distribution<> edgeDist_{0, 3};
};

} // namespace ZombieSurvivor::System
