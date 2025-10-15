file(GLOB_RECURSE SOURCE_FILES *.cc)

add_library(myhttp STATIC
    ${SOURCE_FILES}
)

target_include_directories(myhttp
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/http
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