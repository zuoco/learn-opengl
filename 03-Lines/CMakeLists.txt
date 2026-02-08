cmake_minimum_required(VERSION 3.16)

project(LinesQt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(softRenderer
    main.cpp
    canvasprovider.h
    canvasprovider.cpp
    global/base.h
    gpu/frameBuffer.h
    gpu/frameBuffer.cpp
    gpu/raster.h
    gpu/raster.cpp
    gpu/gpu.h
    gpu/gpu.cpp
)

qt_add_qml_module(softRenderer
    URI LinesQt
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)

target_link_libraries(softRenderer PRIVATE Qt6::Quick)

set_target_properties(softRenderer PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
