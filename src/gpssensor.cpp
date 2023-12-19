#include "gpssensor.hpp"
using namespace devices;

GPSsensor::GPSsensor() : gps{} {}

bool GPSsensor::init() {
  if (!defines::IS_GPS_PRESENT)
    return false;

  Serial1.begin(defines::GPS_BAUD, SERIAL_8N1, defines::GPS_RX_PIN,
                defines::GPS_TX_PIN);

  unsigned long timer = millis();
  utils::print_oled("WAITING GPS FIX...");

  Serial.println("Waiting GPS data");

  for (;;) {
    feedSomeData(defines::GPS_FEED_TIME);

    if (isDataValid())
      break;

    if ((timer - millis() > defines::GPS_MAX_WAIT_TIME) &&
        gps.charsProcessed() < 10) {
      utils::print_oled("NO GPS DATA RECEIVED");
      Serial.println(F("No GPS data received: check wiring"));
      workingstate = false;
      return workingstate;
    }
  }

  utils::print_oled("GPS GOT 3D FIX");
  Serial.println("GPS got 3D fix");
  workingstate = true;
  return workingstate;
}

bool GPSsensor::isWorking() { return workingstate; }

void GPSsensor::showDataOnOled() {
  String dispdata;
  dataString.clear();

  if (!defines::IS_GPS_PRESENT)
    return;

  dispdata += String(gps.date.day());
  dispdata += "-";
  dispdata += String(gps.date.month());
  dispdata += "-";
  dispdata += String(gps.date.year());
  utils::print_oled(dispdata.c_str(), 1, 1, true, false);

  dispdata.clear();

  dispdata += String(gps.time.hour());
  dispdata += "-";
  dispdata += String(gps.time.minute());
  dispdata += "-";
  dispdata += String(gps.time.second());
  utils::print_oled(dispdata.c_str(), 2, 1, false, false);

  dispdata.clear();

  dispdata += String(gps.location.lat(), 4U) + "N";
  dispdata += ';';
  dispdata += String(gps.location.lng(), 4U) + "E";
  utils::print_oled(dispdata.c_str(), 4, 1, false, false);

  dataString += dispdata + ';';
  dispdata.clear();

  dispdata += "ALT " + String(gps.altitude.meters(), 2U) + "m";
  utils::print_oled(dispdata.c_str(), 5, 1, false, false);

  dataString += dispdata + ';';
  dispdata.clear();
}

String GPSsensor::getDataString() {
  return defines::IS_GPS_PRESENT ? dataString : "0.0;0.0;0.0;";
}

bool GPSsensor::isDataValid() {
  if (gps.location.isValid() && gps.time.isValid() && gps.date.isValid() &&
      gps.altitude.isValid())
    return true;
  return false;
}

void GPSsensor::feedSomeData(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (Serial1.available()) {
      gps.encode(Serial1.read());
      utils::print_oled(getSatelliteString(), 3, 1, false, false);
    }
  } while (millis() - start < ms);
}

String GPSsensor::getFileName() {
  if (!defines::IS_GPS_PRESENT)
    return "";

  TinyGPSDate date = gps.date;
  TinyGPSTime time = gps.time;

  String retstring = "/" + String(date.month()) + "-" + String(date.day()) +
                     "-" + String(date.year()) + "$" + String(time.hour()) +
                     "-" + String(time.minute()) + "-" + String(time.second()) +
                     ".csv";
  return retstring;
}

String GPSsensor::getDateTimeString() {
  if (!defines::IS_GPS_PRESENT)
    return String("00-00-0000;00-00;");

  TinyGPSDate date = gps.date;
  TinyGPSTime time = gps.time;

  String retstring = String(date.month()) + "-" + String(date.day()) + "-" +
                     String(date.year()) + ";" + String(time.hour()) + "-" +
                     String(time.minute()) + "-" + String(time.second()) + ";";
  return retstring;
}

const char *GPSsensor::getSatelliteString() {
  static TinyGPSCustom sats(gps, "GPGSV", 3);
  return strcat("Satellites: ", sats.value());
}