#pragma once

#include "Adafruit_BME280.h"
#include "sensor.hpp"

namespace modules {
class BMEsensor : public Sensor {
public:
  bool init() override;
  void showDataOnOled() override;
  bool isWorking() override;
  String getDataString() override;
};
} // namespace devices