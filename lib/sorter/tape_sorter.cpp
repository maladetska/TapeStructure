#include "tape_sorter.hpp"

namespace tape_structure {
    TapeSorter::TapeSorter(Tape &tape_in, Tape &tape_out) : tape_in_(tape_in),
                                                            tape_out_(tape_out) {
        file_stream_in_.open(tape_in_.GetPath());
        file_stream_out_.open(tape_out_.GetPath());
    }

    TapeSorter::~TapeSorter() {
        file_stream_in_.close();
        file_stream_out_.close();
    }

    void TapeSorter::Sort() {
        std::filesystem::create_directories(dir_for_tmp_tapes_);
        std::filesystem::path tmp_path(dir_for_tmp_tapes_);
        TapeSize count_of_chunks = tape_in_.GetCountOfChunks();

        std::vector<Tape> tapes(count_of_chunks);

        Split(tmp_path, tapes);

        if (count_of_chunks == 1) {
            tape_out_ = std::move(tapes[0]);
        } else {
            for (size_t i = count_of_chunks, j = 1; i != 2; i = (i - 1) / 2 + 1, j++) {
                Assembly(j, tapes);
                std::filesystem::path prev(dir_for_tmp_tapes_);
                prev += "/" + std::to_string(j - 1) + "/";
                std::filesystem::remove_all(prev);
            }

            std::fstream tape_out_file(tape_out_.GetPath(), std::fstream::out);
            tape_out_ = std::move(Merge(tape_out_file, tape_out_.GetPath(), tapes[0], tapes[1]));
            tape_out_file.close();
        }
        std::filesystem::remove_all(dir_for_tmp_tapes_);
    }

    Tape TapeSorter::Merge(std::fstream &stream_to, std::filesystem::path path, Tape &tape1, Tape &tape2) {
        std::pair<bool, bool> check_ends = {false, false};
        Tape result_tape(path, tape1.GetSize() + tape2.GetSize(), tape1.GetMaxChunkSize());
        for (size_t i = 0; i < result_tape.GetCountOfChunks() - 1; i++) {
            check_ends = MergeOneChunk(
                    result_tape,
                    tape1,
                    tape2,
                    check_ends.first,
                    check_ends.second,
                    result_tape.GetMaxChunkSize());
        }
        MergeOneChunk(
                result_tape,
                tape1,
                tape2,
                check_ends.first,
                check_ends.second,
                result_tape.GetMinChunkSize());

        stream_to.close();
        tape1.ClearChunkInTape();
        tape2.ClearChunkInTape();

        return result_tape;
    }

    std::pair<bool, bool> TapeSorter::MergeOneChunk(Tape &tape_result,
                                                    Tape &tape1,
                                                    Tape &tape2,
                                                    bool end1,
                                                    bool end2,
                                                    ChunkSize size) {
        std::vector<NumberType> buffer;
        if (end1 && !end2) {
            PutTapeRestToBuffer(tape2, buffer, size);
        } else if (end2 && !end1) {
            PutTapeRestToBuffer(tape1, buffer, size);
        } else {
            while (buffer.size() != size) {
                while (buffer.size() != size) {
                    if (tape1.GetCurrentNumber() < tape2.GetCurrentNumber()) {
                        if (PutNumberInBuffer(tape1, buffer, end1)) {
                            break;
                        }
                    } else if (tape1.GetCurrentNumber() >= tape2.GetCurrentNumber()) {
                        if (PutNumberInBuffer(tape2, buffer, end2)) {
                            break;
                        }
                    }
                }
                if (buffer.size() != size) {
                    if (!end1 && end2) {
                        PutTapeRestToBuffer(tape1, buffer, size);
                    } else if (end1 && !end2) {
                        PutTapeRestToBuffer(tape2, buffer, size);
                    }
                }
            }
        }

        for (NumberType &number: buffer) {
            tape_result.Put(number);
            tape_result.MoveLeft();
        }

        return {end1, end2};
    }

    void TapeSorter::Split(std::filesystem::path &path, std::vector<Tape> &tapes) {
        path += "/" + std::to_string(0) + "/";
        std::filesystem::create_directories(path);
        size_t count_of_chunks = tape_in_.GetCountOfChunks();
        for (size_t i = 0; i < count_of_chunks; i++) {
            MakeSplitFile(path, tapes[i], i);
        }
    }

    void TapeSorter::MakeSplitFile(std::filesystem::path &path, Tape &tape, size_t file_number) {
        std::filesystem::path tmp_file = path;
        tmp_file += std::to_string(file_number) + ".txt";
        std::fstream stream_to(tmp_file, std::fstream::out);

        tape_in_.ReadNextChunk();

        std::vector<NumberType> buffer = tape_in_.GetChunkNumbers();
        std::sort(buffer.begin(), buffer.end());

        for (NumberType &number: buffer) {
            stream_to << number << ' ';
        }

        stream_to.close();
        Tape result_tape(tmp_file, buffer.size(), buffer.size());
        tape = std::move(result_tape);
    }

    void TapeSorter::Assembly(size_t dir, std::vector<Tape> &tapes) {
        std::filesystem::path curr_path(dir_for_tmp_tapes_);
        curr_path += "/" + std::to_string(dir) + "/";
        std::filesystem::create_directories(curr_path);

        size_t tapes_size = tapes.size();
        std::vector<Tape> new_tapes(tapes_size % 2 == 0
                                            ? tapes_size / 2
                                            : tapes_size / 2 + 1);
        size_t i = 0;
        for (size_t j = 0; i < tapes_size / 2; i++, j += 2) {
            std::filesystem::path tmp_file = curr_path;
            tmp_file += std::to_string(i) + ".txt";
            std::fstream stream_to(tmp_file, std::fstream::out);

            new_tapes[i] = std::move(Merge(stream_to, tmp_file, tapes[j], tapes[j + 1]));
        }
        if (tapes_size % 2 != 0) {
            std::filesystem::path tmp_file = curr_path;
            tmp_file += std::to_string(i) + ".txt";
            std::fstream stream_to(tmp_file, std::fstream::out);
            Tape curr_tape(tapes[tapes_size - 1], tmp_file);
            new_tapes[new_tapes.size() - 1] = curr_tape;
        }
        tapes.clear();
        tapes = new_tapes;
    }

    void TapeSorter::PutTapeRestToBuffer(Tape &tape, std::vector<NumberType> &buffer, ChunkSize size) {
        buffer.push_back(tape.GetCurrentNumber());
        while (tape.MoveLeft() && buffer.size() != size) {
            buffer.push_back(tape.GetCurrentNumber());
        }
    }

    bool TapeSorter::PutNumberInBuffer(Tape &tape, std::vector<NumberType> &buffer, bool &end) {
        buffer.push_back(tape.GetCurrentNumber());
        if (!tape.MoveLeft()) {
            end = true;
            return true;
        }
        return false;
    }
} // namespace tape_structure