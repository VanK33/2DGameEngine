// src/scene/WhiteScene.cpp

#include "WhiteScene.hpp"
#include <iostream>

namespace scene {

void WhiteScene::Load() {
    std::cout << "[WhiteScene] Loaded\n";
}

void WhiteScene::Unload() {
    std::cout << "[WhiteScene] Unloaded\n";
}

void WhiteScene::Update(float /*deltaTime*/) {
    // No update logic for now
}

void WhiteScene::Render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, bgColor_.r, bgColor_.g, bgColor_.b, bgColor_.a);
    SDL_RenderClear(renderer);
}

void WhiteScene::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_SPACE) {
        std::cout << "[WhiteScene] Space pressed, switching to TestScene\n";
        // RequestSceneChange("TestScene");  // 使用字符串ID
    }
}

} // namespace scene