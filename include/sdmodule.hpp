#pragma once

#include "utils.hpp"
#include <SD.h>

namespace modules {
class SDmodule {
public:
  bool init();
  int initFile(String);
  int writeFile(String &);
  bool isWorking();

private:
  bool workingstate;
  File file;
  String filename;
};
} // namespace devices
