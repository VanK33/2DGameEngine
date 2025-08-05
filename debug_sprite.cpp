// Quick debug program to check sprite sheet analysis
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL init failed" << std::endl;
        return 1;
    }
    
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cout << "IMG init failed" << std::endl;
        return 1;
    }
    
    // Load the sprite sheet
    SDL_Surface* surface = IMG_Load("assets/Walk/walk_down.png");
    if (!surface) {
        std::cout << "Failed to load image: " << IMG_GetError() << std::endl;
        return 1;
    }
    
    std::cout << "walk_down.png dimensions: " << surface->w << "x" << surface->h << std::endl;
    std::cout << "Calculated 8 frames: " << (surface->w / 8) << "x" << surface->h << " per frame" << std::endl;
    
    SDL_DestroySurface(surface);
    IMG_Quit();
    SDL_Quit();
    
    return 0;
}