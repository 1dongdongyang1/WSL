cmake_minimum_required(VERSION 3.10)

project(multi_file_project)

include_directories(include)

add_executable(app 
	src/main.cc
	src/greet.cc
	)
