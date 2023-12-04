cmake_minimum_required(VERSION 3.16)

project(sft_fall_model CXX)
set(CMAKE_CXX_STANDARD 20)


if(CMAKE_GENERATOR MATCHES "Visual Studio 17 2022")
    set(CMAKE_TOOLCHAIN_FILE "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()
# Set the toolchain file to the vcpkg toolchain file

# Find the Boost package
find_package(Boost REQUIRED)


set(INC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
set(SFT_SOURCES
   
    ${SRC_DIR}/Waypoint.cpp
    ${SRC_DIR}/main.cpp
)


set(SFT_HEADERS
    ${INC_DIR}/Model.h
    ${INC_DIR}/BallisticModel.h
    ${INC_DIR}/PayChute.h
    ${INC_DIR}/VReal.h
    ${INC_DIR}/Settings.h
    ${INC_DIR}/Simulation.h
    ${INC_DIR}/State.h
    ${INC_DIR}/Wind.h
    ${INC_DIR}/Waypoint.h
    ${INC_DIR}/BaseStepper.h
)


set(CMAKE_CXX_FLAGS_PROFILING "-O2 -g -pg" CACHE STRING
    "Flags used by the C++ compiler during Profiling builds."
    FORCE )
set(CMAKE_C_FLAGS_PROFILING "-O2 -g -pg" CACHE STRING
    "Flags used by the C compiler during Profiling builds."
    FORCE )
set(CMAKE_EXE_LINKER_FLAGS_PROFILING
    "-pg" CACHE STRING
    "Flags used for linking binaries during Profiling builds."
    FORCE )
set(CMAKE_SHARED_LINKER_FLAGS_PROFILING
    "-pg" CACHE STRING
    "Flags used by the shared libraries linker during Profiling builds."
    FORCE )
mark_as_advanced(
    CMAKE_CXX_FLAGS_PROFILING
    CMAKE_C_FLAGS_PROFILING
    CMAKE_EXE_LINKER_FLAGS_PROFILING
    CMAKE_SHARED_LINKER_FLAGS_PROFILING
)

# Update the documentation string of CMAKE_BUILD_TYPE for GUIs
set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Profiling."
    FORCE )
    

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

# embed STL in executable
target_link_options(
    ${PROJECT_NAME}
    PRIVATE
    -static-libgcc
    -static-libstdc++
)