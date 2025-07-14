cmake_minimum_required(VERSION 3.16)
project(2DEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# Collect sources (exclude build directories and sandbox main)
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Remove any build directory files that might have been included
list(FILTER ENGINE_SOURCES EXCLUDE REGEX ".*/build/.*")

# Remove sandbox main.cpp to avoid duplicate main functions
list(FILTER ENGINE_SOURCES EXCLUDE REGEX ".*/sandbox/main\\.cpp$")

add_executable(${PROJECT_NAME} ${ENGINE_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    SDL3::SDL3 
    SDL3_image::SDL3_image
    glm
)