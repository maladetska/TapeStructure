#include "tape.h"

namespace tape_sorter {

    void Tape::InitFirstChunk() {
        file_.open(path_);
        chunk_.resize(max_size_chunk_);
        for (int32_t &num: chunk_) {
            file_ >> num;
        }
    }

    void Tape::CountCapacity() {
        file_.open(path_);
        int32_t number;
        while (file_ >> number) {
            capacity_++;
        }
        if (capacity_ < size_) {
            std::filesystem::create_directories(kDirForTempTapes_);
            std::filesystem::path tmp_path(kDirForTempTapes_);
            tmp_path += "cap_tmp.txt";
            std::fstream tmp(tmp_path, std::fstream::out);
            file_.close();
            file_.open(path_);
            for (size_t i = 0; i < capacity_; i++) {
                file_ >> number;
                tmp << number << ' ';
            }
            while (capacity_ < size_) {
                tmp << 0 << ' ';
                capacity_++;
            }
            tmp.close();
            tmp.open(tmp_path, std::fstream::in);
            file_.seekg(0);
            file_.seekp(0);
            for (size_t i = 0; i < capacity_; i++) {
                tmp >> number;
                file_ << number << ' ';
            }
            tmp.close();
            remove_all(tmp_path);
        }
        file_.close();
    }

    Tape::Tape(std::filesystem::path &p) : path_(p) {}

    Tape::Tape(std::filesystem::path &p,
               size_t s,
               size_t sc,
               std::chrono::milliseconds d1,
               std::chrono::milliseconds d2,
               std::chrono::milliseconds d3) : path_(p),
                                               size_(s),
                                               max_size_chunk_(sc),
                                               delay_for_read_(d1),
                                               delay_for_put_(d2),
                                               delay_for_shift_(d3) {
        CountCapacity();
        count_of_cap_chunks_ = capacity_ == 0 ? 0 : (capacity_ - 1) / max_size_chunk_ + 1;
        last_ptr_ = capacity_ % max_size_chunk_ == 0 ? max_size_chunk_ : capacity_ % max_size_chunk_;
        count_of_chunks_ = (size_ - 1) / max_size_chunk_ + 1;
        last_size_chunk_ = size_ % max_size_chunk_ == 0 ? max_size_chunk_ : size_ % max_size_chunk_;
        chunk_number_ = 0;
        ptr_in_chunk_ = 0;
    }

    Tape::Tape(std::filesystem::path &p, size_t s, size_t sc) : path_(p),
                                                                size_(s),
                                                                max_size_chunk_(sc) {
        CountCapacity();
        count_of_cap_chunks_ = capacity_ == 0 ? 0 : (capacity_ - 1) / max_size_chunk_ + 1;
        last_ptr_ = capacity_ % max_size_chunk_ == 0 ? max_size_chunk_ : capacity_ % max_size_chunk_;
        count_of_chunks_ = (size_ - 1) / max_size_chunk_ + 1;
        last_size_chunk_ = size_ % max_size_chunk_ == 0 ? max_size_chunk_ : size_ % max_size_chunk_;
        chunk_number_ = 0;
        ptr_in_chunk_ = 0;
        delay_for_read_ = 0ms;
        delay_for_put_ = 0ms;
        delay_for_shift_ = 0ms;
    }

    Tape::Tape(const Tape &t) : path_(t.path_),
                                size_(t.size_),
                                max_size_chunk_(t.max_size_chunk_),
                                count_of_cap_chunks_(t.count_of_cap_chunks_),
                                count_of_chunks_(t.count_of_chunks_),
                                last_size_chunk_(t.last_size_chunk_),
                                capacity_(t.capacity_),
                                last_ptr_(t.last_ptr_),
                                delay_for_read_(t.delay_for_read_),
                                delay_for_put_(t.delay_for_put_),
                                delay_for_shift_(t.delay_for_shift_) {
        chunk_number_ = 0;
        ptr_in_chunk_ = 0;
    }

    Tape::Tape(std::filesystem::path &p, const Tape &t)
        : path_(p), size_(t.size_),
          max_size_chunk_(t.max_size_chunk_),
          count_of_cap_chunks_(t.count_of_cap_chunks_),
          count_of_chunks_(t.count_of_chunks_),
          last_size_chunk_(t.last_size_chunk_),
          capacity_(t.capacity_),
          last_ptr_(t.last_ptr_),
          delay_for_read_(t.delay_for_read_),
          delay_for_put_(t.delay_for_put_),
          delay_for_shift_(t.delay_for_shift_) {
        file_.open(path_);
        std::ifstream t_file(t.path_);
        int32_t num;
        while (t_file >> num) {
            file_ << num << ' ';
        }
        file_.close();
        t_file.close();
        chunk_number_ = 0;
        ptr_in_chunk_ = 0;
    }

    Tape &Tape::operator=(const Tape &t) {
        size_ = t.size_;
        max_size_chunk_ = t.max_size_chunk_;
        count_of_cap_chunks_ = t.count_of_cap_chunks_;
        count_of_chunks_ = t.count_of_chunks_;
        last_size_chunk_ = t.last_size_chunk_;
        capacity_ = t.capacity_;
        last_ptr_ = t.last_ptr_;
        delay_for_read_ = t.delay_for_read_;
        delay_for_put_ = t.delay_for_put_;
        delay_for_shift_ = t.delay_for_shift_;
        if (exists(path_)) {
            if (file_.is_open()) file_.close();
            file_.open(path_, std::ios::in | std::ios::out);
            std::ifstream t_file(t.path_);
            int32_t num;
            while (t_file >> num) {
                file_ << num << ' ';
            }
            file_.close();
        } else {
            path_ = t.path_;
        }
        chunk_number_ = 0;
        ptr_in_chunk_ = 0;
        return *this;
    }

    Tape &Tape::operator=(Tape &&t) noexcept {
        size_ = t.size_;
        count_of_cap_chunks_ = t.count_of_cap_chunks_;
        count_of_chunks_ = t.count_of_chunks_;
        last_size_chunk_ = t.last_size_chunk_;
        max_size_chunk_ = t.max_size_chunk_;
        capacity_ = t.capacity_;
        last_ptr_ = t.last_ptr_;
        delay_for_read_ = t.delay_for_read_;
        delay_for_put_ = t.delay_for_put_;
        delay_for_shift_ = t.delay_for_shift_;
        if (exists(path_)) {
            if (file_.is_open()) file_.close();
            file_.open(path_, std::ios::in | std::ios::out);
            t.file_.close();
            t.file_.open(t.path_, std::ios::in | std::ios::out);
            int32_t num;
            while (t.file_ >> num) {
                file_ << num << ' ';
            }
            file_.close();
        } else {
            path_ = t.path_;
        }
        chunk_number_ = 0;
        ptr_in_chunk_ = 0;
        t.path_ = "";
        t.size_ = 0;
        t.count_of_cap_chunks_ = 0;
        t.count_of_chunks_ = 0;
        t.last_size_chunk_ = 0;
        t.max_size_chunk_ = 0;
        t.capacity_ = 0;
        t.last_ptr_ = 0;
        delay_for_read_ = 0ms;
        delay_for_put_ = 0ms;
        delay_for_shift_ = 0ms;
        t.file_.close();

        return *this;
    }

    size_t Tape::GetSize() const {
        return size_;
    }

    size_t Tape::GetCapacity() const {
        return capacity_;
    }

    std::chrono::milliseconds Tape::GetDelayForRead() const {
        return delay_for_read_;
    }
    std::chrono::milliseconds Tape::GetDelayForPut() const {
        return delay_for_put_;
    }
    std::chrono::milliseconds Tape::GetDelayForShift() const {
        return delay_for_shift_;
    }

    size_t Tape::GetCountOfChunks() const {
        return count_of_chunks_;
    }

    std::filesystem::path Tape::GetPath() const {
        return path_;
    }

    size_t Tape::GetPtrInChunk() const {
        return ptr_in_chunk_;
    }

    int32_t Tape::GetCurr() {
        std::this_thread::sleep_for(delay_for_read_);
        if (unused_) {
            InitFirstChunk();
            unused_ = false;
        }
        return chunk_[ptr_in_chunk_];
    }

    void *Tape::GetCurrPos() {
        return &ptr_in_chunk_;
    }

    size_t Tape::GetMaxSizeChunk() const {
        return max_size_chunk_;
    }

    size_t Tape::GetMinSizeChunk() const {
        return last_size_chunk_;
    }

    void Tape::ReadNextChunk() {
        if (unused_) {
            InitFirstChunk();
            unused_ = false;
        }
        chunk_number_++;
        ptr_in_chunk_ = 0;
        if (chunk_number_ == count_of_chunks_ - 1) {
            chunk_.resize(last_size_chunk_);
        } else {
            chunk_.resize(max_size_chunk_);
        }
        for (int32_t &i: chunk_) {
            file_ >> i;
        }
    }

    void Tape::ReadPrevChunk() {
        chunk_number_--;
        chunk_.resize(max_size_chunk_);
        file_.seekp(0);
        file_.seekg(0);
        for (size_t i = 0; i < chunk_number_ * max_size_chunk_; i++) {
            int32_t x;
            file_ >> x;
        }
        for (int32_t &i: chunk_) {
            file_ >> i;
        }
        ptr_in_chunk_ = max_size_chunk_ - 1;
    }

    bool Tape::MoveLeft() {
        std::this_thread::sleep_for(delay_for_shift_);
        if (unused_) {
            InitFirstChunk();
            unused_ = false;
        }
        if (chunk_number_ == count_of_chunks_ - 1 && ptr_in_chunk_ == last_size_chunk_ - 1) {
            return false;
        }
        if (ptr_in_chunk_ != chunk_.size() - 1) {
            ptr_in_chunk_++;
        } else {
            ReadNextChunk();
        }
        return true;
    }

    bool Tape::MoveRight() {
        std::this_thread::sleep_for(delay_for_shift_);
        if (unused_) {
            InitFirstChunk();
            unused_ = false;
        }
        if (chunk_number_ == 0 && ptr_in_chunk_ == 0) {
            return false;
        }
        if (ptr_in_chunk_ != 0) {
            ptr_in_chunk_--;
        } else {
            ReadPrevChunk();
        }

        return true;
    }

    int32_t Tape::GetLeft() {
        std::this_thread::sleep_for(delay_for_shift_);
        if (unused_ || !MoveRight()) {
            throw std::out_of_range("Out of Tape range");
        } else {
            return GetCurr();
        }
    }

    int32_t Tape::GetRight() {
        std::this_thread::sleep_for(delay_for_shift_);
        if (unused_) {
            InitFirstChunk();
            unused_ = false;
        }
        if (MoveLeft()) {
            return GetCurr();
        } else {
            throw std::out_of_range("Out of Tape range");
        }
    }

    std::vector<int32_t> &Tape::ReadOneChunk(std::fstream &from, std::vector<int32_t> &ch, size_t s) {
        ch.resize(s);
        for (size_t i = 0; i < s; i++) {
            from >> ch[i];
        }

        return ch;
    }

    void Tape::PrintOneChunk(std::fstream &to, std::vector<int32_t> &ch) {
        for (int32_t &i: ch) {
            to << i << ' ';
        }
    }

    void Tape::PutNumberInChunk(std::fstream &to, const int32_t &number, size_t s) {
        chunk_.resize(s);
        for (size_t i = 0; i < s; i++) {
            file_ >> chunk_[i];
        }
        chunk_[ptr_in_chunk_] = number;
        PrintOneChunk(to, chunk_);
    }

    void Tape::DestroyChunk() {
        chunk_.clear();
    }

    void Tape::Put(const int32_t &number) {
        std::this_thread::sleep_for(delay_for_put_);
        if (unused_) {
            InitFirstChunk();
            unused_ = false;
        }
        std::filesystem::create_directories(kDirForTempTapes_);
        std::filesystem::path tmp_path(kDirForTempTapes_);
        tmp_path += "print_tmp.txt";
        std::fstream tmp(tmp_path, std::fstream::out);
        file_.seekg(0);
        file_.seekp(0);
        chunk_.resize(max_size_chunk_);
        if (count_of_chunks_ == 1) {
            PutNumberInChunk(tmp, number, max_size_chunk_);
        } else if (chunk_number_ == count_of_chunks_ - 1) {
            for (size_t i = 0; i < count_of_chunks_ - 1; i++) {
                PrintOneChunk(tmp, ReadOneChunk(file_, chunk_, max_size_chunk_));
            }
            PutNumberInChunk(tmp, number, last_size_chunk_);
        } else if (chunk_number_ == 0) {
            PutNumberInChunk(tmp, number, max_size_chunk_);
            for (size_t i = 1; i < count_of_chunks_ - 1; i++) {
                PrintOneChunk(tmp, ReadOneChunk(file_, chunk_, max_size_chunk_));
            }
            PrintOneChunk(tmp, ReadOneChunk(file_, chunk_, last_size_chunk_));
        } else {
            for (size_t i = 0; i < chunk_number_; i++) {
                PrintOneChunk(tmp, ReadOneChunk(file_, chunk_, max_size_chunk_));
            }
            PutNumberInChunk(tmp, number, max_size_chunk_);
            for (size_t i = chunk_number_ + 1; i < count_of_chunks_ - 1; i++) {
                PrintOneChunk(tmp, ReadOneChunk(file_, chunk_, max_size_chunk_));
            }
            PrintOneChunk(tmp, ReadOneChunk(file_, chunk_, last_size_chunk_));
        }

        file_.close();
        file_.open(path_, std::ios::in | std::ios::out);
        tmp.close();
        tmp.open(tmp_path, std::ios::in);
        file_.seekg(0);
        file_.seekp(0);
        tmp.seekg(0);
        tmp.seekp(0);

        chunk_.resize(max_size_chunk_);
        for (size_t i = 0; i < count_of_chunks_ - 1; i++) {
            PrintOneChunk(file_, ReadOneChunk(tmp, chunk_, max_size_chunk_));
        }
        PrintOneChunk(file_, ReadOneChunk(tmp, chunk_, last_size_chunk_));

        tmp.close();
        std::filesystem::remove_all(kDirForTempTapes_);

        size_t curr_ptr = ptr_in_chunk_;
        size_t curr_chunk = chunk_number_;
        ptr_in_chunk_ = last_size_chunk_;
        chunk_number_ = count_of_chunks_ - 1;
        while (curr_ptr != ptr_in_chunk_ || curr_chunk != chunk_number_) {
            MoveRight();
        }
    }

}  // namespace tape_sorter