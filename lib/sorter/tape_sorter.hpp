#pragma once

#include <algorithm>

#include "../tape.hpp"

namespace tape_structure {
    class TapeSorter {
    public:
        TapeSorter() = default;
        TapeSorter(Tape &tape_in, Tape &tape_out);

        void Sort();

        ~TapeSorter();

    private:
        static Tape Merge(std::fstream &stream_to, std::filesystem::path path, Tape &tape1, Tape &tape2);
        static std::pair<bool, bool> MergeOneChunk(Tape &tape_result,
                                                   Tape &tape1, Tape &tape2,
                                                   bool end1, bool end2,
                                                   ChunkSize size);

        void Split(std::filesystem::path &path, std::vector<Tape> &tapes);
        void MakeSplitFile(std::filesystem::path &path, Tape &tape, size_t file_number);

        void Assembly(size_t dir, std::vector<Tape> &tapes);

        static void PutTapeRestToBuffer(Tape &tape, std::vector<NumberType> &buffer, ChunkSize size);
        bool static PutNumberInBuffer(Tape &tape, std::vector<NumberType> &buffer, bool &end);

        Tape tape_in_;
        Tape tape_out_;

        std::fstream file_stream_in_;
        std::fstream file_stream_out_;

        const std::filesystem::path dir_for_tmp_tapes_ = "./tmp";
    };

} // namespace tape_structure