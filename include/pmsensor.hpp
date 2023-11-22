#pragma once

#include "sensor.hpp"

namespace devices {
class PMsensor : public Sensor {
public:
  PMsensor();
  void showDataOnOled() override;
  String getDataString() override;
  bool isWorking() override;

private:
  bool readPM();
  float pm10, pm25;
  unsigned short pm10count, pm25count;
};
} // namespace devices
