cmake_minimum_required(VERSION 3.20)
project(http_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)              
set(CMAKE_CXX_STANDARD_REQUIRED ON)     

file(GLOB_RECURSE SOURCE_FILES src/*.cc)

add_library(myhttp STATIC
    ${SOURCE_FILES}
)

target_include_directories(myhttp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 找 OpenSSL
find_package(OpenSSL REQUIRED)

# 框架库依赖
target_link_libraries(myhttp
    PUBLIC   # 注意是 PUBLIC，这样依赖会传递给使用者
        mysqlcppconn
        pthread
        OpenSSL::SSL
        OpenSSL::Crypto
)

# add_subdirectory(example)  # example程序
