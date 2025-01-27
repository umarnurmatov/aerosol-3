#pragma once

#include "sensor.hpp"

namespace devices {
class PMsensor : public Sensor {
public:
  bool init() override;
  void showDataOnOled() override;
  String getDataString() override;
  bool isWorking() override;

private:
  bool readPM();
  float pm10, pm25;
  bool workingstate;

  unsigned short pm10count, pm25count;
};
} // namespace devices
