// src/examples/zombie_survivor/ecs/systems/InputSystem.hpp

#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/input/InputManager.hpp"
#include "../components/InputComponent.hpp"
#include <SDL3/SDL.h>

namespace ZombieSurvivor::System {

class InputSystem : public engine::ECS::System {
public:
    explicit InputSystem(engine::input::InputManager& inputManager);
    ~InputSystem() = default;

    void Init() override;
    void Update(float deltaTime) override;
    const char* GetName() const override { return "InputSystem"; }

private:
    engine::input::InputManager& inputManager_;

    engine::Vector2 GetMoveInputVector() const;
    engine::Vector2 GetMouseScreenPosition() const;
    bool IsShootButtonPressed() const;
    bool IsReloadButtonPressed() const;
    
    bool IsMovementKeyPressed(SDL_Keycode key) const;
    void ResetTriggerInputs(ZombieSurvivor::Component::InputComponent* input) const;
};

} // namespace ZombieSurvivor::System