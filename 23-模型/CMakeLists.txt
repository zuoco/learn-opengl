cmake_minimum_required(VERSION 3.21)

project(Graphic)

set(CMAKE_CXX_STANDARD 17)

# Copy assets to build directory
file(GLOB copyResources "./assets")
file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})

# Slint SDK
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/slint")
find_package(Slint REQUIRED)

# System assimp
find_package(PkgConfig REQUIRED)
pkg_check_modules(ASSIMP REQUIRED assimp)

include_directories(
    SYSTEM ${ASSIMP_INCLUDE_DIRS}
)

link_directories(
    ${ASSIMP_LIBRARY_DIRS}
)

add_subdirectory(application)
add_subdirectory(gpu)

add_executable(softRenderer "main.cpp")

slint_target_sources(softRenderer ui/app-window.slint)

target_link_libraries(softRenderer applicationLib gpuLib Slint::Slint ${ASSIMP_LIBRARIES})
target_link_options(softRenderer PRIVATE ${ASSIMP_LDFLAGS_OTHER})

# Ensure slint shared library can be found at runtime
set_target_properties(softRenderer PROPERTIES
    BUILD_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/slint/lib"
)
