// src/scene/Scene.cpp

#include <string>
#include "Scene.hpp"
#include <algorithm>
// 这里我们需要等你实现事件系统后再完善
// 暂时先留空或者直接调用全局的SceneManager

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