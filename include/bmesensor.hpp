#pragma once

#include "Adafruit_BME280.h"
#include "sensor.hpp"

namespace devices {
class BMEsensor : public Sensor {
public:
  bool init() override;
  void showDataOnOled() override;
  bool isWorking() override;
  String getDataString() override;

private:
  Adafruit_BME280 bme;
};
} // namespace devices