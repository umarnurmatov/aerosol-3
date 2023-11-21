#include "utils.hpp"

using namespace utils;

void print_oled(const char *msg, int y = 1, int x = 1, bool clear = 1,
                bool autoprint = 1) {
  if (clear)
    defines::oled.clear();

  defines::oled.autoPrintln(autoprint);
  defines::oled.setCursor(x, y);
  defines::oled.print(msg);
  defines::oled.update();
}