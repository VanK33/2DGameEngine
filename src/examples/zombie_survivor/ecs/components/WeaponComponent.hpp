// src/examples/zombie_survivor/ecs/components/WeaponComponent.hpp
#pragma once

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
    float damage = 15.0f;
    float range = 250.0f;
    float fireRate = 3.0f;
    float reloadTime = 2.0f;

    int magazineCapacity = 12;
    int defaultTotalAmmo = 120;
    int maxTotalAmmo = 300;
};

}