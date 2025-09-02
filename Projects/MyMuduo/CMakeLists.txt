cmake_minimum_required(VERSION 3.0)
project(mymuduo)

# 设置库路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# 设置编译调试信息
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# 定义参与编译的源代码
aux_source_directory(. SRC_LIST)
# 编译生成动态库
add_library(mymuduo SHARED ${SRC_LIST})