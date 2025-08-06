// src/examples/zombie_survivor/ecs/systems/InputSystem.cpp

#include "InputSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/Tag.hpp"
#include "examples/zombie_survivor/ecs/components/HealthComponent.hpp"
#include <cmath>
#include <algorithm>

namespace ZombieSurvivor::System {

InputSystem::InputSystem(engine::input::InputManager& inputManager)
    : inputManager_(inputManager) {}

void InputSystem::Init() {

}

void InputSystem::Update(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;

    auto& componentManager = world->GetComponentManager();
    auto entities = componentManager.GetEntitiesWithComponent<ZombieSurvivor::Component::InputComponent>();

    for (const auto& entityId : entities) {
        auto* input = componentManager.GetComponent<ZombieSurvivor::Component::InputComponent>(entityId);
        if (!input) continue;

        auto* tag = componentManager.GetComponent<engine::ECS::Tag>(entityId);
        if (!tag || tag->tag != "player") {
            continue;
        }

        ResetTriggerInputs(input);

        input->moveInput = GetMoveInputVector();
        input->mousePosition = GetMouseScreenPosition();

        if (IsShootButtonPressed()) {
            input->shootButtonPressed = true;
        }

        if (IsReloadButtonPressed()) {
            input->reloadButtonPressed = true;
        }
        
        // 测试血量减少 - 按X键减血，按C键加血
        if (inputManager_.IsKeyDown(SDLK_X)) {
            auto* health = componentManager.GetComponent<ZombieSurvivor::Component::HealthComponent>(entityId);
            if (health && health->health > 0) {
                health->health = std::max(0.0f, health->health - 10.0f);  // 每次减10血
                std::cout << "[InputSystem] Health decreased to: " << health->health << "/" << health->maxHealth << std::endl;
            }
        }
        
        if (inputManager_.IsKeyDown(SDLK_C)) {
            auto* health = componentManager.GetComponent<ZombieSurvivor::Component::HealthComponent>(entityId);
            if (health && health->health < health->maxHealth) {
                health->health = std::min(health->maxHealth, health->health + 10.0f);  // 每次加10血
                std::cout << "[InputSystem] Health increased to: " << health->health << "/" << health->maxHealth << std::endl;
            }
        }
    }
}

bool InputSystem::IsMovementKeyPressed(SDL_Keycode key) const {
    return inputManager_.IsKeyHeld(key);
}

bool InputSystem::IsShootButtonPressed() const {
    return inputManager_.IsMouseButtonHeld(SDL_BUTTON_LEFT);
}

bool InputSystem::IsReloadButtonPressed() const {
    bool result = inputManager_.IsKeyDown(SDLK_R);
    if (result) {
        std::cout << "[InputSystem] SDLK_R detected by InputManager!" << std::endl;
    }
    return result;
}

engine::Vector2 InputSystem::GetMouseScreenPosition() const {
    auto mousePos = inputManager_.GetMousePosition();
    
    // Convert window coordinates to game world coordinates by subtracting viewport offset
    const float GAME_WORLD_OFFSET_X = 231.0f; // (1312-850)/2
    const float GAME_WORLD_OFFSET_Y = 66.0f;  // (982-850)/2
    
    float gameWorldX = static_cast<float>(mousePos.x) - GAME_WORLD_OFFSET_X;
    float gameWorldY = static_cast<float>(mousePos.y) - GAME_WORLD_OFFSET_Y;
    
    return engine::Vector2{gameWorldX, gameWorldY};
}

engine::Vector2 InputSystem::GetMoveInputVector() const {
    bool upPressed = inputManager_.IsKeyHeld(SDLK_W) || inputManager_.IsKeyHeld(SDLK_UP);
    bool downPressed = inputManager_.IsKeyHeld(SDLK_S) || inputManager_.IsKeyHeld(SDLK_DOWN);
    bool leftPressed = inputManager_.IsKeyHeld(SDLK_A) || inputManager_.IsKeyHeld(SDLK_LEFT);
    bool rightPressed = inputManager_.IsKeyHeld(SDLK_D) || inputManager_.IsKeyHeld(SDLK_RIGHT);
    
    
    engine::Vector2 moveInput{0.0f, 0.0f};
    
    if (upPressed && !downPressed) moveInput.y = -1.0f;
    else if (downPressed && !upPressed) moveInput.y = 1.0f;
    
    if (leftPressed && !rightPressed) moveInput.x = -1.0f;
    else if (rightPressed && !leftPressed) moveInput.x = 1.0f;
    
    // Normalize diagonal movement
    if (moveInput.x != 0.0f && moveInput.y != 0.0f) {
        float length = std::sqrt(moveInput.x * moveInput.x + moveInput.y * moveInput.y);
        moveInput.x /= length;
        moveInput.y /= length;
    }
    
    return moveInput;
}

void InputSystem::ResetTriggerInputs(ZombieSurvivor::Component::InputComponent* input) const {
    input->shootButtonPressed = false;
    input->reloadButtonPressed = false;
}
} //namespace ZombieSurvivor::System