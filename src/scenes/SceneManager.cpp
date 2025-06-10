// src/scenes/SceneManager.cpp

#include "SceneManager.hpp"
#include "events/SceneEvents.hpp"
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
    std::cout << "[SceneManager] Switching from " 
              << GetCurrentSceneId() << " to " << newScene->GetSceneId() << "\n";
              
    if (currentScene_) {
        std::cout << "[SceneManager] Unloading scene: " << currentScene_->GetSceneId() << std::endl;
        currentScene_->Unload();
    }
    
    currentScene_ = std::move(newScene);
    
    if (currentScene_) {
        std::cout << "[SceneManager] Loading scene: " << currentScene_->GetSceneId() << std::endl;

        if (eventManager_) {
            std::cout << "[SceneManager] Setting EventManager for scene..." << std::endl;
            currentScene_->SetEventManager(eventManager_);
            std::cout << "[SceneManager] EventManager set successfully!" << std::endl;
        } else {
            std::cout << "[SceneManager] ERROR: eventManager_ is null!" << std::endl;
        }

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

void SceneManager::onEvent(const std::shared_ptr<game::events::Event>& event) {
    if (event->getType() == game::events::EventType::SCENE_CHANGE) {
        auto data = std::static_pointer_cast<game::events::SceneChangeData>(event->getData());
        if (data) {
            RequestSceneChange(data->targetSceneId);
        }
    }
}


void SceneManager::SetEventManager(game::events::EventManager* manager) {
    eventManager_ = manager;
    if (eventManager_) {
        eventManager_->subscribe(game::events::EventType::SCENE_CHANGE, this);
    }
}

std::unique_ptr<Scene> SceneManager::CreateScene(const std::string& sceneId) {
    auto it = sceneFactories_.find(sceneId);
    if (it != sceneFactories_.end()) {
        return it->second();  // 调用工厂函数
    }
    return nullptr;
}

} // namespace scene