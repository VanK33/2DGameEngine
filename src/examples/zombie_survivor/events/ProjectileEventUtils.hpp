// src/examples/zombie_survivor/events/ProjectileEventUtils.hpp

#pragma once
#include "GameEventTypes.hpp"
#include "GameEventData.hpp"
#include "engine/core/event/EventManager.hpp"

namespace ZombieSurvivor::Events {

class ProjectileEventUtils {
public:
    static void PublishCreateProjectile(
        engine::event::EventManager& eventManager,
        engine::EntityID shooterId,
        const engine::Vector2& startPos,
        const engine::Vector2& direction,
        float damage,
        float speed,
        float lifetime,
        Component::ProjectileType type,
        Component::WeaponType weaponType = Component::WeaponType::PISTOL
    ) {
        auto data = std::make_shared<CreateProjectileData>();
        data->shooterId = shooterId;
        data->startPosition = startPos;
        data->direction = direction;
        data->damage = damage;
        data->speed = speed;
        data->lifetime = lifetime;
        data->type = type;
        data->weaponType = weaponType;
        
        auto event = std::make_shared<GameEvent>(
            GameEventType::CREATE_PROJECTILE,
            std::static_pointer_cast<void>(data)
        );
        event->SetPriority(engine::event::EventPriority::HIGH);
        eventManager.Publish(event);
    }
    
    static void PublishProjectileHit(
        engine::event::EventManager& eventManager,
        engine::EntityID projectileId,
        engine::EntityID targetId,
        engine::EntityID shooterId,
        float damage,
        const engine::Vector2& hitPos,
        const std::string& hitType
    ) {
        auto data = std::make_shared<ProjectileHitData>();
        data->projectileId = projectileId;
        data->targetId = targetId;
        data->shooterId = shooterId;
        data->damage = damage;
        data->hitPosition = hitPos;
        data->hitType = hitType;
        
        auto event = std::make_shared<GameEvent>(
            GameEventType::PROJECTILE_HIT,
            std::static_pointer_cast<void>(data)
        );
        event->SetPriority(engine::event::EventPriority::HIGH);
        eventManager.Publish(event);
    }
};

} // namespace ZombieSurvivor::Events