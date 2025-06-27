// src/scene/Scene.cpp

#include <string>
#include "Scene.hpp"
#include <algorithm>
// Here we need to wait for you to implement the event system before improving
// For now, leave it empty or call the global SceneManager directly

namespace engine::scene {

ECS::EntityID Scene::CreateSceneEntity(const std::string& name) {
    if (world_) {
        auto entityId = world_->GetEntityFactory().CreateEntity(name);
        sceneEntities_.push_back(entityId);
        return entityId;
    }
    return 0;
}


void Scene::DestroySceneEntity(ECS::EntityID id) {
    if (world_) {
        world_->GetEntityFactory().DestroyEntity(id);
        sceneEntities_.erase(
            std::remove(sceneEntities_.begin(), sceneEntities_.end(), id),
            sceneEntities_.end()
        );
    }
}


void Scene::ClearSceneEntities() {
    if (world_) {
        for (auto entityId: sceneEntities_) {
            world_->GetEntityFactory().DestroyEntity(entityId);
        }
        sceneEntities_.clear();
    }
}
    
    

} // namespace engine::scene