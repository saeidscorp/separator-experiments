cmake_minimum_required(VERSION 3.14)
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

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif ()

add_subdirectory(lib/random)
list(APPEND CMAKE_MODULE_PATH "lib/findtbb/FindTBB.cmake")

find_package(TBB)
find_package(Threads)

add_executable(separator headers.hpp main.cpp model/Node.hpp model/Edge.hpp model/Node.cpp model/model.hpp model/Edge.cpp model/model.cpp model/Graph.hpp gen/LinearGenerator.hpp alg/LinearSeparator.hpp alg/Oracle.hpp util/stlutils.hpp model/modeltypes.hpp util/printutils.hpp util/parallelutils.hpp gen/TreeGenerator.hpp gen/TreeGenerator.cpp util/randutils.hpp util/funcutils.hpp alg/CentroidDecomposition.hpp alg/CentroidDecomposition.cpp model/Tree.hpp model/Tree.cpp util/progutils.hpp alg/TreeSeparator.hpp alg/Separator.hpp)
target_precompile_headers(separator PRIVATE headers.hpp)
target_link_libraries(separator effolkronium_random Threads::Threads tbb)
target_include_directories(separator PRIVATE lib/cpp-subprocess/include)


# colorful ninja

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif ()
endif ()
