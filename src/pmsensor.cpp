#include "pmsensor.hpp"

using namespace devices;

bool PMsensor::init() {
  workingstate = true;
  Serial2.begin(defines::PM_SENSOR_BAUD, SERIAL_8N1, -1, -1, false);
  if (!Serial2.available()) {
    workingstate = false;
  }
  return workingstate;
}

bool PMsensor::readPM() {
  if (!Serial2.available()) {
    workingstate = false;
    return workingstate;
  }

  byte pm_buf[10];
  Serial2.readBytes(pm_buf, 10);

  pm25count = pm_buf[3] << 8;
  pm25count |= pm_buf[2];
  pm25 = static_cast<float>(pm25count) / 10.0f;

  pm10count = pm_buf[5] << 8;
  pm10count |= pm_buf[4];
  pm10 = static_cast<float>(pm10count) / 10.f;

  workingstate = true;
  return workingstate;
}

void PMsensor::showDataOnOled() {
  if (!readPM())
    dataString = "PM PROBLEMS! ";
  else
    dataString = String(pm25, 1U) + ";" + String(pm10, 1U) + ";";

  utils::print_oled(dataString.c_str(), 1, 1, true, true);
}

String PMsensor::getDataString() { return dataString; }

bool PMsensor::isWorking() { return workingstate; }