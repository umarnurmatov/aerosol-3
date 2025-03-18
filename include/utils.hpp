#pragma once

namespace utils {
void print_oled(const char *msg, int y = 1, int x = 1, bool clear = true,
                bool autoprint = true);
void kill();
} // namespace utils