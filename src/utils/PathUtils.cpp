#include "PathUtils.hpp"
#include <SDL3/SDL_filesystem.h>

namespace utils {

std::string GetAssetsPath() {
    const char* basePath = SDL_GetBasePath();
    if (basePath) {
        std::string fullPath(basePath);
        SDL_free((void*)basePath); // SDL API requires you to free it
        return fullPath + "assets/";
    }
    return "assets/";
}

}
