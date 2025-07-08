#!/bin/bash

echo "🚀 Building and running Zombie Survivor..."

# 创建并进入构建目录
mkdir -p build && cd build

# 运行 CMake 并构建项目
cmake ..
if [ $? -ne 0 ]; then
    echo "❌ CMake configuration failed!"
    exit 1
fi

make
if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi

echo "✅ Build successful!"

# ✅ 确保 runtime 可以找到 SDL3
export DYLD_LIBRARY_PATH=./_deps/sdl3-build:$DYLD_LIBRARY_PATH

# ✅ 复制根目录的资源文件到可执行文件目录
# zombie_survivor 可能需要访问根目录的 assets
if [ -d "../../../assets" ]; then
    echo "📁 Copying assets from root directory..."
    cp -r ../../../assets bin/assets
fi

# ✅ 运行程序
echo "🎮 Starting Zombie Survivor..."
cd bin
./ZombieSurvivor

# 返回原始目录
cd ../..
