#pragma once

#include "defines.hpp"
#include "utils.hpp"
#include <TinyGPSPlus.h>

namespace sensors {
class GPSsensor {
public:
  GPSsensor();
  bool isDataValid();
  void showDataOnOled();
  bool isWorking();

private:
  TinyGPSPlus gps;
  bool workingstate;
  void feedSomeData(unsigned long);
};

} // namespace sensors