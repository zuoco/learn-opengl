file(GLOB_RECURSE APP ./ *.cpp)

add_library(applicationLib ${APP})

target_link_libraries(applicationLib PUBLIC Slint::Slint)
