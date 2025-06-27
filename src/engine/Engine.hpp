#pragma once

#include "core/ecs/World.hpp"
#include "core/event/EventManager.hpp"
#include "input/InputManager.hpp"
#include "core/scene/SceneManager.hpp"
#include "graphics/renderer/Renderer.hpp"
#include "graphics/sprite/SpriteRenderer.hpp"
#include "resource/ResourceManager.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include <string>
#include <functional>

namespace engine {

/**
 * Configuration structure for engine initialization
 */
struct EngineConfig {
    std::string windowTitle = "2D Game Engine";
    int windowWidth = 800;
    int windowHeight = 600;
    bool fullscreen = false;
    bool vsync = true;
    int targetFPS = 60;
};

/**
 * Main Engine class - Unified interface for the 2D Game Engine
 * 
 * This class provides a simple, unified API for initializing and running games.
 * It manages all core engine systems and provides easy access to them.
 * 
 * Example usage:
 * ```cpp
 * engine::Engine gameEngine;
 * engine::EngineConfig config;
 * config.windowTitle = "My Game";
 * 
 * if (!gameEngine.Initialize(config)) {
 *     return -1;
 * }
 * 
 * // Register scenes
 * gameEngine.GetSceneManager().RegisterScene("MainMenu", []() {
 *     return std::make_unique<MainMenuScene>();
 * });
 * 
 * // Start the game
 * gameEngine.Run();
 * gameEngine.Shutdown();
 * ```
 */
class Engine {
public:
    Engine();
    ~Engine();

    // Core lifecycle
    bool Initialize(const EngineConfig& config = EngineConfig{});
    void Run();
    void Shutdown();
    
    // System access
    ECS::World& GetWorld() { return world_; }
    event::EventManager& GetEventManager() { return eventManager_; }
    input::InputManager& GetInputManager() { return inputManager_; }
    scene::SceneManager& GetSceneManager() { return sceneManager_; }
    graphics::Renderer& GetRenderer() { return renderer_; }
    engine::graphics::SpriteRenderer& GetSpriteRenderer() { return *spriteRenderer_; }
    resources::ResourceManager& GetResourceManager() { return *resourceManager_; }
    
    // Convenience methods
    void RequestExit() { isRunning_ = false; }
    bool IsRunning() const { return isRunning_; }
    float GetDeltaTime() const { return deltaTime_; }
    float GetFPS() const { return fps_; }
    
    // Scene management shortcuts
    template<typename SceneType, typename... Args>
    void RegisterScene(const std::string& sceneId, Args&&... args) {
        sceneManager_.RegisterScene(sceneId, [args...]() {
            return std::make_unique<SceneType>(args...);
        });
    }
    
    void ChangeScene(const std::string& sceneId) {
        sceneManager_.RequestSceneChange(sceneId);
    }

private:
    // Core systems
    ECS::World world_;
    event::EventManager eventManager_;
    input::InputManager inputManager_;
    scene::SceneManager sceneManager_;
    graphics::Renderer renderer_;
    std::unique_ptr<engine::graphics::SpriteRenderer> spriteRenderer_;
    std::unique_ptr<resources::ResourceManager> resourceManager_;
    
    // Engine state
    bool isRunning_ = false;
    bool initialized_ = false;
    EngineConfig config_;
    
    // Timing
    Uint64 lastFrameTime_ = 0;
    float deltaTime_ = 0.0f;
    float fps_ = 0.0f;
    
    // Internal methods
    void InitializeSystems();
    void UpdateSystems();
    void HandleEvents();
    void UpdateTiming();
};

} // namespace engine
