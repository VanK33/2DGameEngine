// src/examples/zombie_survivor/ecs/systems/InputSystem.cpp

#include "InputSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/ecs/components/Tag.hpp"
#include <cmath>

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
    return engine::Vector2{static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)};
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