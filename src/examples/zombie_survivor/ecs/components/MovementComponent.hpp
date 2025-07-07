// src/examples/zombie_survivor/ecs/component/MovementComponent.hpp
#pragma once

namespace ZombieSurvivor::Component {

struct MovementComponent {
    float speed = 150.0f;
    float acceleration = 800.0f;
    bool canMove = true;
};

}