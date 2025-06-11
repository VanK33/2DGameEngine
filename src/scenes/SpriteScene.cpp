//src/scenes/SprintScene.cpp

#include "SpriteScene.hpp"
#include <iostream>

namespace scene {

SpriteScene::SpriteScene(const std::string& id, SDL_Renderer* renderer)
    : sceneId_(id), resourceManager_(renderer) {}

void SpriteScene::Load() {
    sprite_ = resourceManager_.LoadTexture("img1.jpg");
    if (!sprite_) {
        std::cerr << "[SpriteScene] Failed to load sprite.\n";
    }
}

void SpriteScene::Unload() {
    sprite_ = nullptr;
    resourceManager_.UnloadAll();
}

void SpriteScene::Update(float deltaTime) {
    // Nothing to animate yet
}

void SpriteScene::Render(SDL_Renderer* renderer) {
    if (sprite_) {
        SDL_RenderTexture(renderer, sprite_, nullptr, &spriteRect_);
    }
}

void SpriteScene::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
            case SDLK_LEFT:
                spriteRect_.x -= moveSpeed_ * 0.016f;
                break;
            case SDLK_RIGHT:
                spriteRect_.x += moveSpeed_ * 0.016f;
                break;
            case SDLK_UP:
                spriteRect_.y -= moveSpeed_ * 0.016f;
                break;
            case SDLK_DOWN:
                spriteRect_.y += moveSpeed_ * 0.016f;
                break;
        }
    }
}

std::string SpriteScene::GetSceneId() const {
    return sceneId_;
}

} // namespace scene
