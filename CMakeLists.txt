cmake_minimum_required(VERSION 3.16)

project(sft_fall_model CXX)
set(CMAKE_CXX_STANDARD 20)

set(INC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
set(SFT_SOURCES
    ${SRC_DIR}/BallisticModel.cpp
    ${SRC_DIR}/Simulation.cpp
    ${SRC_DIR}/Waypoint.cpp
    ${SRC_DIR}/main.cpp
)

set(SFT_HEADERS
    ${INC_DIR}/Model.h
    ${INC_DIR}/BallisticModel.h
    ${INC_DIR}/PayChute.h
    ${INC_DIR}/Real3.h
    ${INC_DIR}/Settings.h
    ${INC_DIR}/Simulation.h
    ${INC_DIR}/State.h
    ${INC_DIR}/Wind.h
    ${INC_DIR}/Waypoint.h
    ${INC_DIR}/BaseStepper.h
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
    -O0
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${INC_DIR}
)
