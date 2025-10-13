cmake_minimum_required(VERSION 3.0)
project(myhttp)

set(CMAKE_CXX_STANDARD 20)              
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # Enforce C++20 standard

# library output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib) 
# binary output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Enable all warnings and treat them as errors
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3") 

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include/http)
include_directories(${CMAKE_SOURCE_DIR}/include/router)
include_directories(${CMAKE_SOURCE_DIR}/include/session)
include_directories(${CMAKE_SOURCE_DIR}/include/utils)
include_directories(${CMAKE_SOURCE_DIR}/include/utils/db)
include_directories(${CMAKE_SOURCE_DIR}/include/ssl)
include_directories(${CMAKE_SOURCE_DIR}/include/middleware)
include_directories(${CMAKE_SOURCE_DIR}/include/middleware/cors)

# subdirectories
add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/example)