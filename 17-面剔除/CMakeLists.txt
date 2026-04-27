cmake_minimum_required(VERSION 3.12)

project(Graphic)

set(CMAKE_CXX_STANDARD 17)

# Find Slint C++ SDK
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/slint")
find_package(Slint REQUIRED)

file(GLOB copyResources "./assets")
file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(application)
add_subdirectory(gpu)

add_executable(softRenderer "main.cpp")

# Compile .slint file and generate C++ headers
slint_target_sources(softRenderer "${CMAKE_CURRENT_SOURCE_DIR}/app.slint")

target_link_libraries(softRenderer applicationLib gpuLib Slint::Slint)
