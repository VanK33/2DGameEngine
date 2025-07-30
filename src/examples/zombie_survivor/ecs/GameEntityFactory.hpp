// src/examples/zombie_survivor/ecs/GameEntityFactory.hpp
#pragma once

#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/EntityFactory.hpp"
#include "engine/core/ecs/ComponentManager.hpp"
#include "engine/core/ecs/components/Transform2D.hpp"
#include "engine/core/ecs/components/Sprite2D.hpp"
#include "engine/core/ecs/components/Collider2D.hpp"
#include "engine/core/ecs/components/Velocity2D.hpp"
#include "examples/zombie_survivor/ecs/RenderLayer.hpp"
#include "engine/core/Types.hpp"
#include "engine/resource/ResourceManager.hpp"

namespace ZombieSurvivor::ECS {

class GameEntityFactory {
public:
    explicit GameEntityFactory(engine::ECS::World* world, engine::resources::ResourceManager* resourceManager);
    
    uint32_t CreatePlayer(const engine::Vector2& position = {400.0f, 300.0f});
    uint32_t CreateWeapon(engine::EntityID playerEntityId, const engine::Vector2& offset = {20.0f, 0.0f});
    uint32_t CreatePlayerHUD(engine::EntityID playerEntityId);
    // uint32_t CreateStaticRock(const engine::Vector2& position);
    // uint32_t CreateStaticTree(const engine::Vector2& position);
    
    // 🎯 动态实体创建（运行时）
    uint32_t CreateZombie(const engine::Vector2& position);
    uint32_t CreateProjectile(const engine::Vector2& position, const engine::Vector2& direction, float speed);
    // uint32_t CreatePickup(const engine::Vector2& position, const std::string& type);
    // uint32_t CreateExplosion(const engine::Vector2& position, float radius);
    
    // 🎯 UI实体创建
    // uint32_t CreateHUDElement(const std::string& type, const engine::Vector2& position);
    
private:
    engine::ECS::World* world_;
    engine::resources::ResourceManager* resourceManager_;
    
    // 辅助方法
    bool ValidateWorld() const;
    // uint32_t CreateBaseEntity(const std::string& name);
};

} // namespace ZombieSurvivor::ECS
