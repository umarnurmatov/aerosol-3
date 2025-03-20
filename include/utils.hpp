#pragma once
#include <cstdint>

namespace utils {
void print_oled(const char *msg, int y = 1, int x = 1, bool clear = true,
                bool autoprint = true);

// x in [min, max)
int32_t clamp(int32_t x, int32_t min, int32_t max);

void kill();
} // namespace utils
