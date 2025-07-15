// src/examples/zombie_survivor/ecs/components/AmmoComponent.hpp
#pragma once

#include <string>

namespace ZombieSurvivor::Component { 

enum class AmmoType {
        NONE = 0,
        PISTOL,    // 9mm
        RIFLE,     // 5.56mm  
        SMG,       // .45 ACP
};

struct AmmoComponent {
    int currentAmmo = 12;
    int totalAmmo = 120;
    int maxTotalAmmo = 300;
    bool isReloading = false;
    float reloadProgress = 0.0f;
};

} // namespace ZombieSurvivor::Component

namespace std {
    template<>
    struct hash<ZombieSurvivor::Component::AmmoType> {
        size_t operator()(const ZombieSurvivor::Component::AmmoType& type) const {
            return hash<int>()(static_cast<int>(type));
        }
    };
}