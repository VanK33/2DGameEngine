// src/examples/zombie_survivor/events/GameEventData.hpp

#pragma once

#include "examples/zombie_survivor/ecs/components/AmmoComponent.hpp"
#include "examples/zombie_survivor/ecs/components/WeaponComponent.hpp"
#include "examples/zombie_survivor/ecs/components/ProjectileComponent.hpp"
#include <string>
#include <cstdint>

namespace ZombieSurvivor::Events {

struct EnemyKilledData {
    uint32_t playerId;
    uint32_t enemyId;
    int expReward;
    std::string enemyType;
    float positionX, positionY;
};

struct PlayerLevelUpData {
    uint32_t playerId;
    int oldLevel;
    int newLevel;
    int totalExperience;
};

struct ExperienceGainedData {
    uint32_t playerId;
    int experienceAmount;
    std::string source; // "enemy_kill", "quest", "bonus"
};

struct DamageData {
    uint32_t sourceEntityId;
    uint32_t targetEntityId;
    int damageAmount;
    std::string damageType; // "physical", "fire", "poison"
};

struct WeaponSwitchedData {
    uint32_t playerId;
    Component::WeaponType oldWeaponType;
    Component::WeaponType newWeaponType;
    float fireRate;
    float reloadTime; 
};

struct WeaponFireRequestData {
    uint32_t entityId;
    bool inputPressed;
};

struct WeaponFiredData {
    uint32_t entityId;
    float damage;
    int currentAmmo;
    int totalAmmo;
};

struct ReloadData {
    uint32_t entityId;
    float reloadTime;
    int magazineCapacity;
    int totalAmmo;
};

struct AmmoConsumedData {
    uint32_t entityId;
    int amountConsumed;
    int currentAmmo;
    int totalAmmo;
};

struct HealthChangedData {
    uint32_t entityId;
    float oldHealth;
    float newHealth;
    float healthPercentage;
};

struct EntityDiedData {
    uint32_t entityId;
    std::string deathCause;
    float positionX;
    float positionY;
    float survivalTime;
    uint32_t killerEntityId;
};

struct ReloadExecuteData {
    uint32_t entityId;
    int reloadAmount;
    int magazineCapacity;
};

struct WeaponInitData {
    uint32_t entityId;
    int magazineCapacity;
    int defaultTotalAmmo;
    int maxTotalAmmo;
    float fireRate;
    float damage;
    float reloadTime;
};

struct ReloadExecutedData {
    uint32_t entityId;
    int actualReloadAmount;
    int newCurrentAmmo;
    int newTotalAmmo;
};

struct FireInputData {
    uint32_t playerId;
    bool pressed;
};

struct ReloadInputData {
    uint32_t playerId;
};

struct WeaponSwitchInputData {
    uint32_t playerId;
    int weaponSlot; // 0 - 2 for weapon slot
};

struct AmmoConsumeRequestData {
    uint32_t playerId;
    Component::AmmoType ammoType;
    int amount;
};

struct AmmoRefreshData {
    uint32_t playerId;
    Component::AmmoType ammoType;
    int ammoToAdd;
};

struct AmmoDepletedData {
    uint32_t playerId;
    Component::AmmoType ammoType;
};

struct WeaponPickupData {
    uint32_t playerId;
    Component::WeaponType weaponType;
    int carriedAmmo;
};

struct CreateProjectileData {
    engine::EntityID shooterId;
    engine::Vector2 startPosition;
    engine::Vector2 direction;
    float damage;
    float speed;
    float lifetime;
    Component::ProjectileType type;
    
    Component::WeaponType weaponType;
    int penetration = 1;
    float spread = 0.0f;
};

struct ProjectileCreatedData {
    engine::EntityID projectileId;
    engine::EntityID shooterId;
    engine::Vector2 position;
    engine::Vector2 direction;
    Component::ProjectileType type;
};

struct ProjectileHitData {
    engine::EntityID projectileId;
    engine::EntityID targetId;
    engine::EntityID shooterId;
    float damage;
    engine::Vector2 hitPosition;
    std::string hitType; // "enemy", "wall", "boundary"
};

struct ProjectileDestroyedData {
    engine::EntityID projectileId;
    engine::EntityID shooterId;
    std::string destroyReason; // "hit", "lifetime", "boundary"
    engine::Vector2 lastPosition;
};

} // namespace ZombieSurvivor::Events