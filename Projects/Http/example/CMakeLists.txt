set (DEMO_SRC
    main.cc
)

find_package(OpenSSL REQUIRED)

add_executable(demo_app ${DEMO_SRC})
target_link_libraries(demo_app myhttp OpenSSL::SSL OpenSSL::Crypto muduo_net muduo_base pthread mysqlcppconn)