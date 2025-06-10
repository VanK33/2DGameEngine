// src/scene/TestScene.cpp

#include "TestScene.hpp"
#include <iostream>

namespace scene {

void TestScene::Load() {
    std::cout << "[TestScene] Loaded\n";
}

void TestScene::Unload() {
    std::cout << "[TestScene] Unloaded\n";
}

void TestScene::Update(float /*deltaTime*/) {
    // No update logic for now
}

void TestScene::Render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, bgColor_.r, bgColor_.g, bgColor_.b, bgColor_.a);
    SDL_RenderClear(renderer);
}

void TestScene::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_SPACE) {
        std::cout << "[TestScene] Space pressed, switching to WhiteScene\n";
        // RequestSceneChange("WhiteScene");  // 使用字符串ID
    }
}

} // namespace scene