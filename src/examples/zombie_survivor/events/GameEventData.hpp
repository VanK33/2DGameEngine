// src/examples/zombie_survivor/events/GameEventData.hpp

#pragma once

#include "examples/zombie_survivor/ecs/components/AmmoComponent.hpp"
#include "examples/zombie_survivor/ecs/components/WeaponComponent.hpp"
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

} // namespace ZombieSurvivor::Events