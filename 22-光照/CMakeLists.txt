# 需求的最低cmake程序版本
cmake_minimum_required(VERSION 3.21)

# 本工程的名字
project(Graphic)

# 本工程支持的C++版本
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Slint SDK 路径（项目内预编译库）
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/slint/lib/cmake/Slint")
find_package(Slint REQUIRED)

file(GLOB copyResources "./assets")
file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(application)
add_subdirectory(gpu)

# 本工程所有cpp文件编译链接，生成exe
add_executable(softRenderer "main.cpp")

# 编译 slint UI 文件
slint_target_sources(softRenderer ui/appwindow.slint)

# 链接库
target_link_libraries(softRenderer PRIVATE applicationLib gpuLib Slint::Slint)

# 设置运行时库搜索路径，确保能找到 libslint_cpp.so
set_target_properties(softRenderer PROPERTIES
    BUILD_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/slint/lib"
    INSTALL_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/slint/lib"
)
