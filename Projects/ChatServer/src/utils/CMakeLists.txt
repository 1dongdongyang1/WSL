file(GLOB SRC_FILES 
    *.cc
)

add_library(utils STATIC
    ${SRC_FILES}
)

target_include_directories(untils
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(utils
    PUBLIC
        http
)