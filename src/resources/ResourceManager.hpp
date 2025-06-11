// src/resources/ResourceManager.hpp
#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>


namespace resources {

class ResourceManager {
public:
    ResourceManager(SDL_Renderer* renderer);
    ~ResourceManager();
    
    // Prevent copying and reassigning
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    SDL_Texture* LoadTexture(const std::string& filePath);
    void ReleaseTexture(const std::string& filePath);
    // for debugging purpose now
    SDL_Texture* UnloadTexture(const std::string& filePath);  

    SDL_Texture* GetTexture(const std::string& filePath) const;
    std::string NormalizePath(const std::string& path) const;

    void SetFallbackTexture(SDL_Texture* texture);
    void UnloadAll();

    void PrintCacheStatus() const;  // debugger

private:
    // Pointer to the external SDL renderer, used for creating and managing textures
    SDL_Renderer* renderer_;
    SDL_Texture* fallbackTexture_ = nullptr;
    std::unordered_map<std::string, std::pair<SDL_Texture*, int>> textureCache_;
    
};

} // namespace resources
