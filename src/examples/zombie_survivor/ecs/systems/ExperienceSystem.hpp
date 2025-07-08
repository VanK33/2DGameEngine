// src/examples/zombie_survivor/ecs/systems/ExperienceSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/event/EventListener.hpp"
#include "examples/zombie_survivor/events/GameEventTypes.hpp"
#include "examples/zombie_survivor/ecs/components/ExperienceComponent.hpp"

namespace ZombieSurvivor::System {

class ExperienceSystem : public engine::ECS::System, public engine::event::EventListener {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    const char* GetName() const override { return "ExperienceSystem"; }

    void onEvent(const std::shared_ptr<engine::event::Event>& event) override;

    void AddExperience(uint32_t entityId, int experience);

private:
    void ProcessLevelUp(uint32_t entityId, ZombieSurvivor::Component::ExperienceComponent* exp);
    int CalculateExperienceForNextLevel(int level) const;
    void PublishLevelUpEvent(uint32_t entityId, int oldLevel, int newLevel);

    void HandleGameEvent(const std::shared_ptr<engine::event::Event>& event);
    void HandleEnemyKilled(const std::shared_ptr<void>& eventData);
    void HandleExperienceGained(const std::shared_ptr<void>& eventData);
};

} // namespace ZombieSurvivor::System