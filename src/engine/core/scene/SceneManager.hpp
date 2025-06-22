// src/scene/SceneManager.hpp

#pragma once

#include "Scene.hpp"
#include <memory>
#include <functional>
#include <unordered_map>
#include <string>
#include <SDL3/SDL.h>
#include "engine/core/event/EventListener.hpp"
#include "engine/input/InputManager.hpp"
#include "engine/core/ecs/World.hpp"

namespace engine::scene {

using SceneFactory = std::function<std::unique_ptr<Scene>()>;

class SceneManager : public engine::event::EventListener {
public:
    SceneManager();
    ~SceneManager();

    // core functionalities
    void SetScene(std::unique_ptr<Scene> scene);
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);
    void HandleEvent(const SDL_Event& event);
    void UnloadScene();


    // scene registry system (game level)
    void RegisterScene(const std::string& sceneId, SceneFactory factory);
    void RequestSceneChange(const std::string& sceneId);

    std::string GetCurrentSceneId() const;

    void onEvent(const std::shared_ptr<engine::event::Event>& event) override;

    void SetEventManager(engine::event::EventManager* manager);
    void SetInputManager(input::InputManager* manager);
    void SetWorld(ECS::World* world) { world_ = world; }

    ECS::World* GetWorld() const { return world_; }

private:
    std::unique_ptr<Scene> CreateScene(const std::string& sceneId);
    
    std::unique_ptr<Scene> currentScene_;
    std::unordered_map<std::string, SceneFactory> sceneFactories_;

    engine::event::EventManager* eventManager_ = nullptr;
    input::InputManager* inputManager_ = nullptr;
    ECS::World* world_ = nullptr; 
    
    std::string nextSceneId_;
    bool sceneChangeRequested_ = false;

};

} // namespace scene
