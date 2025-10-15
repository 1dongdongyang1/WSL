set(DEMO_SRC main.cc)

add_executable(demo ${DEMO_SRC})

# bin 目录下生成 demo 可执行文件
set_target_properties(demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin)

# 只需要 link 框架库，不用再 link OpenSSL
target_link_libraries(demo
    PRIVATE
        myhttp
        muduo_net
        muduo_base
        pthread
        mysqlcppconn
)

target_include_directories(demo
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)