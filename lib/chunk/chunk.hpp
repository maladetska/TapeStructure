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

        /**
         * Get the position in the chunk indicated by the magnetic head.
         *
         * @return position in the chunk.
         */
        [[nodiscard]] ChunkSize GetPos() const;

        /**
        * Get the chunk number (id).
        *
        * @return chunk number
        */
        [[nodiscard]] ChunksCount GetChunkNumber() const;
        /**
         * Get the number in the chunk that the magnetic head points to.
         *
         * @return number
         */
        [[nodiscard]] NumberType GetCurrentNumber() const;
        /**
         * Get all numbers in the chunk.
         *
         * @return vector of numbers.
         */
        [[nodiscard]] std::vector<NumberType> GetChunkNumbers() const;

        /**
         * Checking for the number to the left of the magnetic head.
         * It does not exist if this chunk is the leftmost and
         * the magnetic head points to the leftmost number in it.
         *
         * @return true if such a number exists else false.
         */
        [[nodiscard]] bool IsPossibleTakeLeftNumber() const;
        /**
         * Checking for the number to the right of the magnetic head.
         * It does not exist if this chunk is the rightmost and
         * the magnetic head points to the rightmost number in it.
         *
         * @param count_of_chunks total number of chunks on the tape.
         * @return true if such a number exists else false.
         */
        [[nodiscard]] bool IsPossibleTakeRightNumber(ChunksCount count_of_chunks) const;
        /**
         * The position and the number of the chunk are the same as the current ones.
         *
         * @param another_pos
         * @param another_chunk_number
         * @return true if the position and the chunk number are the same else false.
         */
        [[nodiscard]] bool IsMatchWith(ChunkSize another_pos, ChunksCount another_chunk_number) const;

        /**
         * Move the chunk to the right by one position.
         * (that is, the magnetic head will be on the number of the chunk on the left).
         *
         * @return true if the move succeeded else false.
         */
        bool MoveRightPos();
        /**
         * Move the chunk to the left by one position
         * (that is, the magnetic head will be on the number of the chunk on the right).
         *
         * @return true if the move succeeded else false.
         */
        bool MoveLeftPos();

        /**
         * Move the tape to the right
         * until the magnetic head points to the leftmost position of the chunk.
         */
        void MoveToLeftEdge();
        /**
         * Move the tape to the left
         * until the magnetic head points to the rightmost position of the chunk.
         */
        void MoveToRightEdge();

        /**
         * Read new chunk from a file.
         *
         * @param from file stream from where the new chunk will be read.
         * @param new_chunk_number number of the new chunk.
         * @param new_size size of the new chunk.
         */
        void ReadNewChunk(std::fstream &from, ChunksCount new_chunk_number, ChunkSize new_size);
        /**
         * Put a new number in the chunk array.
         *
         * @param number new number
         * @param pos position where the new number is placed
         */
        void PutNumberInArrayByPos(const NumberType &number, ChunkSize pos);
        /**
         * Output a chunk to a file.
         *
         * @param to file stream into which the chunk will be printed.
         */
        void PrintChunk(std::fstream &to);

        /**
         * Clear chunk.
         */
        void Destroy();

    private:
        /**
         * Checking that the current position is the leftmost in the chunk.
         *
         * @return true if the current position is the leftmost else false.
         */
        [[nodiscard]] bool IsLeftEdge() const;
        /**
         * Checking that the current position is the rightmost in the chunk.
         *
         * @return true if the current position is the rightmost else false.
         */
        [[nodiscard]] bool IsRightEdge() const;

        /**
         * Chunk number/position/id.
         */
        ChunksCount chunk_number_{};
        /**
         * Size of the chunk.
         */
        ChunkSize size_{};
        /**
         * Position in the chunk indicated by the magnetic head.
         */
        ChunkSize pos_{};
        /**
         * Array of chunk numbers.
         */
        std::vector<NumberType> numbers_;
    };
} // namespace tape_structure
