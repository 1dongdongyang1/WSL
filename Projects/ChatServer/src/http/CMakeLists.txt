file(GLOB SRC_FILES 
    *.cc
)

add_library(http STATIC
    ${SRC_FILES}
)

target_include_directories(http
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/http
)

# 找 OpenSSL
find_package(OpenSSL REQUIRED)

# 框架库依赖
target_link_libraries(http
    PUBLIC   # 注意是 PUBLIC，这样依赖会传递给使用者
        mysqlcppconn
        pthread
        OpenSSL::SSL
        OpenSSL::Crypto
        muduo_net
        muduo_base
)