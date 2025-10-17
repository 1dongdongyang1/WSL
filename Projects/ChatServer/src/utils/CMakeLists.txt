file(GLOB SRC_FILES 
    *.cc
)

add_library(utils STATIC
    ${SRC_FILES}
)

target_include_directories(utils
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

find_package(spdlog REQUIRED)
find_package(CURL REQUIRED)

target_link_libraries(utils
    PUBLIC
        http
        SimpleAmqpClient
        rabbitmq
        spdlog::spdlog
        CURL::libcurl
)