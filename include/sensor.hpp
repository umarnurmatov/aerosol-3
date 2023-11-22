#pragma once

#include "defines.hpp"
#include "utils.hpp"
#include <FS.h>

namespace devices {
class Sensor {
public:
  virtual void showDataOnOled() = 0;
  virtual bool isWorking() = 0;
  virtual String getDataString() = 0;

protected:
  String dataString;
  bool workingstate;
};
} // namespace devices