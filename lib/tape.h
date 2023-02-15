#pragma once

//#include "tape_sorter.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>
#include <vector>

namespace tape_sorter {

    using namespace std::chrono_literals;

    class Tape {
    public:
        class Iterator : public std::iterator<std::bidirectional_iterator_tag, int> {
        private:
            int *ptr;

        public:
            Iterator() : ptr(nullptr) {}
            explicit Iterator(int *ptr) : ptr(ptr) {}
            Iterator(const Iterator &iter) : ptr(iter.ptr) {}

            Iterator operator++() {
                ptr++;
                return *this;
            }

            Iterator operator++(int) {
                Iterator tmp(*this);
                ++ptr;
                return tmp;
            }

            Iterator &operator--() {
                ptr--;
                return *this;
            }

            Iterator operator--(int) {
                Iterator tmp(*this);
                --ptr;
                return tmp;
            }

            int &operator*() const {
                return *ptr;
            }

            int *operator->() const {
                return ptr;
            }

            bool operator==(const Iterator &iterator) const {
                return iterator.ptr == this->ptr;
            }

            bool operator!=(const Iterator &iterator) const {
                return iterator.ptr != this->ptr;
            }

            bool operator<(const Iterator &iterator) const {
                return iterator.ptr < this->ptr;
            }

            bool operator>(const Iterator &iterator) const {
                return iterator.ptr > this->ptr;
            }

            bool operator>=(const Iterator &iterator) const {
                return iterator.ptr >= this->ptr;
            }

            bool operator<=(const Iterator &iterator) const {
                return iterator.ptr <= this->ptr;
            }
        };

        Tape() = default;

        explicit Tape(std::filesystem::path &p);
        Tape(std::filesystem::path &p,
             size_t s,
             size_t sc,
             std::chrono::milliseconds d1,
             std::chrono::milliseconds d2,
             std::chrono::milliseconds d3);
        Tape(std::filesystem::path &p, size_t s, size_t sc);
        Tape(std::filesystem::path &p, const Tape &t);
        Tape(const Tape &t);
        Tape &operator=(const Tape &t);
        Tape &operator=(Tape &&t) noexcept;

        size_t GetSize() const;
        size_t GetCapacity() const;
        std::filesystem::path GetPath() const;
        std::chrono::milliseconds GetDelayForRead() const;
        std::chrono::milliseconds GetDelayForPut() const;
        std::chrono::milliseconds GetDelayForShift() const;
        size_t GetMaxSizeChunk() const;
        size_t GetMinSizeChunk() const;
        size_t GetPtrInChunk() const;
        void *GetCurrPos();
        size_t GetCountOfChunks() const;
        int32_t GetCurr();
        int32_t GetLeft();
        int32_t GetRight();
        bool MoveRight();
        bool MoveLeft();

        void Put(const int32_t &number);

        ~Tape() {
            file_.close();
        }

        friend class TapeSorter;

    private:
        size_t size_{};
        size_t capacity_{};
        std::filesystem::path path_;
        std::fstream file_;

        std::chrono::milliseconds delay_for_read_{};
        std::chrono::milliseconds delay_for_put_{};
        std::chrono::milliseconds delay_for_shift_{};

        const std::filesystem::path kDirForTempTapes_ = "./kDirForTempTapes_/";

        size_t max_size_chunk_{};
        size_t last_size_chunk_{};
        std::vector<int32_t> chunk_;
        size_t count_of_cap_chunks_{};
        size_t count_of_chunks_{};
        size_t ptr_in_chunk_{};
        size_t last_ptr_{};
        size_t chunk_number_{};

        bool unused_ = true;

        void InitFirstChunk();
        void CountCapacity();
        void ReadNextChunk();
        void ReadPrevChunk();
        static std::vector<int32_t> &ReadOneChunk(std::fstream &from, std::vector<int32_t> &ch, size_t s);
        static void PrintOneChunk(std::fstream &to, std::vector<int32_t> &ch);
        void PutNumberInChunk(std::fstream &to, const int32_t &number, size_t s);
        void DestroyChunk();
    };

}  // namespace tape_sorter