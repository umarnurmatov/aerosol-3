#include "bmesensor.hpp"
#include "gpssensor.hpp"
#include "pmsensor.hpp"
#include "sdmodule.hpp"
#include "gui.hpp"

static modules::BMEsensor bme;
static modules::GPSsensor gps;
static modules::PMsensor pm;
static modules::SDmodule sd;
static modules::GUI gui;

void setup()
{
  Serial.begin(115200);
  ESP_LOGI(defines::ESP_LOG_TAG, "Starting...");

  // defines::oled.init();
  // Serial2.begin(defines::PM_SENSOR_BAUD, SERIAL_8N1, -1, -1, false);
  sd.init();
  sd.initFile(gps.getFileName());
  // pinMode(12, INPUT_PULLUP);


  // if (defines::IS_GPS_PRESENT)
  //   gps.init();

  // if (defines::IS_BME_PRESENT)
  //   bme.init();

  // utils::print_oled("MAYBE INIT SUCCESS!");

  gui.init();
}

void loop()
{
  // static unsigned long timet = 0;

  // defines::oled.clear();

  // if (millis() - timet > defines::FILE_WRITE_PEERIOD)
  // {
  //   sd.writeFile(gps.getDateTimeString() + pm.getDataString() +
  //                gps.getDataString() + bme.getDataString() + '\n');
  //   timet = millis();
  // }
  // pm.getDataString();
  // pm.showDataOnOled();

  gui.tick();
}
