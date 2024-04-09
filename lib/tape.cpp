#include "tape.hpp"

namespace tape_structure {

    Tape::Tape(std::filesystem::path& path)
        : path_(path),
          current_chunk_(Chunk(0, chunks_info_.max_size_chunk_)) {}

    Tape::Tape(std::filesystem::path& path,
               TapeSize tape_size,
               ChunkSize chunk_size,
               std::chrono::milliseconds delay_for_read,
               std::chrono::milliseconds delay_for_put,
               std::chrono::milliseconds delay_for_shift) : path_(path),
                                                            delays_(delay_for_read, delay_for_put, delay_for_shift),
                                                            size_(tape_size) {
        chunks_info_ = ChunksInfo(chunk_size, size_);
        current_chunk_ = Chunk(0, chunks_info_.max_size_chunk_);
        stream_from_.open(path_);
    }

    Tape::Tape(std::filesystem::path& path, TapeSize tape_size, ChunkSize chunk_size)
        : Tape(path, tape_size, chunk_size, 0ms, 0ms, 0ms) {}

    Tape::Tape(const Tape& other) : path_(other.path_),
                                    delays_(other.delays_),
                                    size_(other.size_),
                                    chunks_info_(other.chunks_info_),
                                    current_chunk_(other.current_chunk_) {}

    void Tape::RewriteFromTo(std::fstream& from, std::fstream& to) {
        NumberType num;
        while (from >> num) {
            to << num << ' ';
        }
    }

    Tape::Tape(const Tape& other, std::filesystem::path& path) : Tape(other) {
        path_ = path;
        stream_from_.open(path_);
        std::fstream other_file(other.path_);
        RewriteFromTo(other_file, stream_from_);
        stream_from_.close();
        other_file.close();
    }

    Tape& Tape::operator=(const Tape& other) {
        path_ = other.path_;
        delays_ = other.delays_;
        size_ = other.size_;
        chunks_info_ = other.chunks_info_;
        current_chunk_ = other.current_chunk_;
        unused_ = other.unused_;

        if (exists(path_)) {
            if (stream_from_.is_open()) {
                stream_from_.close();
            }
            stream_from_.open(path_, std::ios::in | std::ios::out);
            std::fstream other_file(other.path_);
            RewriteFromTo(other_file, stream_from_);
            stream_from_.close();
        } else {
            path_ = other.path_;
        }

        return *this;
    }

    Tape::Tape(Tape&& other) noexcept {
        *this = std::move(other);
    }

    Tape& Tape::operator=(Tape&& other) noexcept {
        if (&other == this) {
            return *this;
        }

        std::swap(other.delays_, delays_);
        std::swap(other.size_, size_);
        std::swap(other.chunks_info_, chunks_info_);
        std::swap(other.current_chunk_, current_chunk_);
        std::swap(other.unused_, unused_);

        if (exists(path_)) {
            if (stream_from_.is_open()) stream_from_.close();
            stream_from_.open(path_, std::ios::in | std::ios::out);
            other.stream_from_.close();
            other.stream_from_.open(other.path_, std::ios::in | std::ios::out);
            RewriteFromTo(other.stream_from_, stream_from_);
            stream_from_.close();
        } else {
            path_ = other.path_;
        }
        other.path_ = "";
        other.stream_from_.close();

        return *this;
    }

    Tape::~Tape() {
        stream_from_.close();
    }

    ChunkSize Tape::CountChunkSize(MemorySize memory, TapeSize size) {
        return std::min(memory / kDivider, size);
    }

    std::filesystem::path Tape::GetPath() const {
        return path_;
    }

    TapeSize Tape::GetSize() const {
        return size_;
    }

    TapeSize Tape::GetCountOfChunks() const {
        return chunks_info_.count_of_chunks_;
    }

    TapeSize Tape::GetMaxChunkSize() const {
        return chunks_info_.max_size_chunk_;
    }

    TapeSize Tape::GetMinChunkSize() const {
        return chunks_info_.last_size_chunk_;
    }

    std::vector<NumberType> Tape::GetChunkNumbers() const {
        return current_chunk_.GetChunkNumbers();
    }

    NumberType Tape::GetCurrentNumber() {
        std::this_thread::sleep_for(delays_.delay_for_read_);
        if (InitFirstChunk()) {
            current_chunk_.MoveToLeftEdge();
        }

        return current_chunk_.GetCurrentNumber();
    }

    bool Tape::MoveRight() {
        std::this_thread::sleep_for(delays_.delay_for_shift_);
        if (InitFirstChunk()) {
            current_chunk_.MoveToLeftEdge();
        }

        if (current_chunk_.IsPossibleTakeLeftNumber()) {
            if (!current_chunk_.MoveRightPos()) {
                ReadChunkToTheLeft();
            }
            return true;
        }
        return false;
    }

    bool Tape::MoveLeft() {
        std::this_thread::sleep_for(delays_.delay_for_shift_);
        if (InitFirstChunk()) {
            current_chunk_.MoveToLeftEdge();
        }

        if (current_chunk_.IsPossibleTakeRightNumber(chunks_info_.count_of_chunks_)) {
            if (!current_chunk_.MoveLeftPos()) {
                ReadChunkToTheRight();
            }
            return true;
        }
        return false;
    }

    void Tape::Put(const NumberType& number) {
        std::this_thread::sleep_for(delays_.delay_for_put_);

        ChunkSize current_pos = current_chunk_.GetPos();
        ChunksCount current_chunk_number = current_chunk_.GetChunkNumber();
        if (InitFirstChunk()) {
            current_chunk_.MoveRightPos();
        }
        std::filesystem::create_directories(kDirForTempTapes_);

        std::filesystem::path tmp_path(kDirForTempTapes_);
        tmp_path += "print_tmp.txt";
        std::fstream tmp_to(tmp_path, std::fstream::out);

        stream_from_.seekg(0);
        stream_from_.seekp(0);

        if (chunks_info_.count_of_chunks_ == 1) {
            PutNumberInNewChunk(tmp_to, chunks_info_.max_size_chunk_, current_pos, number);
        } else if (current_chunk_number == chunks_info_.count_of_chunks_ - 1) {
            for (TapeSize i = 0; i < chunks_info_.count_of_chunks_ - 1; i++) {
                ReadAndWriteNewChunk(stream_from_, tmp_to, i, chunks_info_.max_size_chunk_);
            }
            PutNumberInNewChunk(tmp_to, chunks_info_.last_size_chunk_, current_pos, number);
        } else if (current_chunk_number == 0) {
            PutNumberInNewChunk(tmp_to, chunks_info_.max_size_chunk_, current_pos, number);
            for (TapeSize i = 1; i < chunks_info_.count_of_chunks_ - 1; i++) {
                ReadAndWriteNewChunk(stream_from_, tmp_to, i, chunks_info_.max_size_chunk_);
            }
            ReadAndWriteNewChunk(stream_from_,
                                 tmp_to,
                                 chunks_info_.count_of_chunks_ - 1,
                                 chunks_info_.last_size_chunk_);
        } else {
            for (TapeSize i = 0; i < current_chunk_number; i++) {
                ReadAndWriteNewChunk(stream_from_, tmp_to, i, chunks_info_.max_size_chunk_);
            }
            PutNumberInNewChunk(tmp_to, chunks_info_.max_size_chunk_, current_pos, number);
            for (TapeSize i = current_chunk_number + 1; i < chunks_info_.count_of_chunks_ - 1; i++) {
                ReadAndWriteNewChunk(stream_from_, tmp_to, i, chunks_info_.max_size_chunk_);
            }
            ReadAndWriteNewChunk(stream_from_,
                                 tmp_to,
                                 chunks_info_.count_of_chunks_ - 1,
                                 chunks_info_.last_size_chunk_);
        }

        stream_from_.close();
        stream_from_.open(path_, std::ios::in | std::ios::out);
        tmp_to.close();
        tmp_to.open(tmp_path, std::ios::in);
        stream_from_.seekg(0);
        stream_from_.seekp(0);
        tmp_to.seekg(0);
        tmp_to.seekp(0);

        for (TapeSize i = 0; i < chunks_info_.count_of_chunks_ - 1; i++) {
            ReadAndWriteNewChunk(tmp_to, stream_from_, i, chunks_info_.max_size_chunk_);
        }
        ReadAndWriteNewChunk(
                tmp_to,
                stream_from_,
                chunks_info_.count_of_chunks_ - 1,
                chunks_info_.last_size_chunk_);

        tmp_to.close();
        std::filesystem::remove_all(kDirForTempTapes_);

        while (!current_chunk_.IsMatchWith(current_pos, current_chunk_number)) {
            MoveRight();
        }
    }

    void Tape::ClearChunkInTape() {
        current_chunk_.Destroy();
    }

    bool Tape::InitFirstChunk() {
        if (unused_) {
            stream_from_.open(path_);
            current_chunk_.ReadNewChunk(stream_from_, 0, chunks_info_.max_size_chunk_);
            unused_ = false;

            return true;
        }
        return false;
    }

    void Tape::ReadChunkToTheRight() {
        if (InitFirstChunk()) {
            return;
        }

        ChunksCount current_chunk_number = current_chunk_.GetChunkNumber();
        current_chunk_.ReadNewChunk(stream_from_,
                                    current_chunk_number + 1,
                                    current_chunk_number + 1 == chunks_info_.count_of_chunks_ - 1
                                            ? chunks_info_.last_size_chunk_
                                            : chunks_info_.max_size_chunk_);
        current_chunk_.MoveToLeftEdge();
    }

    void Tape::ReadChunkToTheLeft() {
        stream_from_.seekp(0);
        stream_from_.seekg(0);

        ChunksCount current_chunk_number = current_chunk_.GetChunkNumber();

        for (ChunkSize i = 0; i < (current_chunk_number - 1) * chunks_info_.max_size_chunk_; i++) {
            NumberType number;
            stream_from_ >> number;
        }

        current_chunk_.ReadNewChunk(stream_from_,
                                    current_chunk_number - 1,
                                    chunks_info_.max_size_chunk_);
        current_chunk_.MoveToRightEdge();
    }

    void Tape::PutNumberInNewChunk(std::fstream& to, ChunkSize size, ChunkSize pos, NumberType number) {
        current_chunk_.ReadNewChunk(stream_from_,
                                    current_chunk_.GetChunkNumber(),
                                    size);
        current_chunk_.PutNumberInArrayByPos(number, pos);
        current_chunk_.PrintChunk(to);
    }

    void Tape::ReadAndWriteNewChunk(std::fstream& from,
                                    std::fstream& to,
                                    ChunksCount new_chunk_number,
                                    ChunkSize new_size) {
        current_chunk_.ReadNewChunk(from, new_chunk_number, new_size);
        current_chunk_.PrintChunk(to);
    }

    Tape::Delays::Delays(std::chrono::milliseconds delay_for_read,
                         std::chrono::milliseconds delay_for_put,
                         std::chrono::milliseconds delay_for_shift) : delay_for_read_(delay_for_read),
                                                                      delay_for_put_(delay_for_put),
                                                                      delay_for_shift_(delay_for_shift) {}

    Tape::ChunksInfo::ChunksInfo::ChunksInfo(ChunkSize chunk_size, TapeSize tape_size) {
        max_size_chunk_ = chunk_size;
        count_of_chunks_ = (tape_size - 1) / max_size_chunk_ + 1;
        last_size_chunk_ = tape_size % max_size_chunk_ == 0 ? max_size_chunk_ : tape_size % max_size_chunk_;
    }
} // namespace tape_structure