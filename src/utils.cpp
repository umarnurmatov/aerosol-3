#include "utils.hpp"
#include "devices.hpp"

void utils::print_oled(const char *msg, int y, int x, bool clear,
                       bool autoprint) {
  if (clear)
    devices::oled.clear();

  devices::oled.autoPrintln(autoprint);
  devices::oled.setCursor(x, y);
  devices::oled.print(msg);
  devices::oled.update();
}

void utils::kill() {
  for (;;)
    ;
}