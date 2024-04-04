cmake_minimum_required(VERSION 3.24)
project(TapeSorter
        VERSION 2.0
        DESCRIPTION "Tape structure with sort"
        LANGUAGES CXX
        )


set(CMAKE_CXX_STANDARD 23)

add_subdirectory(lib)
add_subdirectory(bin)

enable_testing()
add_subdirectory(tests)