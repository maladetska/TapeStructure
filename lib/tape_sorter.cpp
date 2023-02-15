#include "tape_sorter.h"

namespace tape_sorter {

    TapeSorter::TapeSorter(Tape &ti, Tape &to) : tape_in_(ti), tape_out_(to) {
        file_in_.open(tape_in_.GetPath());
        file_out_.open(tape_out_.GetPath());
    }

    void TapeSorter::MakeSplitFile(std::filesystem::path &path, Tape &tape, size_t number, size_t size) {
        std::filesystem::path tmp_file = path;
        tmp_file += std::to_string(number) + ".txt";
        std::fstream fout(tmp_file, std::fstream::out);
        std::vector<int32_t> buffer;
        for (size_t j = 0; j < size; j++) {
            buffer.push_back(tape_in_.GetCurr());
            tape_in_.MoveLeft();
        }
        std::sort(buffer.begin(), buffer.end());
        for (int32_t j: buffer) {
            fout << j << ' ';
        }
        fout.close();
        Tape result_tape(tmp_file, buffer.size(), buffer.size());
        tape = std::move(result_tape);
    }

    void TapeSorter::Split(std::filesystem::path &p, std::vector<Tape> &vt) {
        p += "/" + std::to_string(0) + "/";
        std::filesystem::create_directories(p);
        size_t file_count = tape_in_.GetCountOfChunks();
        for (size_t i = 0; i < file_count - 1; i++) {
            MakeSplitFile(p, vt[i], i, tape_in_.GetMaxSizeChunk());
        }
        MakeSplitFile(p, vt[file_count - 1], file_count - 1, tape_in_.GetMinSizeChunk());
    }

    std::pair<bool, bool> TapeSorter::MergeOneChunk(Tape &tape_result, Tape &tape1, Tape &tape2, bool end1, bool end2, size_t size) {//in
        std::vector<int32_t> buffer;
        if (end1 && !end2) {
            buffer.push_back(tape2.GetCurr());
            while (tape2.MoveLeft() && buffer.size() != size) {
                buffer.push_back(tape2.GetCurr());
            }
        } else if (end2 && !end1) {
            buffer.push_back(tape1.GetCurr());
            while (tape1.MoveLeft() && buffer.size() != size) {
                buffer.push_back(tape1.GetCurr());
            }
        } else {
            while (buffer.size() != size) {
                while (buffer.size() != size) {
                    if (tape1.GetCurr() < tape2.GetCurr()) {
                        buffer.push_back(tape1.GetCurr());
                        if (!tape1.MoveLeft()) {
                            end1 = true;
                            break;
                        }
                    } else if (tape1.GetCurr() >= tape2.GetCurr()) {
                        buffer.push_back(tape2.GetCurr());
                        if (!tape2.MoveLeft()) {
                            end2 = true;
                            break;
                        }
                    }
                }
                if (buffer.size() != size) {
                    if (!end1 && end2) {
                        buffer.push_back(tape1.GetCurr());
                        while (tape1.MoveLeft() && buffer.size() != size) {
                            buffer.push_back(tape1.GetCurr());
                        }
                    } else if (end1 && !end2) {
                        buffer.push_back(tape2.GetCurr());
                        while (tape2.MoveLeft() && buffer.size() != size) {
                            buffer.push_back(tape2.GetCurr());
                        }
                    }
                }
            }
        }
        for (int32_t &j: buffer) {
            tape_result.Put(j);
            tape_result.MoveLeft();
        }

        return {end1, end2};
    }

    Tape TapeSorter::Merge(std::fstream &fout, std::filesystem::path path, Tape &tape1, Tape &tape2) {
        std::pair<bool, bool> check_ends = {false, false};
        Tape result_tape(path, tape1.GetSize() + tape2.GetSize(), tape1.GetMaxSizeChunk());
        for (size_t i = 0; i < result_tape.GetCountOfChunks() - 1; i++) {
            check_ends = MergeOneChunk(result_tape, tape1, tape2, check_ends.first, check_ends.second, result_tape.GetMaxSizeChunk());
        }
        MergeOneChunk(result_tape, tape1, tape2, check_ends.first, check_ends.second, result_tape.GetMinSizeChunk());
        fout.close();
        tape1.DestroyChunk();
        tape2.DestroyChunk();
        return result_tape;
    }

    void TapeSorter::Assembly(std::filesystem::path &path, size_t dir, std::vector<Tape> &vt) {
        std::filesystem::path curr_path(dir_for_tmp_tapes_);
        curr_path += "/" + std::to_string(dir) + "/";
        std::filesystem::create_directories(curr_path);
        size_t tv_size = vt.size();
        std::vector<Tape> new_tapes_vector(
                tv_size % 2 == 0
                        ? tv_size / 2
                        : tv_size / 2 + 1);
        size_t i = 0;
        for (size_t j = 0; i < tv_size / 2; i++, j += 2) {
            std::filesystem::path tmp_file = curr_path;
            tmp_file += std::to_string(i) + ".txt";
            std::fstream fout(tmp_file, std::fstream::out);
            new_tapes_vector[i] = std::move(Merge(fout, tmp_file, vt[j], vt[j + 1]));
        }
        if (tv_size % 2 != 0) {
            std::filesystem::path tmp_file = curr_path;
            tmp_file += std::to_string(i) + ".txt";
            std::fstream fout(tmp_file, std::fstream::out);
            Tape curr_tape(tmp_file, vt[tv_size - 1]);
            new_tapes_vector[new_tapes_vector.size() - 1] = curr_tape;
            vt[tv_size - 1].DestroyChunk();
        }
        vt.clear();
        vt = new_tapes_vector;
    }

    void TapeSorter::Sort() {
        std::filesystem::create_directories(dir_for_tmp_tapes_);
        std::filesystem::path tmp_path(dir_for_tmp_tapes_);
        size_t file_count = tape_in_.GetCountOfChunks();
        std::vector<Tape> tapes_vector(file_count);

        Split(tmp_path, tapes_vector);

        if (file_count == 1) {
            tape_out_ = std::move(tapes_vector[0]);
        } else {
            for (size_t i = file_count, j = 1; i != 2; i = (i - 1) / 2 + 1, j++) {
                Assembly(tmp_path, j, tapes_vector);
                std::filesystem::path prev(dir_for_tmp_tapes_);
                prev += "/" + std::to_string(j - 1) + "/";
                std::filesystem::remove_all(prev);
            }

            std::fstream tape_out_file(tape_out_.GetPath(), std::fstream::out);
            tape_out_ = std::move(Merge(tape_out_file, tape_out_.GetPath(), tapes_vector[0], tapes_vector[1]));
            tape_out_file.close();
        }
        std::filesystem::remove_all(dir_for_tmp_tapes_);
    }

}  // namespace tape_sorter