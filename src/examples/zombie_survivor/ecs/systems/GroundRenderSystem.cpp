// src/examples/zombie_survivor_Ecs/systems/GroundRenderSystem.cpp
#include "GroundRenderSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include <iostream>
#include <vector>

namespace ZombieSurvivor::System {

void GroundRenderSystem::Init() {
    CreateGroundBackground();
    AddGroundDecorations();
    std::cout << "[GroundRenderSystem] Initialized with background and decorations" << std::endl;
}

void GroundRenderSystem::Update(float deltaTime) {
    // Static, no need to update reguarlarly
}

void GroundRenderSystem::Shutdown() {
    std::cout << "[GroundRenderSystem] Shutdown" << std::endl;
}

void GroundRenderSystem::CreateGroundBackground() {
    auto* world = GetWorld();
    if (!world) return;

    auto& entityFactory = world->GetEntityFactory();
    auto& componentManager = world->GetComponentManager();

    groundEntityId_ = entityFactory.CreateEntity("Ground");
    componentManager.AddComponent<engine::ECS::Transform2D>(groundEntityId_, 
        engine::ECS::Transform2D{0, 0, 0, 20.0f, 20.0f});
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(groundEntityId_,
        engine::ECS::Sprite2D{
            "terrain/ground_tile.png",
            {0, 0, 0, 0},
            true,
            {135, 206, 235, 180},  // Sky blue with transparency
            ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::BACKGROUND)
        });

    std::cout << "[GroundRenderSystem] Created ground background entity: " << groundEntityId_ << std::endl;
}

void GroundRenderSystem::AddGroundDecorations() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& entityFactory = world->GetEntityFactory();
    auto& componentManager = world->GetComponentManager();
    
    auto rockEntity = entityFactory.CreateEntity("Rock");
    componentManager.AddComponent<engine::ECS::Transform2D>(rockEntity, 
        engine::ECS::Transform2D{150, 200, 0, 1.0f, 1.0f});
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(rockEntity, 
        engine::ECS::Sprite2D{
            "terrain/rock.png",
            {0, 0, 0, 0},
            true,
            {255, 255, 255, 255},
            ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::GROUND_DETAILS)
        });
    
    decorationEntities_.push_back(rockEntity);
    
    auto grassEntity = entityFactory.CreateEntity("Grass");
    componentManager.AddComponent<engine::ECS::Transform2D>(grassEntity, 
        engine::ECS::Transform2D{300, 150, 0, 1.0f, 1.0f});
    
    componentManager.AddComponent<engine::ECS::Sprite2D>(grassEntity, 
        engine::ECS::Sprite2D{
            "terrain/grass.png",
            {0, 0, 0, 0},
            true,
            {100, 255, 100, 255},
            ZombieSurvivor::ECS::ToInt(ZombieSurvivor::ECS::RenderLayer::GROUND_DETAILS)
        });
    
    decorationEntities_.push_back(grassEntity);
    
    std::cout << "[GroundRenderSystem] Added " << decorationEntities_.size() << " decoration entities" << std::endl;
}

}
