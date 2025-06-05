// src/graphics/Renderer.cpp

#include "Renderer.hpp"
#include <iostream>

namespace graphics {

Renderer::Renderer()
    : window_(nullptr), renderer_(nullptr) {}

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Init(const std::string& windowTitle, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "[Renderer] Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    window_ = SDL_CreateWindow(windowTitle.c_str(), width, height, SDL_WINDOW_OPENGL);
    if (!window_) {
        std::cerr << "[Renderer] Failed to create SDL Window: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer_ = SDL_CreateRenderer(window_, nullptr);
    if (!renderer_) {
        std::cerr << "[Renderer] Failed to create SDL Renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "[Renderer] SDL3 Initialized. Window and Renderer created.\n";
    return true;
}

void Renderer::Shutdown() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
    std::cout << "[Renderer] Shutdown complete.\n";
}

void Renderer::BeginFrame() {
    SDL_SetRenderDrawColor(renderer_, 30, 30, 30, 255); // Dark gray background
    SDL_RenderClear(renderer_);
}

void Renderer::EndFrame() {
    SDL_RenderPresent(renderer_);
}

void Renderer::DrawTexture(SDL_Texture* texture, int x, int y, int width, int height) {
    if (!texture) return;

    SDL_FRect dstRect = { static_cast<float>(x), static_cast<float>(y),
                          static_cast<float>(width), static_cast<float>(height) };

    SDL_RenderTexture(renderer_, texture, nullptr, &dstRect);
}

} // namespace graphics
