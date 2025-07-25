// src/examples/zombie_survivor/ecs/components/WeaponComponent.hpp
#pragma once

#include "AmmoComponent.hpp"
#include <string>

namespace ZombieSurvivor::Component {

enum class WeaponType {
    NONE = 0,
    PISTOL,
    RIFLE,
    SMG,
};

struct WeaponComponent {
    WeaponType type = WeaponType::PISTOL;
    float damage = 15.0f;  // Likely be over-written by bullet property
    float range = 250.0f;  // Likely be over-written by bullet property
    float fireRate = 0.3f;
    float reloadTime = 2.0f;
    bool isReloading = false; 
    float lastFireTime = 0.0f; 

    int magazineCapacity = 12;
    int defaultTotalAmmo = 120;
    int maxTotalAmmo = 300;

    Component::AmmoType currentAmmoType = Component::AmmoType::PISTOL; 
};

}