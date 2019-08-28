# CMake 最低版本号要求
cmake_minimum_required(VERSION 2.8)
# 项目信息
project(Demo)

# 加入一个配置头文件，用于处理CMake对源码的设置
configure_file(
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
	)
include_directories(${PROJECT_BINARY_DIR})

# 是否使用自己的MathFunctions库
option(USE_MYMATH
	   "Use provided math implementation" ON)
	   
# 是否加入MathFunctions库
if (USE_MYMATH)
	include_directories("${PROJECT_SOURDE_DIR}/math")
	# 添加子目录 指明本项目包含一个子目录 math，这样 
	# math 目录下的CMakeLists.txt 文件和源代码也会被处理
	add_subdirectory(math)
	set(EXTRA_LIBS ${EXTRA_LIBS} MathFunctions)
endif(USE_MYMATH)	
	
# 如果源文件多的话,遍历目录文件 aux_source_directory(<dir> <variable>)
aux_source_directory(. SOURCES)

# 指定生成目标
add_executable(Demo ${SOURCES})

# 添加链接库
target_link_libraries(Demo ${EXTRA_LIBS})
