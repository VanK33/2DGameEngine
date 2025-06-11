// src/utils/PathUtils.cpp

#include "PathUtils.hpp"
#include <SDL3/SDL_filesystem.h>

namespace utils {

std::string GetAssetsPath() {
    static std::string cachedPath;
    if (cachedPath.empty()) {
        const char* raw = SDL_GetBasePath();            // <- note const
        if (raw) {
            cachedPath = std::string(raw) + "assets/";
            SDL_free((void*)raw);                       // cast away const here
        } else {
            cachedPath = "assets/";
        }
    }
    return cachedPath;
}

} // namespace utils
