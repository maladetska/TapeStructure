add_library(TapeStructureLib
        tape.cpp tape.hpp
        chunk/chunk.cpp chunk/chunk.hpp
        sorter/tape_sorter.cpp sorter/tape_sorter.hpp
        )

add_subdirectory(config_reader)