// src/examples/zombie_survivor/ecs/components/EnemyComponent.hpp

#pragma once
#include "engine/core/Types.hpp"

namespace ZombieSurvivor::Component {

enum class EnemyType {
    ZOMBIE_BASIC,
    // ZOMBIE_FAST,
    // ZOMBIE_TANK
};

struct EnemyComponent {
    EnemyType type = EnemyType::ZOMBIE_BASIC;
    float damage = 10.0f;
    float damageCooldown = 1.0f;
    float lastDamageTime = 0.0f;
    float expValue = 10.0f;
};
}