#include "DebugScene.hpp"
#include "events/Event.hpp"
#include "events/EventType.hpp"
#include "events/SceneEvents.hpp"
#include <iostream>

namespace scene {

DebugScene::DebugScene(const std::string& id, SDL_Renderer* renderer) : sceneId_(id), resourceManager_(renderer) {}

void DebugScene::Load() {
    std::cout << "[DebugScene] Loaded: " << sceneId_ << "\n";

    texture_ = resourceManager_.LoadTexture("assets/test.jpg");
    if (!texture_) {
        std::cerr << "[DebugScene] Failed to load texture!\n";
    }
}

void DebugScene::Unload() {
    resourceManager_.UnloadAll();
}

void DebugScene::Update(float /*deltaTime*/) {}

void DebugScene::Render(SDL_Renderer* renderer) {
    if (texture_) {
        SDL_RenderTexture(renderer, texture_, nullptr, &dstRect_);
    }
}

void DebugScene::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_SPACE) {
        std::string next = (sceneId_ == "DebugA") ? "DebugB" : "DebugA";
        std::cout << "[DebugScene] SPACE pressed. Requesting scene change to: " << next << "\n";

        auto payload = std::make_shared<game::events::SceneChangeData>(next);
        if (eventManager_) {
            eventManager_->publish(std::make_shared<game::events::Event>(
                game::events::EventType::SCENE_CHANGE, payload));
        }
    }
}

std::string DebugScene::GetSceneId() const {
    return sceneId_;
}

}  // namespace scene
