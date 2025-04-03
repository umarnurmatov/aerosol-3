#include "sdmodule.hpp"
#include "defines.hpp"

using namespace modules;

bool SDmodule::init()
{
  if (!SD.begin())
  {
    ESP_LOGE(defines::ESP_LOG_TAG, "SD card initialization failed");
    workingstate = false;
    return workingstate;
  }
  uint8_t cardType = SD.cardType();
  ESP_LOGI(defines::ESP_LOG_TAG, "SD card type: %d", cardType);

  if (cardType == CARD_NONE)
  {
    ESP_LOGE(defines::ESP_LOG_TAG, "Card type not recognized");
    workingstate = false;
    return workingstate;
  }

  ESP_LOGI(defines::ESP_LOG_TAG, "SD Card Size: %lluMB", SD.cardSize());
  utils::print_oled("SD INIT SUCCESS");
  workingstate = true;
  return workingstate;
}

int SDmodule::initFile(String _filename)
{
  if (_filename == "")
  {
    ESP_LOGW(defines::ESP_LOG_TAG, "Empty filename provided");
    int counter = 1;
    do
    {
      filename = "/" + String(counter++) + ".csv";
    } while (SD.exists(filename));
  }
  else
  {
    filename = _filename;
  }

  file = SD.open(filename, FILE_WRITE, true);
  if (!file)
    return 1;
  ESP_LOGI(defines::ESP_LOG_TAG, "Initialized file %s", filename);
  file.println(
      "date;time;PM2.5;PM10;lat;lng;alt;bmeAlt;humidity;pressure;temp");
  file.close();

  return 0;
}

int SDmodule::writeFile(String &msg)
{
  file = SD.open(filename, FILE_APPEND);
  if (!file)
    ESP_LOGE(defines::ESP_LOG_TAG, "Failed to open %s for write", filename);

  if (file.println(msg)) {
    ESP_LOGI(defines::ESP_LOG_TAG, "Data was written to %s", filename);
    file.close();
    return 0;
  }
  else
    ESP_LOGI(defines::ESP_LOG_TAG, "Failed to write to %s", filename);

  return 1;
}

bool SDmodule::isWorking() { return workingstate; }
