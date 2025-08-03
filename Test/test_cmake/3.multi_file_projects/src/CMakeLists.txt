add_library(greet STATIC greet.cc)
target_include_directories(greet PUBLIC ${PROJECT_SOURCE_DIR}/include)
add_executable(app main.cc)
target_link_libraries(app PRIVATE greet)
