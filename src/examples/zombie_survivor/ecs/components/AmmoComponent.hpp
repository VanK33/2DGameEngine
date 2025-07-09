// src/examples/zombie_survivor/ecs/components/AmmoComponent.hpp
#pragma once

#include <string>

namespace ZombieSurvivor::Component { 

struct AmmoComponent {
    int currentAmmo = 12;
    int totalAmmo = 120;
    int maxTotalAmmo = 300;
    bool isReloading = false;
    float reloadProgress = 0.0f;
};

} // namespace ZombieSurvivor::Component