cmake_minimum_required(VERSION 3.16)

project(Graphic VERSION 1.0.0 LANGUAGES CXX)

# 设置编译器为 clang
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# 导出 compile_commands.json 用于 clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml)

# 源文件
set(SOURCES
    main.cpp
    application/application.h
    application/application.cpp
    gpu/gpu.h
    gpu/gpu.cpp
    gpu/frameBuffer.h
    gpu/frameBuffer.cpp
)

# QML 资源
set(QML_RESOURCES
    qml/qml.qrc
)

qt_add_executable(softRenderer
    ${SOURCES}
    ${QML_RESOURCES}
)

target_include_directories(softRenderer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/global
    ${CMAKE_CURRENT_SOURCE_DIR}/application
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu
)

target_link_libraries(softRenderer PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Qml
)
