#include "bmesensor.hpp"
#include "utils.hpp"

using namespace devices;

BMEsensor::BMEsensor() {
  dataString = "";
  if (!defines::IS_BME_PRESENT)
    return;

  workingstate = bme.begin(0x76);

  if (workingstate) {
    return;
  }
  Serial.println("Could not find a valid BME280 sensor, check wiring!");
  utils::print_oled("NO BME SENSOR");
}

bool BMEsensor::isWorking() { return workingstate; }

void BMEsensor::showDataOnOled() {
  if (!defines::IS_BME_PRESENT)
    return;

  dataString = String(bme.readAltitude(defines::SEALVLPRESSURE_HPA)) + "," +
               String(bme.readHumidity()) + "," + String(bme.readPressure()) +
               "," + String(bme.readTemperature());
  utils::print_oled(dataString.c_str());
}

String BMEsensor::getDataString() { return dataString; }