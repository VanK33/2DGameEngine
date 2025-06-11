// src/resources/ResourceManager.cpp

#include "ResourceManager.hpp"
#include "utils/PathUtils.hpp"
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <filesystem>

namespace resources {

ResourceManager::ResourceManager(SDL_Renderer* renderer) 
    : renderer_(renderer) {}

ResourceManager::~ResourceManager() {
    UnloadAll();
}

#ifdef DEBUG
#include <chrono>
auto start = std::chrono::high_resolution_clock::now();
...
auto end = std::chrono::high_resolution_clock::now();
SDL_Log("[Timing] Loaded %s in %lld ms", filePath.c_str(),
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
#endif

SDL_Texture* ResourceManager::LoadTexture(const std::string& filePath) {

    std::string fullPath = utils::GetAssetsPath() + filePath;
    std::string normalizedPath = NormalizePath(filePath);
    SDL_Texture* cached = GetTexture(normalizedPath);
    if (cached) {
        return cached;
    }

    SDL_Texture* texture = IMG_LoadTexture(renderer_, fullPath.c_str());
    if (!texture) {
        SDL_Log("[ResourceManager] Failed to load image: %s — %s", normalizedPath.c_str(), SDL_GetError());
        return fallbackTexture_;
    }

    textureCache_[normalizedPath] = texture;
    return texture;
}

SDL_Texture* ResourceManager::UnloadTexture(const std::string& filePath) {
    
    std::string normalizedPath = NormalizePath(filePath);
    auto it = textureCache_.find(normalizedPath);
    if (it !=textureCache_.end()) {
        if (it->second) {
            SDL_DestroyTexture(it->second);
        }
        textureCache_.erase(it);
        SDL_Log("[ResourceManager] Unloaded texture : %s", normalizedPath.c_str());
    }
}

std::string ResourceManager::NormalizePath(const std::string& path) const {
    try {
        return std::filesystem::absolute(path).lexically_normal().string();
    } catch (...) {
        return path;  // fallback
    }
}

SDL_Texture* ResourceManager::GetTexture(const std::string& filePath) const {
    
    std::string normalizedPath = NormalizePath(filePath);
    auto it = textureCache_.find(normalizedPath);
    if (it != textureCache_.end()) {
        return it->second;
    }
    return nullptr;
}

void ResourceManager::SetFallbackTexture(SDL_Texture* texture) {
    fallbackTexture_ = texture;
}

void ResourceManager::UnloadAll() {
    for (auto& pair : textureCache_) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textureCache_.clear();
    SDL_Log("[ResourceManager] Unloaded all textures.");
}

} // namespace resources
