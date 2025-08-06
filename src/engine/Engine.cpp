#include "Engine.hpp"
#include <iostream>
#include <algorithm>
#include "core/ecs/systems/CollisionSystem.hpp"
#include "core/ecs/systems/PhysicsSystem.hpp"
#include "core/ecs/systems/LifetimeSystem.hpp"
#include "core/ecs/systems/RenderSystem.hpp"
#include "core/ecs/systems/DebugRenderSystem.hpp"
#include "core/ecs/systems/ParticleSystem.hpp"
#include "core/ecs/systems/AnimationSystem.hpp"
#include "core/ecs/systems/SpriteStateSystem.hpp"

namespace engine {

Engine::Engine() = default;

Engine::~Engine() {
    if (initialized_) {
        Shutdown();
    }
}

bool Engine::Initialize(const EngineConfig& config) {
    if (initialized_) {
        std::cerr << "[Engine] Already initialized!" << std::endl;
        return false;
    }
    
    config_ = config;
    
    // Initialize renderer first
    if (!renderer_.Init(config_.windowTitle, config_.windowWidth, config_.windowHeight)) {
        std::cerr << "[Engine] Failed to initialize renderer" << std::endl;
        return false;
    }
    
    // Initialize sprite renderer
    spriteRenderer_ = std::make_unique<graphics::SpriteRenderer>(renderer_.GetSDLRenderer());
    resourceManager_ = std::make_unique<resources::ResourceManager>(renderer_.GetSDLRenderer());
    
    // Initialize and connect systems
    InitializeSystems();
    
    initialized_ = true;
    lastFrameTime_ = SDL_GetTicks();
    
    std::cout << "[Engine] Successfully initialized" << std::endl;
    return true;
}

void Engine::Run() {
    if (!initialized_) {
        std::cerr << "[Engine] Not initialized! Call Initialize() first." << std::endl;
        return;
    }
    
    isRunning_ = true;
    std::cout << "[Engine] Starting main loop" << std::endl;
    
    while (isRunning_) {
        UpdateTiming();
        HandleEvents();
        UpdateSystems();
        // Render
        sceneManager_.Render(renderer_.GetSDLRenderer());
    }
    
    std::cout << "[Engine] Main loop ended" << std::endl;
}

void Engine::Shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "[Engine] Shutting down..." << std::endl;
    
    isRunning_ = false;
    
    world_->GetSystemManager().ClearAllSystems();
    world_->ClearAllEntities();
    
    spriteRenderer_.reset();
    resourceManager_.reset();
    renderer_.Shutdown();
    
    initialized_ = false;
    std::cout << "[Engine] Shutdown complete" << std::endl;
}

void Engine::InitializeSystems() {
    // Connect systems
    inputManager_.SetEventManager(&eventManager_);
    sceneManager_.SetEventManager(&eventManager_);
    sceneManager_.SetInputManager(&inputManager_);
    world_ = std::make_unique<engine::ECS::World>(&eventManager_);
    sceneManager_.SetWorld(world_.get());
    
    // **Add core ECS systems**
    auto& systemManager = world_->GetSystemManager();
    auto& eventManager = world_->GetEventManager();
    
    // 1. Add collision system (high priority - detect collisions first)
    auto collisionSystem = std::make_unique<ECS::CollisionSystem>();
    collisionSystem->SetEventManager(&eventManager);
    systemManager.AddSystem(std::move(collisionSystem), 10);
    
    // 2. Add physics system (medium priority - handle movement and collision response)
    auto physicsSystem = std::make_unique<ECS::PhysicsSystem>();
    systemManager.AddSystem(std::move(physicsSystem), 20);
    
    // 3. Add lifetime system (low priority - cleanup last)
    auto lifetimeSystem = std::make_unique<ECS::LifetimeSystem>();
    systemManager.AddSystem(std::move(lifetimeSystem), 30);
    
    // 4. Add particle system (before render system)
    auto particleSystem = std::make_unique<ECS::ParticleSystem>();
    systemManager.AddSystem(std::move(particleSystem), 40);
    
    // 4.3. Add sprite state system (before animation system)
    auto spriteStateSystem = std::make_unique<ECS::SpriteStateSystem>(resourceManager_.get());
    systemManager.AddSystem(std::move(spriteStateSystem), 44);
    
    // 4.5. Add animation system (before render system, after sprite state)
    auto animationSystem = std::make_unique<ECS::AnimationSystem>(resourceManager_.get());
    systemManager.AddSystem(std::move(animationSystem), 45);
    
    // 5. Add render system (lowest priority - render after all logic updates)
    auto renderSystem = std::make_unique<ECS::RenderSystem>(spriteRenderer_.get(), resourceManager_.get(), &renderer_);
    systemManager.AddSystem(std::move(renderSystem), 50);

    // 6. Add debug render system (lowest priority - render debug info on top)
    auto debugRenderSystem = std::make_unique<ECS::DebugRenderSystem>(renderer_.GetSDLRenderer(), &inputManager_);
    systemManager.AddSystem(std::move(debugRenderSystem), 100);
    
    std::cout << "[Engine] Core ECS systems initialized (including RenderSystem and DebugRenderSystem)" << std::endl;
}

void Engine::UpdateSystems() {
    // Don't clear input states before systems process them!
    // inputManager_.Update(); // Move this to end of frame
    eventManager_.Update();
    world_->Update(deltaTime_);
    sceneManager_.Update(deltaTime_);
    inputManager_.Update(); // Clear input states after systems have processed them
}

void Engine::HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            RequestExit();
        }
        
        // DEBUG: Log R key events at the engine level
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_R) {
            std::cout << "[Engine] R key event received from SDL!" << std::endl;
        }
        
        inputManager_.HandleEvent(event);
        sceneManager_.HandleEvent(event);
    }
}

void Engine::UpdateTiming() {
    Uint64 currentTime = SDL_GetTicks();
    deltaTime_ = (currentTime - lastFrameTime_) / 1000.0f;
    lastFrameTime_ = currentTime;
    
    // Calculate FPS
    if (deltaTime_ > 0.0f) {
        fps_ = 1.0f / deltaTime_;
    }
    
    // Cap frame rate if needed
    if (config_.targetFPS > 0) {
        float targetFrameTime = 1.0f / config_.targetFPS;
        if (deltaTime_ < targetFrameTime) {
            Uint32 delayTime = static_cast<Uint32>((targetFrameTime - deltaTime_) * 1000.0f);
            SDL_Delay(delayTime);
        }
    }
}

} // namespace engine