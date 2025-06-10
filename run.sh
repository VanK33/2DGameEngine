#!/bin/bash

# 创建并进入构建目录
mkdir -p build && cd build

# 运行 CMake 并构建项目
cmake ..
make

# ✅ 确保 runtime 可以找到 SDL3（如果用到了）
export DYLD_LIBRARY_PATH=./_deps/sdl3-build:$DYLD_LIBRARY_PATH

# ✅ 回到项目根目录复制资源
cd ..
cp -r assets build/assets

# ✅ 回到构建目录并运行程序
cd build
./bin/2DEngine
