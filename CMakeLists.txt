cmake_minimum_required(VERSION 3.16)
project(2DEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output dir to bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Automatically fetch SDL3 from GitHub
include(FetchContent)
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.16
)
FetchContent_MakeAvailable(SDL3)

# Collect all .cpp files in src/ recursively
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Define executable
add_executable(2DEngine ${ENGINE_SOURCES})

# Include the src directory so you can write #include "graphics/Renderer.hpp"
target_include_directories(2DEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Link SDL3
target_link_libraries(2DEngine PRIVATE SDL3::SDL3)
