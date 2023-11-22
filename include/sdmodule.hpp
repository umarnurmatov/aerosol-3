#pragma once

#include "utils.hpp"
#include <SD.h>

namespace devices {
class SDmodule {
public:
  SDmodule();
  bool initFile(String);
  void writeFile(String &);
  bool isWorking();

private:
  bool workingstate;
  File file;
  String filename;
};
} // namespace devices