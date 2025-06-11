// src/resources/ResourceManager.cpp

#include "ResourceManager.hpp"
#include "utils/PathUtils.hpp"
#include <SDL3_image/SDL_image.h>
#include <iostream>

namespace resources {

ResourceManager::ResourceManager(SDL_Renderer* renderer) 
    : renderer_(renderer) {
    SDL_Log("[ResourceManager] Created ResourceManager instance: %p", this);
}

ResourceManager::~ResourceManager() {
    SDL_Log("[ResourceManager] Destroying ResourceManager instance: %p", this);
    UnloadAll();
}

SDL_Texture* ResourceManager::LoadTexture(const std::string& filePath) {
    if (!renderer_) {
        SDL_Log("[ResourceManager] Error: renderer is null!");
        return nullptr;
    }

    SDL_Log("[ResourceManager] Loading texture: %s", filePath.c_str());
    
    // 简化路径处理，不使用 std::filesystem
    std::string fullPath = utils::GetAssetsPath() + filePath;
    SDL_Log("[ResourceManager] Full path: %s", fullPath.c_str());
    
    // 检查是否已经加载过 - 使用原始文件路径作为键
    auto it = textureCache_.find(filePath);
    if (it != textureCache_.end() && it->second != nullptr) {
        SDL_Log("[ResourceManager] Returning cached texture: %s (%p)", filePath.c_str(), it->second);
        return it->second;
    }

    SDL_Log("[ResourceManager] About to call IMG_LoadTexture...");
    SDL_Texture* texture = IMG_LoadTexture(renderer_, fullPath.c_str());
    SDL_Log("[ResourceManager] IMG_LoadTexture returned: %p", texture);
    
    if (!texture) {
        SDL_Log("[ResourceManager] Failed to load image: %s — %s", fullPath.c_str(), SDL_GetError());
        return fallbackTexture_;
    }

    SDL_Log("[ResourceManager] About to store texture in cache...");
    textureCache_[filePath] = texture;  // 使用原始文件路径作为键
    SDL_Log("[ResourceManager] Texture stored in cache. Cache size: %zu", textureCache_.size());
    
    return texture;
}

SDL_Texture* ResourceManager::UnloadTexture(const std::string& filePath) {
    if (!renderer_) {
        return nullptr;
    }

    SDL_Log("[ResourceManager] Unloading texture: %s", filePath.c_str());
    auto it = textureCache_.find(filePath);
    
    if (it != textureCache_.end()) {
        if (it->second) {
            SDL_Log("[ResourceManager] Destroying texture: %s (%p)", filePath.c_str(), it->second);
            SDL_DestroyTexture(it->second);
        }
        textureCache_.erase(it);
    }
    return nullptr;
}

std::string ResourceManager::NormalizePath(const std::string& path) const {
    // 暂时直接返回原路径，避免 std::filesystem 的问题
    return path;
}

SDL_Texture* ResourceManager::GetTexture(const std::string& filePath) const {
    if (!renderer_) {
        return nullptr;
    }

    auto it = textureCache_.find(filePath);
    return (it != textureCache_.end()) ? it->second : nullptr;
}

void ResourceManager::SetFallbackTexture(SDL_Texture* texture) {
    fallbackTexture_ = texture;
}

void ResourceManager::UnloadAll() {
    SDL_Log("[ResourceManager] UnloadAll called, cache size: %zu", textureCache_.size());
    
    for (auto& pair : textureCache_) {
        if (pair.second) {
            SDL_Log("[ResourceManager] Destroying texture: %s (%p)", pair.first.c_str(), pair.second);
            SDL_DestroyTexture(pair.second);
        }
    }
    textureCache_.clear();
    SDL_Log("[ResourceManager] UnloadAll completed");
}

} // namespace resources