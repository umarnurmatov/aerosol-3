#pragma once

#include "defines.hpp"
#include "utils.hpp"
#include <TinyGPSPlus.h>

namespace devices {
class GPSsensor {
public:
  GPSsensor();
  bool isDataValid();
  void showDataOnOled();
  String getDataString();
  bool isWorking();
  String getDateTimeString();

private:
  TinyGPSPlus gps;
  String dataString;
  bool workingstate;
  void feedSomeData(unsigned long);
};

} // namespace devices