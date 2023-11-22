#pragma once

#include "sensor.hpp"
#include <TinyGPSPlus.h>

namespace devices {
class GPSsensor : public Sensor {
public:
  GPSsensor();
  bool isDataValid();
  void showDataOnOled() override;
  String getDataString() override;
  bool isWorking() override;
  String getDateTimeString();

private:
  TinyGPSPlus gps;
  void feedSomeData(unsigned long);
};

} // namespace devices