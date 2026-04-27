#递归将本文件夹下所有cpp放到FUNCS中
file(GLOB_RECURSE APP ./  *.cpp)

#将FUNCS中所有cpp编译为funcs这个lib库
add_library(applicationLib ${APP})

target_include_directories(applicationLib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
    "${CMAKE_CURRENT_SOURCE_DIR}/../slint/include"
)
