// src/examples/zombie_survivor/ecs/components/HealthComponent.hpp
#pragma once

namespace ZombieSurvivor::Component {

struct HealthComponent {
    float health = 100.0f;
    float maxHealth = 100.0f;
    bool isAlive = true;
};
    
} // namespace ZombieSurvivor::Component