#!/bin/bash

# 俄罗斯方块构建脚本

echo "正在构建俄罗斯方块游戏..."

# 创建构建目录
mkdir -p build
cd build

# 配置项目
echo "正在配置项目..."
cmake ..

# 构建项目
echo "正在构建项目..."
cmake --build .

echo "构建完成！"
echo "可执行文件位于: build/bin/"
echo "运行游戏命令: ./bin/Tetris"