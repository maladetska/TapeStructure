#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>
#include <vector>

#include "chunk/chunk.hpp"

namespace tape_structure {
    using namespace std::chrono_literals;
    using TapeSize = ChunksCount;
    using MemorySize = uint32_t;

    class Tape {
    public:
        Tape() = default;

        explicit Tape(std::filesystem::path &path);
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

        static ChunkSize CountChunkSize(MemorySize memory, TapeSize size);

        [[nodiscard]] std::filesystem::path GetPath() const;
        [[nodiscard]] TapeSize GetSize() const;
        [[nodiscard]] TapeSize GetCountOfChunks() const;
        [[nodiscard]] ChunkSize GetMaxChunkSize() const;
        [[nodiscard]] ChunkSize GetMinChunkSize() const;
        [[nodiscard]] std::vector<NumberType> GetChunkNumbers() const;
        [[nodiscard]] NumberType GetCurrentNumber();

        bool MoveRight();
        bool MoveLeft();

        void Put(const NumberType &number);

        void ClearChunkInTape();

        friend class TapeSorter;

    private:
        struct Delays {
            Delays() = default;
            Delays(std::chrono::milliseconds delay_for_read,
                   std::chrono::milliseconds delay_for_put,
                   std::chrono::milliseconds delay_for_shift);

            std::chrono::milliseconds delay_for_read_{};
            std::chrono::milliseconds delay_for_put_{};
            std::chrono::milliseconds delay_for_shift_{};
        };

        struct ChunksInfo {
            ChunksInfo() = default;
            ChunksInfo(ChunkSize chunk_size, TapeSize tape_size);

            TapeSize count_of_chunks_{};

            ChunkSize max_size_chunk_{};
            ChunkSize last_size_chunk_{};
        };

        void CountCapacity();

        bool InitFirstChunk();

        void ReadNextChunk();
        void ReadPrevChunk();

        void PrintInFile(std::fstream& stream_to);

        void PutNumberInChunk(std::fstream &stream_to, ChunkSize size, ChunkSize pos, NumberType number);
        void ReadAndWriteNewChunk(std::fstream &from, std::fstream &to,
                                  ChunksCount new_chunk_number,
                                  ChunkSize new_size);

        Delays delays_;

        std::filesystem::path path_;
        std::fstream stream_from_;

        TapeSize size_{};
        TapeSize capacity_{};

        ChunksInfo chunks_info_;
        Chunk current_chunk_;

        bool unused_ = true;

        static const NumberType kDivider = 16;
        const std::filesystem::path kDirForTempTapes_ = "./kDirForTempTapes_/";
    };

} // namespace tape_structure