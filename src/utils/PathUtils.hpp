// src/utils/PathUtils.hpp
#pragma once
#include <string>

namespace utils {
  /// Returns "<exe-dir>/assets/" (cached after the first SDL_GetBasePath() call).
  std::string GetAssetsPath();
}