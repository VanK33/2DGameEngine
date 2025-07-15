// src/examples/zombie_survivor/configs/ProjectileConfig.hpp

#pragma once
#include "examples/zombie_survivor/ecs/components/AmmoComponent.hpp"
#include "examples/zombie_survivor/ecs/components/ProjectileComponent.hpp"
#include <unordered_map>

namespace ZombieSurvivor::Config {

struct ProjectileConfig {
    float speed;
    float lifetime;
    float damage;
    int penetration;
    Component::ProjectileType type;
};

class ProjectileConfigManager {
public:
    static ProjectileConfig GetConfig(Component::AmmoType ammoType);
    
private:
    static std::unordered_map<Component::AmmoType, ProjectileConfig> InitializeConfigs();
};

} // namespace ZombieSurvivor::Config
