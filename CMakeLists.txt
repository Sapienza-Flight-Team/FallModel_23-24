cmake_minimum_required(VERSION 3.16)

project(sft_fall_model CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PythonLibs REQUIRED)

if(NOT PYTHONLIBS_FOUND)
    message(FATAL_ERROR "Python libs not found")
else(NOT PYTHONLIBS_FOUND)
    message(STATUS "Python libs found at ${PYTHON_LIBRARIES}")
endif(NOT PYTHONLIBS_FOUND)

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

set(SFT_HEADERS
    ${INC_DIR}/BallisticModel.h
    ${INC_DIR}/BaseStepper.h
    ${INC_DIR}/Force.h
    ${INC_DIR}/PayChute.h
    ${INC_DIR}/pch.h
    ${INC_DIR}/Settings.h
    ${INC_DIR}/Simulation.h
    ${INC_DIR}/State.h
    ${INC_DIR}/VReal.h
    ${INC_DIR}/Waypoint.h
    ${INC_DIR}/Wind.h
)

add_library(${PROJECT_NAME} SHARED
    ${SFT_SOURCES} ${SFT_HEADERS}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${INC_DIR}
    ${PYTHON_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${PYTHON_LIBRARIES}
)

target_compile_features(
    ${PROJECT_NAME} PUBLIC
    cxx_std_20
)

target_compile_options(${PROJECT_NAME} PRIVATE # SEMPRE
    -Wall
    -Wextra
    -Wpedantic
) # -O0 -g e -O3/-O2 sono sempre settate in base al profilo di build

set(
    COMPILE_NATIVE FALSE
)

if(COMPILE_NATIVE)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -march=native -mtune=native
    )
endif(COMPILE_NATIVE)

# custom clang-tidy target
set(CLANG_TIDY_CHECKS # from a lib of mine, sensible defaults
    "misc-*,modernize-*,cppcoreguidelines-*,performance-*,bugprone-*,clang-analyzer-*,-bugprone-easily-swappable-parameters,-bugprone-unchecked-optional-access,-misc-no-recursion,-misc-non-private-*,-performance-no-int-to-ptr,-cppcoreguidelines-avoid-magic-numbers,-cppcoreguidelines-pro-bounds-constant-array-index,-cppcoreguidelines-non-private-*")

add_custom_target(
    clang-tidy-lib
    COMMAND clang-tidy -header-filter=${SFT_HEADERS} --fix --fix-errors -checks=${CLANG_TIDY_CHECKS} -p ${CMAKE_BINARY_DIR} ${SFT_SOURCES}
    -- -I${INC_DIR} -I${SRC_DIR} -I${PYTHON_INCLUDE_DIRS} -std=c++20 -fexperimental-library
    COMMENT "Running clang-tidy on teapot"
    VERBATIM
)