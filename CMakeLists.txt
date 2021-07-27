cmake_minimum_required(VERSION 3.14)

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif ()

project(separator)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-flto)
        add_link_options(-flto)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-flto=thin)
        add_link_options(-fuse-ld=lld)
    endif ()
endif ()

#if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
#    add_compile_options(--coverage)
#    add_link_options(--coverage)
#endif ()

add_subdirectory(lib/random)
add_subdirectory(lib/subprocess)
list(APPEND CMAKE_MODULE_PATH "lib/findtbb/FindTBB.cmake")

find_package(TBB)
find_package(Threads)

add_executable(separator main.cpp model/Node.hpp model/Edge.hpp model/Node.cpp model/model.hpp model/Edge.cpp model/model.cpp model/Graph.hpp model/Graph.cpp gen/LinearGenerator.hpp gen/LinearGenerator.cpp alg/LinearSeparator.hpp alg/LinearSeparator.cpp alg/Oracle.hpp alg/Oracle.cpp util/stlutils.hpp model/modeltypes.hpp util/printutils.hpp util/parallelutils.hpp gen/TreeGenerator.hpp gen/TreeGenerator.cpp util/randutils.hpp util/funcutils.hpp alg/CentroidDecomposition.hpp alg/CentroidDecomposition.cpp model/Tree.hpp model/Tree.cpp util/progutils.hpp alg/TreeSeparator.hpp alg/TreeSeparator.cpp alg/Separator.hpp alg/Separator.cpp)

target_link_libraries(separator effolkronium_random tbb Threads::Threads subprocess)

target_precompile_headers(separator PRIVATE <string> <vector> <map> <functional> <concepts> <iostream> <execution>)

set_target_properties(separator PROPERTIES UNITY_BUILD ON)

# colorful ninja

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif ()
endif ()
