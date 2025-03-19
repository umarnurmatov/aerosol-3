#include "sdmodule.hpp"
#include "defines.hpp"

using namespace modules;

bool SDmodule::init()
{
  if (!SD.begin(5))
  {
    utils::print_oled("SD INIT FAIL");
    workingstate = false;
    return workingstate;
  }
  uint8_t cardType = SD.cardType();
  ESP_LOGI(defines::ESP_LOG_TAG, "SD card type: %d", cardType);

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    workingstate = false;
    return workingstate;
  }

  ESP_LOGI(defines::ESP_LOG_TAG, "SD Card Size: %lluMB\n", SD.cardSize());
  utils::print_oled("SD INIT SUCCESS");
  workingstate = true;
  return workingstate;
}

bool SDmodule::initFile(String _filename)
{
  if (_filename == "")
  {
    ESP_LOGW(defines::ESP_LOG_TAG, "Empty filename");
    int counter = 1;
    do
    {
      filename = "/" + String(counter++) + ".csv";
      Serial.println(filename);
    } while (SD.exists(filename));
  }
  else
  {
    filename = _filename;
  }

  file = SD.open(filename, FILE_WRITE, true);
  if (!file)
    return false;
  file.println(
      "date;time;PM2.5;PM10;lat;lng;alt;bmeAlt;humidity;pressure;temp");
  file.close();

  return true;
}

void SDmodule::writeFile(String &msg)
{
  file = SD.open(filename, FILE_APPEND);
  if (!file)
  {
    utils::print_oled("FAILED TO OPEN FILE TO WRITE", 1, 1, true);
    utils::kill();
  }

  if (file.println(msg))
  {
    utils::print_oled("FILE WRITTEN", 6, 1, false);
    utils::print_oled(filename.c_str(), 7, 1, false);
  }
  else
  {
    Serial.println("Write failed");
    utils::print_oled("WRITE FAIL", 1, 1, true);
    // file.close();
    // utils::kill();
  }

  file.close();
}

bool SDmodule::isWorking() { return workingstate; }
