#include "bmesensor.hpp"
#include "gpssensor.hpp"
#include "pmsensor.hpp"
#include "sdmodule.hpp"

static devices::BMEsensor bme;
static devices::GPSsensor gps;
static devices::PMsensor pm;
static devices::SDmodule sd;

void setup() {
  Serial.begin(defines::SERIAL_BAUD);
  Serial.println("STARTING SERIAL...");

  defines::oled.init();

  pm.init();
  sd.init();
  sd.initFile(gps.getFileName());

  if (!sd.isWorking())
    utils::kill();

  if (defines::IS_GPS_PRESENT)
    gps.init();

  if (defines::IS_BME_PRESENT)
    bme.init();

  utils::print_oled("MAYBE INIT SUCCESS!");
}

void loop() {
  static unsigned long timet = 0;
  if (millis() - timet < defines::FILE_WRITE_PEERIOD)
    return;

  defines::oled.clear();

  pm.showDataOnOled();

  sd.writeFile(gps.getDateTimeString() + pm.getDataString() +
               gps.getDataString() + bme.getDataString() + '\n');
  timet = millis();
}