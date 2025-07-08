// src/examples/zombie_survivor/ecs/components/UpgradeComponent.hpp
#pragma once

#include <vector>
#include <unordered_map>

namespace ZombieSurvivor::Component {

enum class UpgradeType {
    DAMAGE_BOOST,
    MOVEMENT_SPEED_BOOST,
    MAX_HEALTH_BOOST,
    FIRE_RATE_BOOST,
    MAGAZINE_SIZE_BOOST
};

struct UpgradeComponent {
    std::unordered_map<UpgradeType, int> upgradeLevels;
    
    bool pendingUpgrade = false;
    std::vector<UpgradeType> currentOptions;
    
    UpgradeComponent() {
        upgradeLevels[UpgradeType::DAMAGE_BOOST] = 0;
        upgradeLevels[UpgradeType::MOVEMENT_SPEED_BOOST] = 0;
        upgradeLevels[UpgradeType::MAX_HEALTH_BOOST] = 0;
        upgradeLevels[UpgradeType::FIRE_RATE_BOOST] = 0;
        upgradeLevels[UpgradeType::MAGAZINE_SIZE_BOOST] = 0;
    }
    
    int GetUpgradeLevel(UpgradeType type) const {
        auto it = upgradeLevels.find(type);
        return it != upgradeLevels.end() ? it->second : 0;
    }
    
    void ApplyUpgrade(UpgradeType type) {
        upgradeLevels[type]++;
    }
    
    bool HasUpgrade(UpgradeType type) const {
        return GetUpgradeLevel(type) > 0;
    }
};

} // namespace ZombieSurvivor::Component