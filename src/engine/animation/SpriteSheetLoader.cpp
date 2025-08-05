// src/engine/animation/SpriteSheetLoader.cpp

#include "SpriteSheetLoader.hpp"
#include <SDL3/SDL.h>
#include <iostream>
#include <cmath>
#include <vector>

namespace engine::animation {

SpriteSheetLoader::SpriteSheetLoader(engine::resources::ResourceManager* resourceManager)
    : resourceManager_(resourceManager) {
}

SpriteSheetInfo SpriteSheetLoader::AnalyzeSpriteSheet(const std::string& texturePath, int expectedFrameCount) {
    // Check cache first
    auto it = cache_.find(texturePath);
    if (it != cache_.end()) {
        return it->second;
    }
    
    // Analyze the sprite sheet
    SpriteSheetInfo info = DetectFrames(texturePath, expectedFrameCount);
    
    // Cache the result
    cache_[texturePath] = info;
    
    return info;
}

engine::ECS::SpriteAnimation SpriteSheetLoader::CreateAnimation(const std::string& texturePath, 
                                                                int expectedFrameCount, 
                                                                float frameDuration,
                                                                bool loop) {
    SpriteSheetInfo info = AnalyzeSpriteSheet(texturePath, expectedFrameCount);
    
    if (!info.isValid) {
        std::cout << "[SpriteSheetLoader] Warning: Could not analyze " << texturePath 
                  << ", using defaults" << std::endl;
        return engine::ECS::SpriteAnimation{
            1, 1, 32, 32, frameDuration, loop, true
        };
    }
    
    std::cout << "*** [SpriteSheetLoader] " << texturePath << ": " 
              << info.frameCount << " frames, " 
              << info.frameWidth << "x" << info.frameHeight << " each (total: " 
              << info.totalWidth << "x" << info.totalHeight << ") ***" << std::endl;
    
    return engine::ECS::SpriteAnimation{
        info.frameCount,    // frameCount
        info.frameCount,    // framesPerRow (assuming horizontal layout)
        info.frameWidth,    // frameWidth
        info.frameHeight,   // frameHeight
        frameDuration,      // frameDuration
        loop,               // loop
        true                // autoPlay
    };
}

engine::ECS::SpriteAnimation SpriteSheetLoader::CreateAnimation(const std::string& texturePath,
                                                                int frameCount,
                                                                int frameWidth,
                                                                int frameHeight,
                                                                float frameDuration,
                                                                bool loop) {
    return engine::ECS::SpriteAnimation{
        frameCount,
        frameCount,  // Assume single row
        frameWidth,
        frameHeight,
        frameDuration,
        loop,
        true
    };
}

SpriteSheetInfo SpriteSheetLoader::DetectFrames(const std::string& texturePath, int expectedFrameCount) {
    SpriteSheetInfo info;
    
    if (!resourceManager_) {
        return info; // isValid = false
    }
    
    // Load or get existing texture
    SDL_Texture* texture = resourceManager_->GetTexture(texturePath);
    if (!texture) {
        texture = resourceManager_->LoadTexture(texturePath);
    }
    
    if (!texture) {
        std::cout << "[SpriteSheetLoader] Error: Could not load texture " << texturePath << std::endl;
        return info;
    }
    
    // Get texture dimensions
    float width, height;
    if (!SDL_GetTextureSize(texture, &width, &height)) {
        std::cout << "[SpriteSheetLoader] Error: Could not get texture size for " << texturePath << std::endl;
        return info;
    }
    
    info.totalWidth = static_cast<int>(width);
    info.totalHeight = static_cast<int>(height);
    
    // Determine frame count
    if (expectedFrameCount > 0) {
        info.frameCount = expectedFrameCount;
    } else {
        info.frameCount = GuessFrameCount(info.totalWidth, info.totalHeight);
    }
    
    // Calculate frame dimensions (assuming horizontal layout)
    info.frameWidth = info.totalWidth / info.frameCount;
    info.frameHeight = info.totalHeight; // Single row
    
    // Validate
    if (info.frameWidth > 0 && info.frameHeight > 0 && info.frameCount > 0) {
        info.isValid = true;
    }
    
    return info;
}

int SpriteSheetLoader::GuessFrameCount(int totalWidth, int totalHeight) {
    // Check for specific known sprite patterns first
    if (totalWidth == 768 && totalHeight == 128) {
        return 6; // Zombie sprites
    }
    if (totalWidth == 960 && totalHeight == 128) {
        return 10; // Zombie walk sprites  
    }
    if (totalWidth == 384 && totalHeight == 64) {
        return 8; // Player sprites
    }
    
    // Common sprite sheet frame counts to try
    std::vector<int> commonFrameCounts = {8, 6, 4, 10, 12, 16, 5, 7, 9};
    
    for (int frameCount : commonFrameCounts) {
        int frameWidth = totalWidth / frameCount;
        
        // Check if this gives us reasonable square-ish frames
        if (frameWidth > 0 && std::abs(frameWidth - totalHeight) < totalHeight * 0.5f) {
            return frameCount;
        }
    }
    
    // Fallback: assume square frames
    if (totalHeight > 0) {
        return totalWidth / totalHeight;
    }
    
    return 1; // Safe fallback
}

} // namespace engine::animation