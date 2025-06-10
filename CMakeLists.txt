cmake_minimum_required(VERSION 3.16)
project(2DEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# SDL3
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.16
)
FetchContent_MakeAvailable(SDL3)

# SDL3_image
FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-3.2.4
)
set(SDL3IMAGE_SDL3_REQUIRED ON)
set(SDL3IMAGE_INSTALL OFF)
FetchContent_MakeAvailable(SDL3_image)

# Collect sources
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(2DEngine ${ENGINE_SOURCES})

target_include_directories(2DEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(2DEngine PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)