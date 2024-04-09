#pragma once

#include <algorithm>

#include "../tape.hpp"

namespace tape_structure {
    class TapeSorter {
    public:
        TapeSorter() = default;
        TapeSorter(Tape &tape_in, Tape &tape_out);

        ~TapeSorter() = default;

        /**
         * Starting sorting.
         */
        void Sort();

    private:
        /**
         * Merge two sorted tapes into one sorted tape.
         *
         * @param path path to the file of new tape file to which the result is written
         * @param tape1 first sorted tape
         * @param tape2 second sorted tape
         * @return sorted tape consisting of two introductory tapes
         */
        static Tape Merge(std::filesystem::path path, Tape &tape1, Tape &tape2);
        /**
         * Create new sorted chunk from two tapes by merging.
         *
         * @param tape_result writing a new chunk to this tape
         * @param tape1 first sorted resource tape
         * @param tape2 second sorted resource tape
         * @param end1 true if the position on the tape1 is the rightmost and the tape1 is passed to the end
         * @param end2 true if the position on the tape2 is the rightmost and the tape2 is passed to the end
         * @param size size of new chunk
         * @return new value of end1 and end2 params
         */
        static std::pair<bool, bool> MergeOneChunk(Tape &tape_result,
                                                   Tape &tape1, Tape &tape2,
                                                   bool end1, bool end2,
                                                   ChunkSize size);

        /**
         * Starting splitting tapes into array of tapes.
         *
         * @param path file path where the tapes should be stored
         * @param tapes split tapes
         */
        void Split(std::filesystem::path &path, std::vector<Tape> &tapes);
        /**
         * Create a new split tape.
         *
         * @param path path to the file where new tape will be located.
         * @param tape new tape
         * @param tape_number number of new tape
         */
        void MakeSplitTape(std::filesystem::path &path, Tape &tape, TapeSize tape_number);

        /**
         * Starting of the assembly of split tapes together.
         *
         * @param dir
         * @param tapes split tapes
         */
        void Assembly(TapeSize dir, std::vector<Tape> &tapes);

        /**
         * Put the remaining numbers of the tape in the buffer.
         *
         * @param tape tape from which the current number is taken
         * @param buffer buffer of numbers
         * @param size size that the buffer should have
         */
        static void PutTapeRestToBuffer(Tape &tape, std::vector<NumberType> &buffer, ChunkSize size);
        /**
         * Put the current number from the tape (indicated by the magnetic head) in the buffer.
         *
         * @param tape tape from which the current number is taken
         * @param buffer buffer of numbers
         * @param end true if the magnetic head points to the rightmost position of the tape else false
         * @return true if the magnetic head points to the rightmost position of the tape else false
         */
        bool static PutNumberInBuffer(Tape &tape, std::vector<NumberType> &buffer, bool &end);

        /**
         * Tape that needs to be sorted.
         */
        Tape tape_in_;
        /**
         * Tape in which the final sorted result will be recorded.
         */
        Tape tape_out_;

        const std::filesystem::path dir_for_tmp_tapes_ = "./tmp";
    };

} // namespace tape_structure