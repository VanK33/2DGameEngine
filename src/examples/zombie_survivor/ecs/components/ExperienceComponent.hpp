// src/examples/zombie_survivor/ecs/components/ExperienceComponent.hpp
#pragma once

namespace ZombieSurvivor::Component {

struct ExperienceComponent {
    int level = 1;
    int experience = 0;
    int experienceToNext = 10;
    int skillPoints = 0;
    bool canLevelUp = true;
};

}
