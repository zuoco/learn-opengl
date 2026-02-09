cmake_minimum_required(VERSION 3.21)

project(Graphic VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 拷贝资源文件到构建目录
file(GLOB copyResources "./assets")
file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})

# 使用本地 slint
set(Slint_DIR "${CMAKE_CURRENT_SOURCE_DIR}/slint/lib/cmake/Slint")
find_package(Slint REQUIRED)

# 查找依赖
find_package(Freetype REQUIRED)
find_package(Fontconfig REQUIRED)
find_package(PkgConfig REQUIRED)

set(SOURCES
    main.cpp
    application/application.h
    application/application.cpp
    application/image.h
    application/image.cpp
    gpu/gpu.h
    gpu/gpu.cpp
    gpu/frameBuffer.h
    gpu/frameBuffer.cpp
    gpu/raster.h
    gpu/raster.cpp
)

add_executable(softRenderer ${SOURCES})

target_include_directories(softRenderer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/global
    ${CMAKE_CURRENT_SOURCE_DIR}/application
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu
)

pkg_check_modules(LIBINPUT REQUIRED libinput)
pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)
pkg_check_modules(UDEV REQUIRED libudev)
pkg_check_modules(GBM REQUIRED gbm)
pkg_check_modules(SEAT REQUIRED libseat)

target_link_libraries(softRenderer PRIVATE
    Slint::Slint
    Fontconfig::Fontconfig
    Freetype::Freetype
    ${LIBINPUT_LIBRARIES}
    ${XKBCOMMON_LIBRARIES}
    ${UDEV_LIBRARIES}
    ${GBM_LIBRARIES}
    ${SEAT_LIBRARIES}
)

# 编译 Slint UI 文件
slint_target_sources(softRenderer ui/main.slint)
