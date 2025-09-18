# 俄罗斯方块游戏

这是一个使用C++和SFML库开发的俄罗斯方块游戏。

## 功能特性

- 经典俄罗斯方块玩法
- 分数系统
- 等级系统（随消除行数增加而加快速度）
- 背景音乐和音效
- 暂停/继续功能
- 重新开始功能

## 控制说明

- A/← : 左移
- D/→ : 右移
- W/↑ : 旋转
- S/↓ : 软降落（加速下降）
- 空格键 : 硬降落（立即下落）
- P : 暂停/继续
- R : 重新开始
- ESC : 退出游戏

## 构建说明

### 使用CMake构建（推荐）

#### 先决条件

1. C++17兼容的编译器（GCC 7+, Clang 5+, MSVC 2017+）
2. CMake 3.20或更高版本
3. SFML 3.0或更高版本

#### 安装SFML

##### Windows
使用vcpkg:
```bash
vcpkg install sfml
```

或者从SFML官网下载并安装: https://www.sfml-dev.org/

##### Ubuntu/Debian
```bash
sudo apt-get install libsfml-dev
```

##### macOS (使用Homebrew)
```bash
brew install sfml
```

#### 构建步骤

1. 克隆或下载项目源代码
2. 创建构建目录:
   ```bash
   mkdir build
   cd build
   ```
3. 配置项目:
   ```bash
   cmake ..
   ```
4. 构建项目:
   ```bash
   cmake --build .
   ```
5. 运行游戏:
   ```bash
   ./bin/Tetris
   ```

### 使用传统方式构建

#### Windows (MinGW)
```bash
g++ -o Tetris.exe Tetri_SFML3.0.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```

#### Linux
```bash
g++ -o Tetris Tetri_SFML3.0.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```

#### macOS
```bash
g++ -o Tetris Tetri_SFML3.0.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -I/usr/local/include -L/usr/local/lib
```

## 打包和分发

使用CPack创建安装包:
```bash
cmake --build . --target package
```

这将生成适合您平台的安装包（Windows上的ZIP文件，Linux上的TGZ文件，macOS上的dmg文件）。

## 项目结构

```
.
├── CMakeLists.txt          # CMake构建配置
├── Tetri_SFML3.0.cpp       # 主程序源代码
├── res/                    # 资源文件
│   ├── bgm.mp3             # 背景音乐
│   ├── clear.MP3           # 消除音效
│   └── gameover.MP3        # 游戏结束音效
└── README.md               # 项目说明文档
```

## 故障排除

### SFML未找到
确保SFML已正确安装，并且CMake能够找到它。您可能需要设置CMAKE_PREFIX_PATH变量:
```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/sfml ..
```

### 链接错误
确保链接了所有必需的SFML模块: graphics, window, system, audio。

### 运行时错误
确保所有必需的DLL（Windows）或共享库（Linux/macOS）与可执行文件在同一目录中，或者在系统的库路径中。

## 许可证

本项目仅供学习和参考使用。