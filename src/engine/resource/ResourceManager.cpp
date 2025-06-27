#include "ResourceManager.hpp"
#include "engine/utils/PathUtils.hpp"
#include <SDL3_image/SDL_image.h>
#include <iostream>

namespace engine::resources {

ResourceManager::ResourceManager(SDL_Renderer* renderer) 
    : renderer_(renderer) {
    SDL_Log("[ResourceManager] Created ResourceManager instance: %p", this);
}

ResourceManager::~ResourceManager() {
    SDL_Log("[ResourceManager] Destroying ResourceManager instance: %p", this);
    UnloadAll();
}

SDL_Texture* ResourceManager::LoadTexture(const std::string& filePath) {
    std::string fullPath = utils::GetAssetsPath() + filePath;
    std::string normalizedPath = NormalizePath(fullPath);

    auto it = textureCache_.find(normalizedPath);
    if (it != textureCache_.end()) {
        it->second.second += 1; // ref++
        SDL_Log("[ResourceManager] Reusing texture: %s (ref count = %d)", 
                normalizedPath.c_str(), it->second.second);
        return it->second.first;
    }

#ifdef DEBUG
    auto start = std::chrono::high_resolution_clock::now();
#endif

    SDL_Texture* texture = IMG_LoadTexture(renderer_, fullPath.c_str());
    SDL_Log("[ResourceManager] IMG_LoadTexture returned: %p", texture);
    
    if (!texture) {
        SDL_Log("[ResourceManager] Failed to load image: %s — %s", 
                normalizedPath.c_str(), SDL_GetError());
        return fallbackTexture_;
    }

#ifdef DEBUG
    auto end = std::chrono::high_resolution_clock::now();
    SDL_Log("[Timing] Loaded %s in %lld ms", filePath.c_str(),
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
#endif

    textureCache_[normalizedPath] = {texture, 1}; // initialize counter at 1
    SDL_Log("[ResourceManager] Loaded texture: %s (ref count = 1)", normalizedPath.c_str());
    return texture;
}

void ResourceManager::ReleaseTexture(const std::string& filePath) {
    std::string fullPath = utils::GetAssetsPath() + filePath;
    std::string normalizedPath = NormalizePath(fullPath);

    auto it = textureCache_.find(normalizedPath);
    if (it == textureCache_.end()) {
        SDL_Log("[ResourceManager] Attempted to release non-existent texture: %s", normalizedPath.c_str());
        return;
    }

    if (it->second.second <= 0) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                    "[ResourceManager] Double release detected: %s", normalizedPath.c_str());
        return;
    }

    it->second.second -= 1; // ref--
    SDL_Log("[ResourceManager] Released texture: %s (ref count = %d)", 
            normalizedPath.c_str(), it->second.second);

    if (it->second.second == 0) {
        SDL_DestroyTexture(it->second.first);
        textureCache_.erase(it);
        SDL_Log("[ResourceManager] Texture destroyed: %s", normalizedPath.c_str());
    }
}

SDL_Texture* ResourceManager::UnloadTexture(const std::string& filePath) {
    std::string fullPath = utils::GetAssetsPath() + filePath;
    std::string normalizedPath = NormalizePath(fullPath);
    auto it = textureCache_.find(normalizedPath);
    if (it != textureCache_.end()) {
        if (it->second.second > 0) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                        "[ResourceManager] WARNING: Force-unloading texture with ref count > 0: %s (ref = %d)",
                        normalizedPath.c_str(), it->second.second);
        }

        if (it->second.first) {
            SDL_DestroyTexture(it->second.first);
        }
        textureCache_.erase(it);
    }
    return nullptr;
}

SDL_Texture* ResourceManager::GetTexture(const std::string& filePath) const {
    std::string fullPath = utils::GetAssetsPath() + filePath;
    std::string normalizedPath = NormalizePath(fullPath);
    auto it = textureCache_.find(normalizedPath);
    return (it != textureCache_.end()) ? it->second.first : nullptr;
}

std::string ResourceManager::NormalizePath(const std::string& path) const {
    // Temporarily return original path directly to avoid std::filesystem issues
    return path;
}

void ResourceManager::SetFallbackTexture(SDL_Texture* texture) {
    fallbackTexture_ = texture;
}

void ResourceManager::UnloadAll() {
    SDL_Log("[ResourceManager] UnloadAll called, cache size: %zu", textureCache_.size());
    
    for (auto& pair : textureCache_) {
        if (pair.second.first) {
            SDL_DestroyTexture(pair.second.first);
        }
    }
    textureCache_.clear();
    SDL_Log("[ResourceManager] UnloadAll completed");
}

void ResourceManager::PrintCacheStatus() const {
    SDL_Log("---- [ResourceManager] Texture Cache Status ----");
    for (const auto& pair : textureCache_) {
        SDL_Log("  %s → ref count = %d", pair.first.c_str(), pair.second.second);
    }
    SDL_Log("------------------------------------------------");
}

} // namespace resources