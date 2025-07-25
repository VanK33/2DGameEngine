// src/examples/zombie_survivor/configs/ProjectileConfig.cpp

#include "ProjectileConfig.hpp"

namespace ZombieSurvivor::Config {

ProjectileConfig ProjectileConfigManager::GetConfig(Component::AmmoType ammoType) {
    static auto configs = InitializeConfigs();
    auto it = configs.find(ammoType);
    return (it != configs.end()) ? it->second : configs[Component::AmmoType::PISTOL];
}

std::unordered_map<Component::AmmoType, ProjectileConfig> ProjectileConfigManager::InitializeConfigs() {
    return {
        // {AmmoType, {speed, range, damage, penetration, type}}
        {Component::AmmoType::PISTOL, {400.0f, 250.0f, 25.0f, 1, Component::ProjectileType::BULLET_PISTOL}},  // 0.625s lifetime
        {Component::AmmoType::RIFLE,  {800.0f, 500.0f, 50.0f, 2, Component::ProjectileType::BULLET_RIFLE}},   // 0.625s lifetime
        {Component::AmmoType::SMG,    {600.0f, 250.0f, 20.0f, 1, Component::ProjectileType::BULLET_PISTOL}},  // 0.417s lifetime
    };
}

} // namespace ZombieSurvivor::Config