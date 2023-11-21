#include "sdmodule.hpp"

using namespace devices;

SDmodule::SDmodule() {
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    utils::print_oled("SD INIT FAIL");
    workingstate = false;
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    workingstate = false;
    return;
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  utils::print_oled("SD INIT SUCCESS");
  workingstate = true;
}

bool SDmodule::initFile(String _filename = "") {
  if (!defines::IS_GPS_PRESENT) {
    int counter = 1;
    while (filename = "/" + String(++counter) + ".csv" && SD.exists(filename))
      ;
  }
}