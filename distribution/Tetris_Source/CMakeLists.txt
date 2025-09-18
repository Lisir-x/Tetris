cmake_minimum_required(VERSION 3.20)
project(Tetris VERSION 1.0.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找SFML包
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Audio)

# 如果找不到SFML 3，尝试查找SFML 2
if(NOT SFML_FOUND)
    find_package(SFML 2 REQUIRED COMPONENTS graphics window system audio)
endif()

# 定义可执行文件
add_executable(Tetris
    Tetri_SFML3.0.cpp
)

# 链接SFML库
target_link_libraries(Tetris 
    sfml-graphics 
    sfml-window 
    sfml-system 
    sfml-audio
)

# 设置包含目录
target_include_directories(Tetris PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 复制资源文件到构建目录
file(COPY res DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# 跨平台特定设置
if(WIN32)
    # Windows特定设置
    target_compile_definitions(Tetris PRIVATE _WIN32)
    
    # 在Windows上，如果使用动态链接，复制DLL文件到输出目录
    if(SFML_FOUND AND NOT SFML_STATIC_LIBRARIES)
        # 获取SFML库路径
        get_target_property(SFML_GRAPHICS_LOCATION sfml-graphics LOCATION)
        get_filename_component(SFML_LIB_DIR ${SFML_GRAPHICS_LOCATION} DIRECTORY)
        
        # 复制DLL文件（如果存在）
        file(GLOB SFML_DLLS "${SFML_LIB_DIR}/*.dll")
        if(SFML_DLLS)
            file(COPY ${SFML_DLLS} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        endif()
    endif()
elseif(UNIX AND NOT APPLE)
    # Linux特定设置
    target_compile_definitions(Tetris PRIVATE __linux__)
elseif(APPLE)
    # macOS特定设置
    target_compile_definitions(Tetris PRIVATE __APPLE__)
endif()

# 设置安装规则
install(TARGETS Tetris DESTINATION bin)
install(DIRECTORY res DESTINATION bin)

# 设置CPack打包参数
set(CPACK_PACKAGE_NAME "Tetris")
set(CPACK_PACKAGE_VERSION ${CMAKE_PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "俄罗斯方块游戏 - 经典的俄罗斯方块实现")
set(CPACK_PACKAGE_CONTACT "Tetris Developer")
set(CPACK_PACKAGE_VENDOR "Tetris Project")

# Windows打包设置
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
endif()

# Linux打包设置
if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "TGZ")
endif()

# macOS打包设置
if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
endif()

include(CPack)