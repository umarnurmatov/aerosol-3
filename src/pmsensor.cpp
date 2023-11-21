#include "pmsensor.hpp"

using namespace sensors;

PMsensor::PMsensor() {
  Serial2.begin(defines::PM_SENSOR_BAUD, SERIAL_8N1, -1, -1, false);
}

bool PMsensor::readPM() {
  if (!Serial2.available())
    return false;

  byte pm_buf[10];
  Serial2.readBytes(pm_buf, 10);

  pm25count = pm_buf[3] << 8;
  pm25count |= pm_buf[2];
  pm25 = static_cast<float>(pm25count) / 10.0f;

  pm10count = pm_buf[5] << 8;
  pm10count |= pm_buf[4];
  pm10 = static_cast<float>(pm10count) / 10.f;

  return true;
}

String PMsensor::getDataString() {
  return String(pm25) + "," + String(pm10) + ",";
}
