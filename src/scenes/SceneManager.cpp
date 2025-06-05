#include "SceneManager.hpp"
#include <iostream>

namespace scene {

SceneManager::SceneManager() : currentScene_(nullptr) {}

SceneManager::~SceneManager() {
    UnloadScene();
}

void SceneManager::RegisterScene(const std::string& sceneId, SceneFactory factory) {
    sceneFactories_[sceneId] = factory;
    std::cout << "[SceneManager] Registered scene: " << sceneId << std::endl;
}

void SceneManager::SetScene(std::unique_ptr<Scene> newScene) {
    if (currentScene_) {
        std::cout << "[SceneManager] Unloading scene: " << currentScene_->GetSceneId() << std::endl;
        currentScene_->Unload();
    }
    
    currentScene_ = std::move(newScene);
    
    if (currentScene_) {
        std::cout << "[SceneManager] Loading scene: " << currentScene_->GetSceneId() << std::endl;
        currentScene_->Load();
    }
}

void SceneManager::Update(float deltaTime) {
    if (sceneChangeRequested_) {
        auto newScene = CreateScene(nextSceneId_);
        if (newScene) {
            SetScene(std::move(newScene));
        } else {
            std::cout << "[SceneManager] Warning: Scene '" << nextSceneId_ << "' not found!" << std::endl;
        }
        sceneChangeRequested_ = false;
    }
    
    if (currentScene_) {
        currentScene_->Update(deltaTime);
    }
}

void SceneManager::Render(SDL_Renderer* renderer) {
    if (currentScene_) {
        currentScene_->Render(renderer);
    }
}

void SceneManager::HandleEvent(const SDL_Event& event) {
    if (currentScene_) {
        currentScene_->HandleEvent(event);
    }
}

void SceneManager::UnloadScene() {
    if (currentScene_) {
        std::cout << "[SceneManager] Cleaning up scene: " << currentScene_->GetSceneId() << std::endl;
        currentScene_->Unload();
        currentScene_.reset();
    }
}

void SceneManager::RequestSceneChange(const std::string& sceneId) {
    nextSceneId_ = sceneId;
    sceneChangeRequested_ = true;
}

std::string SceneManager::GetCurrentSceneId() const {
    return currentScene_ ? currentScene_->GetSceneId() : "";
}

std::unique_ptr<Scene> SceneManager::CreateScene(const std::string& sceneId) {
    auto it = sceneFactories_.find(sceneId);
    if (it != sceneFactories_.end()) {
        return it->second();  // 调用工厂函数
    }
    return nullptr;
}

} // namespace scene