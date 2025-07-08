// src/examples/zombie_survivor/ecs/systems/UpgradeSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/event/EventListener.hpp"
#include "engine/core/event/Event.hpp"
#include "examples/zombie_survivor/ecs/components/UpgradeComponent.hpp"
#include <memory>
#include <vector>
#include <random>

namespace ZombieSurvivor::System {

class UpgradeSystem : public engine::ECS::System, public engine::event::EventListener {

public:
    UpgradeSystem() = default;

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "UpgradeSystem"; }

    void onEvent(const std::shared_ptr<engine::event::Event>& event) override;
    void SelectUpgrade(uint32_t entityId, Component::UpgradeType upgrateType);

private:
    std::mt19937 rng_;

    void HandleGameEvent(const std::shared_ptr<engine::event::Event>& event);
    void HandleLevelUpEvent(const std::shared_ptr<void>& eventData);
    std::vector<Component::UpgradeType> GenerateUpgradeOptions(int numOptions = 3);
    void ApplyUpgradeEffect(uint32_t entityId, Component::UpgradeType upgradeType);
};

} // namespace ZombieSurvivor::System