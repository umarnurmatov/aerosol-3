#include "bmesensor.hpp"
#include "utils.hpp"

using namespace devices;

bool BMEsensor::init() {
  workingstate = bme.begin(0x76);

  if (workingstate) {
    Serial.println("BM OK!");
    return true;
  }
  Serial.println("Could not find a valid BME280 sensor, check wiring!");
  utils::print_oled("NO BME SENSOR");
  return false;
}

bool BMEsensor::isWorking() {
  return defines::IS_BME_PRESENT ? workingstate : true;
}

void BMEsensor::showDataOnOled() {
  if (!defines::IS_BME_PRESENT)
    return;

  dataString = String(bme.readAltitude(defines::SEALVLPRESSURE_HPA)) + ';' +
               String(bme.readHumidity()) + ';' + String(bme.readPressure()) +
               ";" + String(bme.readTemperature() + '\n');
  // utils::print_oled(dataString.c_str());
}

String BMEsensor::getDataString() { return dataString; }