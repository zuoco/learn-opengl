#递归将本文件夹下所有cpp放到FUNCS中
file(GLOB_RECURSE APP ./  *.cpp)

#将FUNCS中所有cpp编译为funcs这个lib库
add_library(applicationLib ${APP} )

target_link_libraries(applicationLib PUBLIC Slint::Slint)
target_include_directories(applicationLib PUBLIC ${CMAKE_BINARY_DIR})

slint_target_sources(applicationLib ${CMAKE_CURRENT_SOURCE_DIR}/../ui/app.slint NAMESPACE ui)
