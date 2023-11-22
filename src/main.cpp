#include "bmesensor.hpp"
#include "gpssensor.hpp"
#include "pmsensor.hpp"
#include "sdmodule.hpp"

devices::BMEsensor bme;
devices::GPSsensor gps;
devices::PMsensor pm;
devices::SDmodule sd;

void setup() {
  Serial.begin(defines::SERIAL_BAUD);

  if (defines::IS_GPS_PRESENT)
    Serial1.begin(defines::GPS_BAUD, SERIAL_8N1, defines::GPS_RX_PIN,
                  defines::GPS_TX_PIN);

  Serial2.begin(defines::PM_SENSOR_BAUD, SERIAL_8N1, -1, -1, false);

  defines::oled.init();

  if (!sd.isWorking())
    utils::kill();

  sd.initFile(gps.getDateTimeString());

  Serial.println("Init success!");
  utils::print_oled("INIT SUCCESS!");
}

void loop() {
  static unsigned long timet = 0;
  if (millis() - timet < defines::FILE_WRITE_PEERIOD)
    return;

  pm.showDataOnOled();
  bme.showDataOnOled();
  gps.showDataOnOled();

  sd.writeFile(gps.getDataString() + pm.getDataString() + bme.getDataString());
  timet = millis();
}