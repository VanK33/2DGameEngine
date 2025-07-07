// src/examples/zombie_survivor/ecs/components/UpgradeComponent.hpp
#pragma once

namespace ZombieSurvivor::Component {

struct UpgradeComponent {
    int level = 1;
    int experience = 0;
    int experienceToNext = 10;
    int skillPoints = 0;
};

}
