# 仅编译图像加载模块
file(GLOB_RECURSE APP ./*.cpp)
add_library(applicationLib ${APP})
