#include "utils.hpp"

void utils::print_oled(const char *msg, int y, int x, bool clear,
                       bool autoprint) {
  if (clear)
    defines::oled.clear();

  defines::oled.autoPrintln(autoprint);
  defines::oled.setCursor(x, y);
  defines::oled.print(msg);
  defines::oled.update();
}

void utils::kill() {
  for (;;)
    ;
}