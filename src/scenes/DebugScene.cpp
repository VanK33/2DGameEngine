#include "DebugScene.hpp"
#include "events/Event.hpp"
#include "events/EventType.hpp"
#include "events/SceneEvents.hpp"
#include "utils/PathUtils.hpp"
#include <iostream>
#include <cassert>

namespace scene {

DebugScene::DebugScene(const std::string& id, SDL_Renderer* renderer, graphics::SpriteRenderer* spriteRenderer) : sceneId_(id), resourceManager_(renderer), spriteRenderer_(spriteRenderer) {}

void DebugScene::Load() {
    std::cout << "[DebugScene] Loaded: " << sceneId_ << "\n";
    assert(spriteRenderer_ && "SpriteRenderer not set!");

    imagePaths_ = {
        "img1.jpg",
        "img2.jpg",
        "img3.jpg"
    };

    for (const auto& path : imagePaths_) {
        SDL_Texture* tex = resourceManager_.LoadTexture(path);
        if (tex) {
            textures_.push_back(tex);
            SDL_Log("[DebugScene] Loaded texture: %s", path.c_str());
        }
    }

    if (!textures_.empty()) {
        currentTextureIndex_ = 0;
    }

    dstRect_ = {100.0f, 100.0f, 256.0f, 256.0f}; 
}

void DebugScene::Unload() {
    // release ex
    // resourceManager->ReleaseTexture("example.png");

    for (const auto& path : imagePaths_) {
        resourceManager_.ReleaseTexture(path);
    }

    textures_.clear();
    imagePaths_.clear();
    currentTextureIndex_ = 0;

    resourceManager_.PrintCacheStatus(); // debugger
}

void DebugScene::Update(float /*deltaTime*/) {
    if (!textures_.empty() && inputManager_) {
        if (inputManager_->IsKeyDown(SDLK_RIGHT)) {
            currentTextureIndex_ = (currentTextureIndex_ + 1) % textures_.size();
            SDL_Log("[DebugScene] Switched to texture index: %d", currentTextureIndex_);
        }

        if (inputManager_->IsKeyDown(SDLK_LEFT)) {
            currentTextureIndex_ = (currentTextureIndex_ - 1 + textures_.size()) % textures_.size();
            SDL_Log("[DebugScene] Switched to texture index: %d", currentTextureIndex_);
        }
    }
}

void DebugScene::Render(SDL_Renderer* /*renderer*/) {
    if (!textures_.empty() && spriteRenderer_) {
        spriteRenderer_->Draw(textures_[currentTextureIndex_],
                              dstRect_.x, dstRect_.y, dstRect_.w, dstRect_.h);
    }
}


void DebugScene::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.key == SDLK_RIGHT) {
            currentTextureIndex_ = (currentTextureIndex_ + 1) % textures_.size();
            SDL_Log("[DebugScene] Switched to texture index: %d", currentTextureIndex_);
        } else if (event.key.key == SDLK_LEFT) {
            currentTextureIndex_ = (currentTextureIndex_ - 1 + textures_.size()) % textures_.size();
            SDL_Log("[DebugScene] Switched to texture index: %d", currentTextureIndex_);
        }
    }
}
void DebugScene::SetEventManager(game::events::EventManager* manager) {
    eventManager_ = manager;
    std::cout << "[DebugScene] EventManager set to: " << (manager ? "valid" : "null") << std::endl;
}

std::string DebugScene::GetSceneId() const {
    return sceneId_;
}

}  // namespace scene