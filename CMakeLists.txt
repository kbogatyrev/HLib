cmake_minimum_required(VERSION 3.21)

project(HLib LANGUAGES CXX)

add_library(HLib INTERFACE)

target_include_directories(HLib
		INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(HLib
		INTERFACE
		cxx_std_17)

option(HLIB_BUILD_TESTS "Build HLib tests" ON)

if(HLIB_BUILD_TESTS)
	add_subdirectory(tests)
endif()

