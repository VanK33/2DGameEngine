// src/examples/zombie_survivor/GameScene.hpp

#pragma once

#include "engine/core/scene/Scene.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "engine/input/InputManager.hpp"
#include "engine/resource/ResourceManager.hpp"
#include "ecs/systems/GroundRenderSystem.hpp"

#include "examples/zombie_survivor/ecs/GameEntityFactory.hpp"

#include <memory>

namespace ZombieSurvivor {

class GameScene : public engine::scene::Scene {
public:
    GameScene(const std::string& id, 
              engine::input::InputManager* inputManager,
              engine::resources::ResourceManager* resourceManager);
    
    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    void HandleEvent(const SDL_Event& event) override;
    std::string GetSceneId() const override;

    // Manager setters
    void SetEventManager(engine::event::EventManager* manager) override;
    void SetInputManager(engine::input::InputManager* manager) override;

private:
    std::string sceneId_;
    
    // Manager references
    engine::input::InputManager* inputManager_;
    engine::resources::ResourceManager* resourceManager_;
    std::unique_ptr<ZombieSurvivor::ECS::GameEntityFactory> gameEntityFactory_;
    void InitializeSystems();  // 通过SystemManager添加系统
    void CreateEntities();     // 只负责创建游戏实体
};

} // namespace ZombieSurvivor