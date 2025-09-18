@echo off
REM 俄罗斯方块构建脚本 (Windows)

echo 正在构建俄罗斯方块游戏...

REM 创建构建目录
if not exist build mkdir build
cd build

REM 配置项目
echo 正在配置项目...
cmake ..

REM 构建项目
echo 正在构建项目...
cmake --build .

echo.
echo 构建完成！
echo 可执行文件位于: build\bin\
echo 运行游戏命令: build\bin\Tetris.exe
pause