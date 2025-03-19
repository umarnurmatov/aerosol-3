#pragma once

#include "sensor.hpp"

namespace modules {
class GPSsensor : public Sensor {
public:
  GPSsensor();
  bool init() override;
  bool isDataValid();
  void showDataOnOled() override;
  String getDataString() override;
  bool isWorking() override;
  String getDateTimeString();
  String getFileName();

private:
  void feedSomeData(unsigned long);
  const char *getSatelliteString();
};

} // namespace devices
