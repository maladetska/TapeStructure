#include <iostream>

#include "lib/config_reader/simple_yaml_reader.hpp"
#include "lib/sorter/tape_sorter.hpp"

using namespace std::chrono_literals;

int main(int argc, char *argv[]) {
    std::filesystem::path path = argv[1];

    config_reader::SimpleYamlReader config(path);
    config.ReadConfig();

    size_t size = config["N"].AsInt32();
    size_t memory = config["M"].AsInt32();

    std::chrono::milliseconds delay_for_read = config["delay_for_read"].AsMilliseconds();
    std::chrono::milliseconds delay_for_put = config["delay_for_put"].AsMilliseconds();
    std::chrono::milliseconds delay_for_shift = config["delay_for_shift"].AsMilliseconds();

    std::filesystem::path path_in = config["path_in"].AsPath();
    std::filesystem::path path_out = config["path_out"].AsPath();

    tape_structure::Tape tape_in(
            path_in,
            size,
            tape_structure::Tape::CountChunkSize(memory, size),
            delay_for_read,
            delay_for_put,
            delay_for_shift);
    tape_structure::Tape tape_out(path_out);
    tape_structure::TapeSorter sorter(tape_in, tape_out);

    sorter.Sort();

    return 0;
}