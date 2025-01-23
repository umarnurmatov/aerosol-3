#include "pmsensor.hpp"

using namespace devices;
#define START_1 0x42
#define START_2 0x4d

bool PMsensor::init() {
    Serial2.begin(defines::PM_SENSOR_BAUD, SERIAL_8N1, -1, -1, false);
  workingstate = true;

  if (!Serial2.available()) {
    workingstate = false;
  }

  return workingstate;
}

bool PMsensor::readPM() {
  byte pm_buf[30];

  int32_t byte_count = 0;
  bool got_value = false;

  int start = millis();
  int end = millis();

  while(!got_value && end - start < 1000){
    while(!Serial2.available()){}
    uint8_t c = 0;
    Serial2.readBytes(&c, 1);

    if(c == START_1 && byte_count == 0){
      ++byte_count;
    }  
    if(c == START_2 && byte_count == 1){
      ++byte_count;
    }

    if(byte_count == 2) {
      for (size_t i = 0; i < 30; ++i){

        while(!Serial2.available()){}

        uint8_t d = 0;
        Serial2.readBytes(&d, 1);
        pm_buf[i] = d;
      }
          got_value = true;
    }
    end = millis();

  } 


  pm25 = static_cast<int16_t>(static_cast<int>(pm_buf[6]) << 8 | static_cast<int>(pm_buf[7]));
  pm10 = static_cast<int16_t>(static_cast<int>(pm_buf[4]) << 8 | static_cast<int>(pm_buf[5]));

  workingstate = true;
  return workingstate;
}

void PMsensor::showDataOnOled() {
  utils::print_oled(dataString.c_str(), 1, 1, true, true);
}

String PMsensor::getDataString() {
  if (!readPM()){
    dataString = "PM PROBLEMS! ";
  }
  else
    dataString = String(pm25, 1U) + ";" + String(pm10, 1U) + ";";

  return dataString;
}

bool PMsensor::isWorking() { return workingstate; }