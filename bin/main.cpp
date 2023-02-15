#include "lib/config_reader.h"
#include "lib/tape.h"
#include "lib/tape_sorter.h"

#include <iostream>

using namespace std::chrono_literals;

int main(int argc, char *argv[]) {
    std::filesystem::path path = argv[1];
    tape_sorter::ConfigReader config(path);
    config.ReadConfig();
    size_t size = config["N"].AsInt();
    size_t memory = config["M"].AsInt();
    std::chrono::milliseconds delay_for_read = config["delay_for_read"].AsMilliseconds();
    std::chrono::milliseconds delay_for_put = config["delay_for_put"].AsMilliseconds();
    std::chrono::milliseconds delay_for_shift = config["delay_for_shift"].AsMilliseconds();
    std::filesystem::path path_in = config["path_in"].AsPath();
    std::filesystem::path path_out = config["path_out"].AsPath();

    tape_sorter::Tape tape_in(
            path_in,
            size,
            std::min(memory / 16, size),
            delay_for_read,
            delay_for_put,
            delay_for_shift
    );
    tape_sorter::Tape tape_out(path_out);
    tape_sorter::TapeSorter sorter(tape_in, tape_out);
    sorter.Sort();

    return 0;
}
