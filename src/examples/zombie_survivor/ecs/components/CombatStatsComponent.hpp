// src/examples/zombie_survivor/ecs/components/CombatStatsComponent.hpp

#pragma once

#include <memory>

namespace ZombieSurvivor::Component {

struct CombatStatsComponent {
    uint32_t lastDamageSource = 0;
    float lastDamageTime = 0.0f;
    float totalDamageDealt = 0.0f;
    float totalDamageTaken = 0.0f;
};

} // namespace ZombieSurvivor::Component