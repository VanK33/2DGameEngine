// src/scene/Scene.cpp

#include <string>
#include "Scene.hpp"
// 这里我们需要等你实现事件系统后再完善
// 暂时先留空或者直接调用全局的SceneManager

namespace scene {

void Scene::RequestSceneChange(const std::string& sceneId) {
    // TODO: 通过事件系统发送场景切换请求
    // 暂时可以先打印日志
    // EventSystem::Publish(EngineEvent::SCENE_CHANGE_REQUEST, sceneId);
}

} // namespace scene