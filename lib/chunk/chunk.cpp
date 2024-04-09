#include "chunk.hpp"

namespace tape_structure {
    Chunk::Chunk(Delays delays,
                 ChunksCount chunk_number,
                 ChunkSize size) : delays_(delays),
                                   chunk_number_(chunk_number),
                                   size_(size),
                                   pos_(0) {}

    ChunkSize Chunk::GetPos() const {
        return pos_;
    }

    ChunksCount Chunk::GetChunkNumber() const {
        return chunk_number_;
    }

    NumberType Chunk::GetCurrentNumber() const {
        std::this_thread::sleep_for(delays_.delay_for_read_);
        return numbers_[pos_];
    }

    bool Chunk::IsLeftEdge() const {
        return pos_ == 0;
    }

    bool Chunk::IsRightEdge() const {
        return pos_ == size_ - 1;
    }

    bool Chunk::IsPossibleTakeLeftNumber() const {
        return !(chunk_number_ == 0 && pos_ == 0);
    }

    bool Chunk::IsPossibleTakeRightNumber(ChunksCount count_of_chunks) const {
        return !(chunk_number_ == count_of_chunks - 1 && pos_ == size_ - 1);
    }

    bool Chunk::IsMatchWith(ChunkSize another_pos, ChunksCount another_chunk_number) const {
        return pos_ == another_pos && another_chunk_number == chunk_number_;
    }

    bool Chunk::MoveLeftPos() {
        if (IsRightEdge()) {
            return false;
        }
        std::this_thread::sleep_for(delays_.delay_for_shift_);
        pos_++;

        return true;
    }

    bool Chunk::MoveRightPos() {
        if (!IsPossibleTakeLeftNumber() || IsLeftEdge()) {
            return false;
        }
        std::this_thread::sleep_for(delays_.delay_for_shift_);
        pos_--;

        return true;
    }

    void Chunk::ReadNewChunk(std::fstream &from, ChunksCount new_chunk_number, ChunkSize new_size) {
        size_ = new_size;
        pos_ = new_chunk_number >= chunk_number_ ? size_ - 1 : 0;
        chunk_number_ = new_chunk_number;
        numbers_.clear();
        numbers_.resize(size_);
        for (NumberType &num: numbers_) {
            std::this_thread::sleep_for(delays_.delay_for_shift_);
            std::this_thread::sleep_for(delays_.delay_for_read_);
            from >> num;
        }
    }

    void Chunk::PutNumberInArrayByPos(const NumberType &number, const ChunkSize pos) {
        std::this_thread::sleep_for(delays_.delay_for_put_);
        numbers_[pos] = number;
    }

    void Chunk::PrintChunk(std::fstream &to) {
        for (NumberType num: numbers_) {
            to << num << ' ';
        }
    }

    void Chunk::Destroy() {
        chunk_number_ = 0;
        size_ = 0;
        pos_ = 0;
        numbers_.clear();
    }

    std::vector<NumberType> Chunk::GetChunkNumbers() const {
        return numbers_;
    }

    void Chunk::MoveToLeftEdge() {
        while (!IsLeftEdge()) {
            MoveRightPos();
        }
    }

    void Chunk::MoveToRightEdge() {
        while (!IsRightEdge()) {
            MoveLeftPos();
        }
    }
} // namespace tape_structure
