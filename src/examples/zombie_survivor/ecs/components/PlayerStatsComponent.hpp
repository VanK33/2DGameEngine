// src/engine/core/ecs/components/PlayerStatsComponent.hpp

#pragma once

namespace ZombieSurvivor::Component {

struct PlayerStatsComponent {
    int killCount = 0;
    float survivalTime = 0.0f;
    int totalExperienceGained = 0;
    int highestLevel = 1;
    float totalDamageTaken = 0.0f;
    float totalDamageDealt = 0.0f;
};

} // namespace ZombieSurvivor::Component