// src/graphics/Renderer.hpp
#pragma once

#include <SDL3/SDL.h>
#include <string>

namespace engine::graphics {

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool Init(const std::string& windowTitle, int width, int height);

    void Shutdown();

    void BeginFrame();

    void EndFrame();

    void DrawTexture(SDL_Texture* texture, int x, int y, int width, int height);

    SDL_Renderer* GetSDLRenderer() const { return renderer_; }

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
};

} // namespace engine::graphics
