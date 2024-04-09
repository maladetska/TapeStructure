#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

#include "chunk/chunk.hpp"

namespace tape_structure {
    using TapeSize = ChunksCount;
    using MemorySize = uint32_t;

    /**
     * Tape can move to the right or to the left while the magnetic head is stationary.
     * By default, the numbering of the tape elements starts on the left.
     * Tape consists of chunks for the implementation of which there is a class Chunk.
     */
    class Tape {
    public:
        Tape() = default;

        explicit Tape(Delays delays);
        Tape(std::chrono::milliseconds delay_for_read,
             std::chrono::milliseconds delay_for_put,
             std::chrono::milliseconds delay_for_shift);
        Tape(std::filesystem::path &path,
             std::chrono::milliseconds delay_for_read,
             std::chrono::milliseconds delay_for_put,
             std::chrono::milliseconds delay_for_shift);
        Tape(std::filesystem::path &path, Delays delays);
        Tape(std::filesystem::path &path,
             TapeSize tape_size,
             ChunkSize chunk_size,
             std::chrono::milliseconds delay_for_read,
             std::chrono::milliseconds delay_for_put,
             std::chrono::milliseconds delay_for_shift);
        Tape(std::filesystem::path &path, TapeSize tape_size, ChunkSize chunk_size);

        Tape(const Tape &);
        Tape(const Tape &, std::filesystem::path &path);
        Tape &operator=(const Tape &);

        Tape(Tape &&) noexcept;
        Tape &operator=(Tape &&) noexcept;

        ~Tape();

        /**
         * Count one chunk size.
         *
         * @param memory RAM memory
         * @param size size of the tape
         * @return size of one chunk
         */
        static ChunkSize CountChunkSize(MemorySize memory, TapeSize size);

        /**
         * Get the path to the file where the tape is located.
         *
         * @return path to the file where the tape is located
         */
        [[nodiscard]] std::filesystem::path GetPath() const;
        /**
         * Get the size of tape.
         *
         * @return size of tape
         */
        [[nodiscard]] TapeSize GetSize() const;
        /**
         * Get the number of chunks.
         *
         * @return number of chunks
         */
        [[nodiscard]] TapeSize GetCountOfChunks() const;
        /**
         * Get the size of all chunks
         * except the size of the last chunk if it is smaller than the rest.
         *
         * @return max size of chunks
         */
        [[nodiscard]] ChunkSize GetMaxChunkSize() const;
        /**
         * Get the size of the last chunk.
         *
         * @return min size of chunks
         */
        [[nodiscard]] ChunkSize GetMinChunkSize() const;
        /**
         * Get the number of chunks in the tape.
         *
         * @return number of chunks in the tape
         */
        [[nodiscard]] std::vector<NumberType> GetChunkNumbers() const;
        /**
         * Get the number indicated by the magnetic head.
         *
         * @return number indicated by the magnetic head
         */
        [[nodiscard]] NumberType GetCurrentNumber();

        /**
         * Move the tape under the magnetic head to the right.
         * After execution, the tape element will be one position to the left under the magnetic head.
         *
         * @return true if the move succeeded else false
         */
        bool MoveRight();
        /**
         * Move the tape under the magnetic head to the left.
         * After execution, the tape element will be one position to the right under the magnetic head.
         *
         * @return true if the move succeeded else false
         */
        bool MoveLeft();

        /**
         * Put a new element in the tape.
         *
         * @param number new element.
         */
        void Put(const NumberType &number);

        /**
         * Clear current chunk.
         */
        void ClearChunkInTape();

        friend class TapeSorter;

    private:
        /**
         * Information about chunks inside the tape.
         */
        struct ChunksInfo {
            ChunksInfo() = default;
            ChunksInfo(ChunkSize chunk_size, TapeSize tape_size);

            /**
             * Number of chunks in the tape.
             */
            TapeSize count_of_chunks_{};
            /**
             * The maximum possible size of the chunk.
             * All the chunks have this size except the last one.
             * The last chunk can also be of this size,
             * but nevertheless its actual size lies in the last_size_chunk_ variable.
             */
            ChunkSize max_size_chunk_{};
            /**
             * The size of the last chunk.
             * last_size_chunk_ is less than or equal to max_size_chunk_.
             */
            ChunkSize last_size_chunk_{};
        };

        /**
         * Initializing the first chunk.
         * 
         * @return false if initialization happened earlier and 
         *         true if initialization did not happen before and it is now completed
         */
        bool InitFirstChunk();

        /**
         * Read the chunk to the right of the current one.
         */
        void ReadChunkToTheRight();
        /**
         * Read the chunk to the left of the current one.
         */
        void ReadChunkToTheLeft();

        /**
         * Rewrite tape from one file to another.
         *
         * @param from file stream from where the tape is being read
         * @param to file stream where the tape is recorded
         */
        static void RewriteFromTo(std::fstream &from, std::fstream &to);

        /**
         * Put a new number in the current chunk.
         *
         * @param to file stream where the chunk with the new number will be placed
         * @param size size for the new chunk
         * @param pos position to put the new number on
         * @param number number to put
         */
        void PutNumberInNewChunk(std::fstream &to, ChunkSize size, ChunkSize pos, NumberType number);

        /**
         * Read and write new chunk.
         *
         * @param from file stream from where the chunk is read
         * @param to file stream where the chunk is written
         * @param new_chunk_number number of new chunk
         * @param new_size size of new chunk
         */
        void ReadAndWriteNewChunk(std::fstream &from, std::fstream &to,
                                  ChunksCount new_chunk_number,
                                  ChunkSize new_size);

        /**
         * Delays in reading, putting and shifting.
         */
        Delays delays_;

        /**
         * Path to the file where the tape is located.
         */
        std::filesystem::path path_;
        /**
         * File stream from where the tape is read.
         */
        std::fstream stream_from_;

        /**
         * Number  of elements (numbers) of the tape.
         */
        TapeSize size_{};

        /**
         * Information about chunks.
         */
        ChunksInfo chunks_info_;
        /**
         * The current chunk.
         */
        Chunk current_chunk_;

        /**
         * Tape initialization flag.
         * If the first chunk was not initialized then the variable is true else false.
         */
        bool unused_ = true;

        static const NumberType kDivider = 16;
        const std::filesystem::path kDirForTempTapes_ = "./kDirForTempTapes_/";
    };

} // namespace tape_structure