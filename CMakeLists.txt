cmake_minimum_required(VERSION 3.0)

set(projname contour)

set(CXX_STANDARD C++20)

set(include_dir D:/Extra/CavalierContoursDev/CavalierContours/include 
	D:/projects/cmakeqt/thirdparty/fmt-8.1.1/include
	D:/projects/cmakeqt/thirdparty/glm/
	include)

project(${projname})

add_executable(${projname} contour_ex.cpp glm-aabb/aabb.cpp)

target_include_directories(${projname} PRIVATE ${include_dir})
