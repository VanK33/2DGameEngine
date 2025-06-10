// src/scenes/DebugsScene.cpp

#include "DebugScene.hpp"
#include "events/Event.hpp"
#include "events/EventType.hpp"
#include "events/SceneEvents.hpp"
#include <iostream>
#include <SDL3/SDL.h>

namespace scene {

DebugScene::DebugScene(std::string id) : sceneId_(std::move(id)) {}

void DebugScene::Load() {
    std::cout << "[DebugScene] Loaded: " << sceneId_ << std::endl;
}

void DebugScene::Unload() {
    std::cout << "[DebugScene] Unloaded: " << sceneId_ << std::endl;
}

void DebugScene::Update(float /*deltaTime*/) {
    // Nothing yet
}

void DebugScene::Render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);
}

void DebugScene::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_SPACE) {
        std::string next = (sceneId_ == "DebugA") ? "DebugB" : "DebugA";
        std::cout << "[DebugScene] SPACE pressed. Requesting scene change to: " << next << "\n";

        auto payload = std::make_shared<game::events::SceneChangeData>(next);
        if (eventManager_) {
            eventManager_->publish(std::make_shared<game::events::Event>(
                game::events::EventType::SCENE_CHANGE,
                payload
            ));
        }
    }
}

std::string DebugScene::GetSceneId() const {
    return sceneId_;
}

} // namespace scene
