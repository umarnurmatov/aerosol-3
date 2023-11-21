#pragma once

#include "utils.hpp"
#include <SD.h>

namespace devices {
class SDmodule {
public:
  SDmodule();
  bool initFile();
  void writeFile();

private:
  bool workingstate;
  String filename;
};
} // namespace devices