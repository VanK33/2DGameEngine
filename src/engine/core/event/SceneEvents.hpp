// src/events/SceneEvents.cpp

#pragma once
#include <string>

namespace engine::event {

struct SceneChangeData {
    std::string targetSceneId;

    explicit SceneChangeData(const std::string& id) : targetSceneId(id) {}
};

}  // namespace engine::events
