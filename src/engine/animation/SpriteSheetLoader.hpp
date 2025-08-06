// src/engine/animation/SpriteSheetLoader.hpp

#pragma once
#include <string>
#include <unordered_map>
#include "engine/resource/ResourceManager.hpp"
#include "engine/core/ecs/components/SpriteAnimation.hpp"

namespace engine::animation {

struct SpriteSheetInfo {
    int totalWidth = 0;
    int totalHeight = 0;
    int frameCount = 0;
    int frameWidth = 0;
    int frameHeight = 0;
    bool isValid = false;
};

class SpriteSheetLoader {
public:
    SpriteSheetLoader(engine::resources::ResourceManager* resourceManager);
    
    // Automatically detect sprite sheet properties
    SpriteSheetInfo AnalyzeSpriteSheet(const std::string& texturePath, int expectedFrameCount = 0);
    
    // Create SpriteAnimation component with auto-detected properties
    engine::ECS::SpriteAnimation CreateAnimation(const std::string& texturePath, 
                                                 int expectedFrameCount = 0, 
                                                 float frameDuration = 0.1f,
                                                 bool loop = true);
    
    // Manual frame specification (fallback)
    engine::ECS::SpriteAnimation CreateAnimation(const std::string& texturePath,
                                                 int frameCount,
                                                 int frameWidth,
                                                 int frameHeight,
                                                 float frameDuration = 0.1f,
                                                 bool loop = true);

private:
    engine::resources::ResourceManager* resourceManager_;
    std::unordered_map<std::string, SpriteSheetInfo> cache_;
    
    // Helper functions
    SpriteSheetInfo DetectFrames(const std::string& texturePath, int expectedFrameCount);
    int GuessFrameCount(int totalWidth, int totalHeight);
};

} // namespace engine::animation