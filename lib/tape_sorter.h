#pragma once

#include "tape.h"

#include <algorithm>

namespace tape_sorter {

    class TapeSorter {
    public:
        TapeSorter() = default;
        explicit TapeSorter(Tape &ti, Tape &to);

        void Sort();

        ~TapeSorter() {
            stream_to_tmp_.close();
            file_in_.close();
            file_out_.close();
        }

    private:
        Tape tape_in_;
        Tape tape_out_;
        std::filesystem::path dir_for_tmp_tapes_ = "./tmp";
        std::fstream stream_to_tmp_;
        std::fstream file_in_;
        std::fstream file_out_;

        void MakeSplitFile(std::filesystem::path &p, Tape &t, size_t n, size_t s);
        void Split(std::filesystem::path &p, std::vector<Tape> &vt);
        void Assembly(std::filesystem::path &path, size_t dir, std::vector<Tape> &vt);
        static std::pair<bool, bool> MergeOneChunk(Tape &tape_result, Tape &t1, Tape &t2, bool e1, bool e2, size_t s);
        static Tape Merge(std::fstream &f, std::filesystem::path p, Tape &t1, Tape &t2);
    };

}  // namespace tape_sorter