cmake_minimum_required(VERSION 3.12)
include(CheckCXXCompilerFlag)
include(ExternalProject)

project(KaDraw CXX)
set(CMAKE_CXX_STANDARD 11)

# if no build mode is specified build in release mode
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

# tweak compiler flags
CHECK_CXX_COMPILER_FLAG(-funroll-loops COMPILER_SUPPORTS_FUNROLL_LOOPS)
if (COMPILER_SUPPORTS_FUNROLL_LOOPS)
    add_definitions(-funroll-loops)
endif ()
CHECK_CXX_COMPILER_FLAG(-fno-stack-limit COMPILER_SUPPORTS_FNOSTACKLIMITS)
if (COMPILER_SUPPORTS_FNOSTACKLIMITS)
    add_definitions(-fno-stack-limit)
endif ()
CHECK_CXX_COMPILER_FLAG(-Wall COMPILER_SUPPORTS_WALL)
if (COMPILER_SUPPORTS_WALL)
    add_definitions(-Wall)
endif ()
CHECK_CXX_COMPILER_FLAG(-march=native COMPILER_SUPPORTS_MARCH_NATIVE)
if (COMPILER_SUPPORTS_MARCH_NATIVE)
    add_definitions(-march=native)
endif ()
CHECK_CXX_COMPILER_FLAG(-fpermissive COMPILER_SUPPORTS_FPERMISSIVE)
if (COMPILER_SUPPORTS_FPERMISSIVE)
    add_definitions(-fpermissive)
endif ()
CHECK_CXX_COMPILER_FLAG(-Wno-unused-result COMPILER_SUPPORTS_UNUSED)
if (COMPILER_SUPPORTS_UNUSED)
    add_definitions(-Wno-unused-result)
endif ()
CHECK_CXX_COMPILER_FLAG(-Wno-sign-compare COMPILER_SUPPORTS_NOSIGNCOMP)
if (COMPILER_SUPPORTS_NOSIGNCOMP)
    add_definitions(-Wno-sign-compare)
endif ()
CHECK_CXX_COMPILER_FLAG(-ffast-math COMPILER_SUPPORTS_FFAST_MATH)
if (COMPILER_SUPPORTS_FFAST_MATH)
    add_definitions(-ffast-math)
endif ()
CHECK_CXX_COMPILER_FLAG(-fopenmp COMPILER_SUPPORTS_FOPENMP)
if (COMPILER_SUPPORTS_FOPENMP)
    add_definitions(-fopenmp)
endif ()

# optimized output
option(OPTIMIZED_OUTPUT "optimized output" OFF)
if (OPTIMIZED_OUTPUT)
    add_definitions("-DKAFFPAOUTPUT")
endif ()

# libargtable2
ExternalProject_Add(argtable2
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/argtable2-13
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ./configure)
find_library(LIBARGTABLE argtable2 ${CMAKE_CURRENT_SOURCE_DIR}/extern/argtable2-13/src/.libs/)

# cairo
find_library(LIBCAIRO cairo REQUIRED)
include_directories(${LIBCAIRO_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/tools)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/drawing)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/io)

set(LIBDRAWIT_FILES
        lib/data_structure/graph_hierarchy.cpp
        lib/algorithms/shortest_paths.cpp
        lib/io/graph_io.cpp
        lib/tools/random_functions.cpp
        lib/tools/graph_extractor.cpp
        lib/tools/quality_metrics.cpp
        lib/drawing/coarsening/coarsening.cpp
        lib/drawing/coarsening/contraction.cpp
        lib/drawing/coarsening/matching/matching.cpp
        lib/drawing/coarsening/clustering/node_ordering.cpp
        lib/drawing/coarsening/clustering/size_constraint_label_propagation.cpp
        lib/drawing/uncoarsening/uncoarsening.cpp
        lib/drawing/graph_drawer.cpp
        lib/drawing/uncoarsening/complete_boundary.cpp
        lib/drawing/uncoarsening/local_optimizer.cpp
        lib/drawing/uncoarsening/partial_boundary.cpp
        lib/burn_drawing/burn_drawing.cpp)
add_library(libdrawit OBJECT ${LIBDRAWIT_FILES})

add_executable(kadraw app/kadraw.cpp)
add_dependencies(kadraw argtable2)
target_link_libraries(kadraw libdrawit ${LIBARGTABLE} ${LIBCAIRO} gomp)
