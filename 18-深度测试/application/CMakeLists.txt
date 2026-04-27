#����image.cpp����stb_image��װ
file(GLOB_RECURSE APP ./*.cpp)

#��FUNCS���ж�cpp���뱾��Ϊfuncs����lib��
add_library(applicationLib ${APP})
