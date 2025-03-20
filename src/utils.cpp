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

int32_t utils::clamp(int32_t x, int32_t min, int32_t max)
{
  return x < min ? min : (x > (max - 1) ? (max - 1) : x);
}

void utils::kill() {
  for (;;)
    ;
}
