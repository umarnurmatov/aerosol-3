#pragma once

#include "defines.hpp"
#include "utils.hpp"
#include <FS.h>
#include <PMserial.h>

namespace modules {
class Sensor {
public:
  Sensor(bool state = false)
    : workingstate { state }
  {
  } 

  virtual bool init() = 0;
  virtual void showDataOnOled() = 0;
  virtual bool isWorking() = 0;
  virtual String getDataString() = 0;

protected:
  String dataString;
  bool workingstate;
};
} // namespace devices
