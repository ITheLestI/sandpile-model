cmake_minimum_required(VERSION 3.14)

project(
    labwork3
    VERSION 1.0
    DESCRIPTION "C++ Year 2023 Labwork3"
    LANGUAGES CXX
)


set(CMAKE_CXX_STANDARD 17)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")

add_subdirectory(lib)
add_subdirectory(bin)
