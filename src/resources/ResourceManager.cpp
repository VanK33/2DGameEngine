// src/resources/ResourceManager.cpp

#include "ResourceManager.hpp"
#include <SDL3_image/SDL_image.h>
#include <iostream>

namespace resources {

ResourceManager::ResourceManager(SDL_Renderer* renderer) 
    : renderer_(renderer) {}

ResourceManager::~ResourceManager() {
    UnloadAll();
}

SDL_Texture* ResourceManager::LoadTexture(const std::string& filePath) {

    SDL_Texture* cached = GetTexture(filePath);
    if (cached) {
        return cached;
    }

    SDL_Texture* texture = IMG_LoadTexture(renderer_, filePath.c_str());
    if (!texture) {
        SDL_Log("[ResourceManager] Failed to load image: %s — %s", filePath.c_str(), SDL_GetError());
        return nullptr;
    }

    textureCache_[filePath] = texture;
    return texture;
}


SDL_Texture* ResourceManager::GetTexture(const std::string& filePath) const {
    auto it = textureCache_.find(filePath);
    if (it != textureCache_.end()) {
        return it->second;
    }
    return nullptr;
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
