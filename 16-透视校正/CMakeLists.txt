cmake_minimum_required(VERSION 3.12)
project(Graphic)
set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/slint")
find_package(Slint REQUIRED)

set(CMAKE_BUILD_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/slint/lib")

file(GLOB copyResources "./assets")
file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(application)
add_subdirectory(gpu)

add_executable(softRenderer "main.cpp")
target_link_libraries(softRenderer applicationLib gpuLib Slint::Slint)
target_include_directories(softRenderer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/slint/include")

slint_target_sources(softRenderer app.slint)
