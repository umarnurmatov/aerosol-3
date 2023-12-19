#pragma once

#include "utils.hpp"
#include <SD.h>

namespace devices {
class SDmodule {
public:
  bool init();
  bool initFile(String);
  void writeFile(String &);
  bool isWorking();

private:
  bool workingstate;
  File file;
  String filename;
};
} // namespace devices