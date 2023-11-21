#pragma once

#include "defines.hpp"
#include "utils.hpp"

namespace devices {
class PMsensor {
public:
  PMsensor();
  bool readPM();
  void showDataOnOled();
  String getDataString();

private:
  float pm10, pm25;
  unsigned short pm10count, pm25count;
  String dataString;
};
} // namespace devices
