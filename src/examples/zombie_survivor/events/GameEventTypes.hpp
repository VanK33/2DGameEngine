// src/examples/zombie_survivor/events/GameEventTypes.hpp

#pragma once

#include "engine/core/event/Event.hpp"

namespace ZombieSurvivor::Events {

enum class GameEventType {
    ENEMY_KILLED,
    PLAYER_LEVEL_UP,
    EXPERIENCE_GAINED,
    PLAYER_DIED,
    WEAPON_SWITCHED,
    WEAPON_FIRE_REQUESTED,
    WEAPON_FIRED,
    RELOAD_STARTED,
    RELOAD_COMPLETED,
    RELOAD_REQUESTED,
    AMMO_DEPLETED,
    AMMO_CONSUMED,
    DAMAGE_DEALT,
    DAMAGE_TAKEN,
    POWER_UP_COLLECTED,
    WAVE_COMPLETED,
    HEALTH_CHANGED,
    ENTITY_DIED,
    WEAPON_INITIALIZED, 
    RELOAD_EXECUTED,
    RELOAD_EXECUTE,
};

class GameEvent : public engine::event::Event {
public:
    GameEvent(GameEventType type, std::shared_ptr<void> data = nullptr) 
        : Event(engine::event::EventType::CUSTOM, data), gameEventType(type) {}
    
    GameEventType GetGameEventType() const { return gameEventType; }

    template<typename T>
    std::shared_ptr<T> GetEventData() const {
        return std::static_pointer_cast<T>(GetData());
    }
    
private:
    GameEventType gameEventType;
};

} // namespace ZombieSurvivor::Events