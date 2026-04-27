#需求的最低cmake程序版本
cmake_minimum_required(VERSION 3.12)

#本工程的名字
project(Graphic)

#本工程支持的C++版本
set(CMAKE_CXX_STANDARD 17)

# 设置Slint路径
set(SLINT_COMPILER "${CMAKE_CURRENT_SOURCE_DIR}/third_party/slint/bin/slint-compiler" CACHE STRING "Path to the slint-compiler executable")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/slint/lib/cmake/Slint")
find_package(Slint REQUIRED)

file(GLOB copyResources "./assets")
file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(application)
add_subdirectory(gpu)

#本工程所有cpp文件编译链接，生成exe
add_executable(softRenderer "main.cpp")

# 编译Slint UI文件
slint_target_sources(softRenderer "ui/appwindow.slint")

target_link_libraries(softRenderer applicationLib gpuLib Slint::Slint)
