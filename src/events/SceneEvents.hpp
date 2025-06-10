// src/events/SceneEvents.cpp

#pragma once
#include <string>

namespace game {
namespace events {

struct SceneChangeData {
    std::string targetSceneId;

    explicit SceneChangeData(const std::string& id) : targetSceneId(id) {}
};

}  // namespace events
}  // namespace game
