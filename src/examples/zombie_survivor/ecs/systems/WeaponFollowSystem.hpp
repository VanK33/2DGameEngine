#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "examples/zombie_survivor/ecs/components/FollowComponent.hpp"

namespace ZombieSurvivor::System {

class WeaponFollowSystem : public engine::ECS::System {
public:
    WeaponFollowSystem() = default;
    ~WeaponFollowSystem() = default;

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    const char* GetName() const override { return "WeaponFollowSystem"; }

private:
    void UpdateFollowPosition(engine::EntityID followerId, const Component::FollowComponent& follow, float deltaTime);
    void CopyInputFromPlayer(engine::EntityID weaponId, const Component::FollowComponent& follow);
};

} // namespace ZombieSurvivor::System