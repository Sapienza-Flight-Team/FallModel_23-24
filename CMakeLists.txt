cmake_minimum_required(VERSION 3.16)

project(sft_fall_model CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "-g -O0 -Wall -Wextra -Wpedantic -std=c++20")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "-O3 -w -std=c++20")
else()
    set(CMAKE_CXX_FLAGS "-O3 -std=c++20")
endif()
find_package(PythonLibs REQUIRED)
set(SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set(INC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(SFT_SOURCES

    ${SRC_DIR}/Waypoint.cpp
    ${SRC_DIR}/bridge.cpp
)

message(${PYTHON_INCLUDE_DIRS})

include_directories(
    ${INC_DIR}
    ${PYTHON_INCLUDE_DIRS}
)

link_libraries(
    ${PYTHON_LIBRARIES}
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_library(${PROJECT_NAME} SHARED
    ${SFT_SOURCES}
)