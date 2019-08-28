# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息
project (Demo)


# 如果源文件多的话,遍历目录文件 aux_source_directory(<dir> <variable>)
aux_source_directory(. SOURCES)

# 添加子目录
# 指明本项目包含一个子目录 math，这样 math 目录下的 CMakeLists.txt 文件和源代码也会被处理
add_subdirectory(math)

# 指定生成目标
add_executable(Demo ${SOURCES})
#add_executable(Demo main.cpp MathFunctions.cpp)

# 添加链接库
target_link_libraries(Demo MathFunctions)
