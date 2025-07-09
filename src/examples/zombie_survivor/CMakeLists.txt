cmake_minimum_required(VERSION 3.16)
project(ZombieSurvivor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
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

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# Collect only engine core sources (exclude tests and examples)
file(GLOB_RECURSE ENGINE_CORE_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/../../engine/*.cpp")

# Collect zombie survivor sources
set(ZOMBIE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ZombieTestScene.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/systems/DamageSystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/systems/ExperienceSystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/systems/HealthSystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/systems/InputSystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/systems/MovementSystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/systems/UpgradeSystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/systems/WeaponSystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/systems/AmmoSystem.cpp"
)

add_executable(${PROJECT_NAME} ${ENGINE_CORE_SOURCES} ${ZOMBIE_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    SDL3::SDL3 
    SDL3_image::SDL3_image
    glm
)
