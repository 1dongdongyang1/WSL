aux_source_directory(. SRC_LIST)

add_library(myhttp ${SRC_LIST})
find_package(OpenSSL REQUIRED)

# Link with OpenSSL, MySQL Connector/C++, and pthread
target_link_libraries(myhttp PUBLIC mysqlcppconn pthread OpenSSL::SSL OpenSSL::Crypto)    