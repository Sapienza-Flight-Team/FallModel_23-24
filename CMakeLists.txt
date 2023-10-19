cmake_minimum_required(VERSION 3.16)

project(sft_fall_model CXX)
set(CMAKE_CXX_STANDARD 17)

set(INC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(SFT_SOURCES
    ${SRC_DIR}/Simulation.cpp
    ${SRC_DIR}/main.cpp
)

set(SFT_HEADERS
    ${INC_DIR}/Model.h
    ${INC_DIR}/PayChute.h
    ${INC_DIR}/pch.h
    ${INC_DIR}/Real3.h
    ${INC_DIR}/Settings.h
    ${INC_DIR}/Simulation.h
    ${INC_DIR}/State.h
    ${INC_DIR}/Wind.h
)

add_executable(
    ${PROJECT_NAME}
    ${SFT_SOURCES}
    ${SFT_HEADERS}
)

set_property(
    TARGET ${PROJECT_NAME}
    APPEND
    PROPERTY ADDITIONAL_CLEAN_FILES ${CMAKE_CURRENT_SOURCE_DIR}/res.csv
)

target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${INC_DIR}
)
