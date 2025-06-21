// src/engine/core/ecs/components/Tag.hpp

#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <vector>

namespace engine::ECS {

struct Tag {
    std::string tag;
    std::vector<std::string> tags;
};

} // namespace engine::ECS