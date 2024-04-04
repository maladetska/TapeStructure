#pragma once

#include <cstdint>
#include <fstream>
#include <vector>

namespace tape_structure {
    using NumberType = int32_t;
    using ChunkSize = uint32_t;
    using ChunksCount = uint32_t;

    class Chunk {
    public:
        Chunk() = default;
        Chunk(ChunksCount chunk_number, ChunkSize size);

        Chunk(const Chunk &) = default;
        Chunk &operator=(const Chunk &) = default;

        Chunk(Chunk &&) noexcept = default;
        Chunk &operator=(Chunk &&) noexcept = default;

        [[nodiscard]] ChunkSize GetPos() const;
        [[nodiscard]] ChunksCount GetChunkNumber() const;
        [[nodiscard]] NumberType GetCurrentNumber() const;
        [[nodiscard]] std::vector<NumberType> GetChunkNumbers() const;

        [[nodiscard]] bool IsPossibleTakeLeftNumber() const;
        [[nodiscard]] bool IsPossibleTakeRightNumber(ChunksCount count_of_chunks) const;
        [[nodiscard]] bool IsToTheLeftThan(ChunkSize pos_to, ChunksCount chunk_number_to) const;

        bool MoveRightPos();
        bool MoveLeftPos();

        void MoveToLeftEdge();
        void MoveToRightEdge();

        void ReadNewChunk(std::fstream &from, ChunksCount new_chunk_number, ChunkSize new_size);
        void PutNumberInArrayByPos(const NumberType &number, ChunkSize pos);
        void PrintChunk(std::fstream &to);

        void Destroy();

        friend class Tape;

    private:
        [[nodiscard]] bool IsLeftEdge() const;
        [[nodiscard]] bool IsRightEdge() const;

        ChunksCount chunk_number_{};
        ChunkSize size_{};
        ChunkSize pos_{};

        std::vector<NumberType> numbers_;
    };

} // namespace tape_structure
