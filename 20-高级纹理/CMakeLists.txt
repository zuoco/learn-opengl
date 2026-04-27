#需求的最低cmake程序版本
cmake_minimum_required(VERSION 3.12)

#本工程的名字
project(Graphic)

#本工程支持的C++版本
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/slint")
find_package(Slint REQUIRED)

file(GLOB copyResources "./assets")
file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(application)
add_subdirectory(gpu)

#本工程所有cpp文件编译链接，生成exe
add_executable(softRenderer "main.cpp")

target_link_libraries(softRenderer applicationLib gpuLib Slint::Slint)

slint_target_sources(softRenderer ui/app.slint)
