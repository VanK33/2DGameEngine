// src/graphics/Renderer.hpp

#pragma once 

#include <SDL3/SDL.h>
#include <string>

// src/graphics/Renderer.hpp
#pragma once

#include <SDL3/SDL.h>
#include <string>

namespace graphics {

class Renderer {
public:
    Renderer();
    ~Renderer();

    // 初始化窗口和渲染器
    bool Init(const std::string& windowTitle, int width, int height);

    // 清理资源
    void Shutdown();

    // 每帧开始：清空画面
    void BeginFrame();

    // 每帧结束：提交渲染内容
    void EndFrame();

    // 渲染一张贴图到屏幕（简单版本）
    void DrawTexture(SDL_Texture* texture, int x, int y, int width, int height);

    // 供外部系统获取 SDL_Renderer*
    SDL_Renderer* GetSDLRenderer() const { return renderer_; }

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
};

} // namespace graphics
