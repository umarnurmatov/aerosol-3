#pragma once

#include "defines.hpp"
#include <FS.h>

namespace sensors {
class PMsensor {
public:
  PMsensor();
  bool readPM();
  String getDataString();

private:
  float pm10, pm25;
  unsigned short pm10count, pm25count;
};
} // namespace sensors
