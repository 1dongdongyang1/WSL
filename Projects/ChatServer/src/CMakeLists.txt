add_subdirectory(http)
add_subdirectory(utils)
add_subdirectory(handlers)

add_executable(chatserver 
    main.cc
    ChatServer.cc
)

set_target_properties(chatserver PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

target_include_directories(chatserver PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(chatserver PRIVATE
    http
    utils
    handlers
)