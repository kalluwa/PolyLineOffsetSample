cmake_minimum_required(VERSION 3.0)

set(projname contour)

set(CMAKE_CXX_STANDARD 20)

set(include_dir 
	D:/libs/glm
	include)

project(${projname})


add_executable(${projname} contour_ex.cpp glm-aabb/aabb.cpp)

target_include_directories(${projname} PRIVATE ${include_dir})
