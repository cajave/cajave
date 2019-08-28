# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息
project (Demo)


# 如果源文件多的话 aux_source_directory(<dir> <variable>)
aux_source_directory(. SOURCES)

# 指定生成目标
add_executable(Demo ${SOURCES})
#add_executable(Demo main.cpp MathFunctions.cpp)