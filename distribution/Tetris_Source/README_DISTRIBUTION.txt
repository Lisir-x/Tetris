俄罗斯方块游戏源代码分发包
==========================

感谢下载俄罗斯方块游戏源代码！

包内容说明：
- Tetri_SFML3.0.cpp: 游戏主程序源代码
- CMakeLists.txt: CMake构建配置文件
- README.md: 详细的项目说明和构建指南
- build.sh: Linux/macOS构建脚本
- build.bat: Windows构建脚本
- res/: 游戏资源文件夹（音频文件等）

如何构建和运行：

方法一：使用CMake（推荐）
1. 确保已安装SFML库和CMake 3.20+
2. 在项目根目录运行：
   - Windows: build.bat
   - Linux/macOS: ./build.sh
3. 运行生成的可执行文件

方法二：直接编译
Windows (MinGW):
  g++ -o Tetris.exe Tetri_SFML3.0.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

Linux:
  g++ -o Tetris Tetri_SFML3.0.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

macOS:
  g++ -o Tetris Tetri_SFML3.0.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -I/usr/local/include -L/usr/local/lib

系统要求：
- C++17兼容的编译器
- SFML 3.0或更高版本（图形、窗口、系统、音频模块）

注意：接收者需要自行安装SFML开发库才能构建此项目。