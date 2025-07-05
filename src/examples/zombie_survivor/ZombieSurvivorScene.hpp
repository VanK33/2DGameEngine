// src/example/zombie_survivor/ZombieSurvivorScene.hpp

#pragma once

#include "engine/core/scene/Scene.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "engine/core/ecs/components/Sprite2D.hpp"
#include "engine/core/ecs/components/Collider2D.hpp"
#include "engine/core/ecs/components/Lifetime.hpp"
#include "engine/core/ecs/components/AIComponent.hpp"
#include "engine/core/Types.hpp"
#include "engine/graphics/sprite/SpriteRenderer.hpp"
#include "engine/resource/ResourceManager.hpp"
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <memory>

namespace game::scene {

// ================================
// GAME CONSTANTS
// ================================
constexpr float PLAYER_DEFAULT_SPEED = 150.0f;
constexpr float PROJECTILE_DEFAULT_SPEED = 400.0f;
constexpr float DEFAULT_WEAPON_RANGE = 250.0f;
constexpr float ZOMBIE_BASIC_HEALTH = 30.0f;
constexpr float ZOMBIE_BASIC_DAMAGE = 20.0f;
constexpr int MAX_ENEMIES = 20;
constexpr int MAX_PROJECTILES = 50;

// ================================
// GAME-SPECIFIC COMPONENTS
// ================================

struct PlayerComponent {
    float health = 100.0f;
    float maxHealth = 100.0f;
    float moveSpeed = PLAYER_DEFAULT_SPEED;  // 使用常量
    int level = 1;
    int experience = 0;
    int experienceToNext = 10;
    bool isAlive = true;
};

struct WeaponComponent {
    enum class WeaponType { PISTOL, SHOTGUN, RIFLE };
    
    WeaponType type = WeaponType::PISTOL;
    
    float damage = 15.0f;
    float fireRate = 3.0f;
    float range = DEFAULT_WEAPON_RANGE;  // 使用常量
    float projectileSpeed = PROJECTILE_DEFAULT_SPEED;  // 使用常量
    
    int ammo = 12;
    int maxAmmo = 12;
    int totalAmmo = 120;
    bool hasInfiniteAmmo = true;
    float reloadTime = 2.0f;
    
    float timeSinceLastShot = 0.0f;
    float reloadTimer = 0.0f;
    bool isReloading = false;
    bool canShoot = true;
};

struct AimingComponent {
    engine::Vector2 aimDirection{1.0f, 0.0f};
    engine::Vector2 mouseWorldPos{0.0f, 0.0f};
    float aimRange = DEFAULT_WEAPON_RANGE;  // 使用常量
    bool isAiming = false;
    bool showAimLine = true;
};

struct EnemyComponent {
    enum class EnemyType { ZOMBIE_BASIC };

    EnemyType type = EnemyType::ZOMBIE_BASIC;
    float health = ZOMBIE_BASIC_HEALTH;
    float maxHealth = ZOMBIE_BASIC_HEALTH;
    float moveSpeed = 50.0f;
    float attackDamage = ZOMBIE_BASIC_DAMAGE;
    float expValue = 3.0f;
    float attackRange = 25.0f;
    float attackCooldown = 2.0f;
    float timeSinceLastAttack = 0.0f;
};

struct ProjectileComponent {
    engine::Vector2 velocity{0.0f, 0.0f};
    float damage = 15.0f;
    float remainingTime = 0.0f;
    engine::ECS::EntityID shooter = 0;
    bool hasHit = false;
};

// ================================
// MAIN SCENE CLASS
// ================================

class ZombieSurvivorScene : public engine::scene::Scene {
public:
    ZombieSurvivorScene(const std::string& id, SDL_Renderer* renderer,
                        engine::graphics::SpriteRenderer* spriteRenderer);
    
    // Scene lifecycle
    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void HandleEvent(const SDL_Event& event) override;
    void SetEventManager(engine::event::EventManager* manager) override;
    void SetInputManager(engine::input::InputManager* manager) override;
    std::string GetSceneId() const override;

private:
    // ================================
    // CORE SCENE DATA
    // ================================
    std::string sceneId_;
    engine::graphics::SpriteRenderer* spriteRenderer_;
    engine::resources::ResourceManager resourceManager_;
    
    // ================================
    // GAME ENTITIES
    // ================================
    engine::ECS::EntityID playerId_ = 0;
    std::vector<engine::ECS::EntityID> enemies_;
    std::vector<engine::ECS::EntityID> projectiles_;
    
    // ================================
    // GAME STATE
    // ================================
    struct GameState {
        bool isGameRunning = true;
        bool isPaused = false;
        float gameTime = 0.0f;
        int killCount = 0;
        int currentWave = 1;
        float nextEnemySpawn = 2.0f;
        float enemySpawnInterval = 3.0f;
    } gameState_;
    
    // ================================
    // INPUT & AIMING
    // ================================
    struct MouseState {
        float worldX = 0.0f;
        float worldY = 0.0f;
        bool leftButtonHeld = false;
        bool leftButtonPressed = false;
    } mouseState_;

    // ================================
    // PHASE 1: CORE SYSTEMS
    // ================================
    
    // 1.1 Player Control and Aiming System
    void HandlePlayerInput(float deltaTime);
    void UpdatePlayerAiming();
    void CreatePlayer();
    
    // 1.2 Manual Shooting System
    void HandleShooting(float deltaTime);
    void HandleReloading(float deltaTime);
    void CreateProjectile(float startX, float startY, float dirX, float dirY);
    
    // 1.3 Bullet and Ballistics System
    void UpdateProjectiles(float deltaTime);
    void CheckProjectileCollisions();
    
    // 1.4 Enemy AI and Tracking System
    void SpawnEnemy();
    void UpdateEnemies(float deltaTime);
    void UpdateEnemyAI(engine::ECS::EntityID enemyId, float deltaTime);
    
    // 1.5 Experience and Upgrade System
    void GainExperience(int amount);
    void CheckLevelUp();
    void ShowUpgradeOptions();
    
    // 1.6 UI and User Interface
    void RenderUI(SDL_Renderer* renderer);
    void RenderHealthBar(SDL_Renderer* renderer);
    void RenderAmmoDisplay(SDL_Renderer* renderer);
    void RenderAimLine(SDL_Renderer* renderer);
    void RenderGameStats(SDL_Renderer* renderer);
    
    // ================================
    // HELPER METHODS
    // ================================
    void CleanupDeadEntities();
    void CheckCollisions();
    float CalculateDistance(float x1, float y1, float x2, float y2);
    float CalculateAngle(float fromX, float fromY, float toX, float toY);
    void NormalizeVector(float& x, float& y);
    
    // ================================
    // UTILITY METHODS
    // ================================
    
    // Calculate squared distance between two points (avoids sqrt operation)
    float CalculateDistanceSquared(float x1, float y1, float x2, float y2);
    
    // Safe component getter with null check
    template<typename T>
    T* GetComponentSafe(engine::ECS::EntityID entityId);
    
    // Check if entity is within viewport bounds
    bool IsEntityInViewport(engine::ECS::EntityID entityId);
    
    // World-screen coordinate conversion methods
    engine::Vector2 ScreenToWorld(float screenX, float screenY);
    engine::Vector2 WorldToScreen(float worldX, float worldY);
    
    // ================================
    // GAME SETTINGS
    // ================================
    struct Settings {
        float playerSpeed = PLAYER_DEFAULT_SPEED;
        float projectileSpeed = PROJECTILE_DEFAULT_SPEED;
        float enemySpawnRate = 3.0f;
        int maxEnemies = MAX_ENEMIES;
        int maxProjectiles = MAX_PROJECTILES;
        bool debugMode = false;
        bool showAimLine = true;
    } settings_;
};
}