// src/examples/zombie_survivor/ecs/components/ProjectileComponent.hpp

#pragma once
#include "engine/core/Types.hpp"
#include "engine/core/ecs/components/Tag.hpp"
#include "WeaponComponent.hpp"
#include <iostream>

namespace ZombieSurvivor::Component {

enum class ProjectileType {
    BULLET_PISTOL,
    BULLET_RIFLE,
};

struct ProjectileComponent {
    float damage = 25.0f;
    float speed = 800.0f;
    float maxLifetime = 3.0f;
    float currentLifetime = 0.0f;
    
    engine::Vector2 direction{1.0f, 0.0f};
    engine::Vector2 velocity{0.0f, 0.0f};
    
    engine::EntityID shooterId = 0;
    ProjectileType type = ProjectileType::BULLET_PISTOL;
    WeaponType sourceWeaponType = WeaponType::PISTOL;
    
    int penetration = 1;   
    float spread = 0.0f; 
    bool hasHit = false;
    bool shouldDestroy = false;

    float distanceTraveled = 0.0f;
    // int targetsHit = 0;
    
    // Visual Tailing Effect (Optional)
    // std::string visualEffect = "bullet_trail";
    // float trailLength = 0.0f;
};

} // namespace ZombieSurvivor::Component