cmake_minimum_required(VERSION 3.16)

project(Graphic)

set(CMAKE_CXX_STANDARD 17)

file(GLOB copyResources "./assets")
file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})

set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/slint")
find_package(Slint REQUIRED)

add_subdirectory(application)
add_subdirectory(gpu)

add_executable(softRenderer main.cpp)
slint_target_sources(softRenderer app.slint)

target_link_libraries(softRenderer applicationLib gpuLib Slint::Slint)
