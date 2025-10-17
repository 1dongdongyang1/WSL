file(GLOB SRC_FILES 
    *.cc
)

add_library(handlers STATIC
    ${SRC_FILES}
)

target_include_directories(handlers
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)


target_link_libraries(handlers
    PUBLIC
        http
        utils
)