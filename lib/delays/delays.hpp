#pragma once

#include <chrono>
#include <future>

namespace tape_structure {
    using namespace std::chrono_literals;

    /**
     * Delays for processes.
     */
    struct Delays {
        Delays() = default;
        Delays(std::chrono::milliseconds delay_for_read,
               std::chrono::milliseconds delay_for_put,
               std::chrono::milliseconds delay_for_shift);

        /**
         * Delay in reading the number indicated by the magnetic head.
         */
        std::chrono::milliseconds delay_for_read_{};
        /**
         * Delay in putting the number to the position indicated by the magnetic head.
         */
        std::chrono::milliseconds delay_for_put_{};
        /**
         * Delay for moving the tape by one position.
         */
        std::chrono::milliseconds delay_for_shift_{};
    };
} // namespace tape_structure