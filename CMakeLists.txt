cmake_minimum_required(VERSION 3.16)
project(Jeff2DEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch SDL3 from GitHub
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.16
)
FetchContent_MakeAvailable(SDL3)

add_executable(Jeff2DEngine src/main.cpp)

target_link_libraries(Jeff2DEngine PRIVATE SDL3::SDL3)
