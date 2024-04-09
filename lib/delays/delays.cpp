#include "delays.hpp"

namespace tape_structure {
    Delays::Delays(std::chrono::milliseconds delay_for_read,
                   std::chrono::milliseconds delay_for_put,
                   std::chrono::milliseconds delay_for_shift) : delay_for_read_(delay_for_read),
                                                                delay_for_put_(delay_for_put),
                                                                delay_for_shift_(delay_for_shift) {}
} // namespace tape_structure