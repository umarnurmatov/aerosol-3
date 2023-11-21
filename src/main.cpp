#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <WiFi.h>

#include "Adafruit_BME280.h"
#include "Adafruit_Sensor.h"
#include "Button.h"
#include "ESP-FTP-Server-Lib.h"
#include "FTPFilesystem.h"
#include "GyverOLED.h"
#include "TinyGPSPlus.h"

///////////////////////////////////
#define SERIAL_BAUD 115200

#define FILE_WRITE_PERIOD 3000

#define GPS_RX_PIN 2
#define GPS_TX_PIN 4
#define GPS_BAUD 9600
#define GPS_MAX_WAIT_TIME 5000
#define GPS_FEED_TIME 1000

#define PM_SENSOR_BAUD 9600

#define BUTTON_1_PIN 15
#define BUTTON_2_PIN 34

#define SEALEVELPRESSURE_HPA 1013.25f

#define IS_BME_PRESENT false
#define IS_GPS_PRESENT false
///////////////////////////////////

// WIFI SETTINGS //////////////////////
const char *ssid = "ESP32_AEROSOL";
const char *password = "xtcf7080";

IPAddress local_ip(192, 168, 0, 1);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
///////////////////////////////////////

// FTP SETTINGS ////////////////
#define FTP_USER "ESP32_FTP"
#define FTP_PASSWORD "xtcf7080"
////////////////////////////////

/////////////////////////////////////////////////
Button btn1{BUTTON_1_PIN, false};
GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;
TinyGPSPlus gps;
Adafruit_BME280 bme;
String filename;
FTPServer ftpServer;
int a = 0;
/////////////////////////////////////////////////

////////////////////////
static bool initSD();
static bool initBME();
static bool initGPS();
static bool initFile();
static void shutDown();

bool readPM(float &pm10, float &pm25);

static void feedGpsSomeData(unsigned long ms);
static bool isGPSDataValid();

void readBME(float &alt, float &humidity, float &temperature, float &pressure);

static void print_oled(const char *msg, int y = 1, int x = 1, bool clear = true,
                       bool autoprint = true);
static void writeFile(const char *path, String &message);

void IRAM_ATTR isr_button();

void setupWiFi();
bool initFTP();
///////////////////////

void setup() {
  Serial.begin(SERIAL_BAUD);
  if (IS_GPS_PRESENT)
    Serial1.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial2.begin(PM_SENSOR_BAUD, SERIAL_8N1, -1, -1, false);

  pinMode(btn1.PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(btn1.PIN), isr_button, FALLING);

  // setupWiFi();
  // if(!initFTP())
  // while(true);

  /////////////////
  oled.init();

  if (IS_BME_PRESENT)
    if (!initBME())
      while (true)
        ;

  if (!initSD())
    while (true)
      ;

  if (IS_GPS_PRESENT)
    if (!initGPS())
      while (true)
        ;

  if (!initFile())
    while (true)
      ;
  ////////////////

  Serial.println("Init success!");
  print_oled("INIT SUCCESS!");
}

void loop() {
  float pm10 = 0.f, pm25 = 0.f;
  float bmeAlttitude = 0.f, humidity = 0.f, pressure = 0.f, temperature = 0.f;
  static unsigned long timet = 0;

  if (btn1.pressed)
    shutDown();

  if (IS_GPS_PRESENT)
    feedGpsSomeData(GPS_FEED_TIME);

  if (readPM(pm10, pm25) && (!IS_GPS_PRESENT || isGPSDataValid())) {
    if (IS_BME_PRESENT)
      readBME(bmeAlttitude, humidity, temperature, pressure);

    // file output
    // "dd-mm-yyyy,hh:mm:ss,PM2.5,PM10,lat,lng,alt,bmeAltitude,humidity,pressure,temperature"
    // the "\n" is added in writeFile()
    if (millis() - timet >= FILE_WRITE_PERIOD) {
      Serial.println(a);
      String strdata;
      if (IS_GPS_PRESENT) {
        strdata += String(gps.date.day());
        strdata += "-";
        strdata += String(gps.date.month());
        strdata += "-";
        strdata += String(gps.date.year());
        strdata += ",";
        strdata += String(gps.time.hour());
        strdata += ":";
        strdata += String(gps.time.minute());
        strdata += ":";
        strdata += String(gps.time.second());
        strdata += ",";
      } else
        strdata += "NONE-NONE-NONE,NONE:NONE:NONE,";
      strdata += String(pm25);
      strdata += ",";
      strdata += String(pm10);
      strdata += ",";
      if (IS_GPS_PRESENT) {
        strdata += String(gps.location.lat(), 6U);
        strdata += ",";
        strdata += String(gps.location.lng(), 6U);
        strdata += ",";
        strdata += String(gps.altitude.meters(), 2U);
        strdata += ",";
      } else
        strdata += "NONE,NONE,NONE,";
      if (IS_BME_PRESENT)
        strdata += String(bmeAlttitude) + "," + String(humidity) + "," +
                   String(pressure) + "," + String(temperature);
      else
        strdata += "NONE,NONE,NONE,NONE";

      String dispdata;
      if (IS_GPS_PRESENT)
        // {
        //     dispdata += "DATE ";
        //     dispdata += String(gps.date.day());
        //     dispdata += "-";
        //     dispdata += String(gps.date.month());
        //     dispdata += "-";
        //     dispdata += String(gps.date.year());

        //     print_oled(dispdata.c_str(), 1, 1, true, false);
        //     dispdata.clear();

        //     dispdata += "TIME ";
        //     dispdata += String(gps.time.hour());
        //     dispdata += ":";
        //     dispdata += String(gps.time.minute());
        //     dispdata += ":";
        //     dispdata += String(gps.time.second());
        //     print_oled(dispdata.c_str(), 2, 1, false, false);
        //     dispdata.clear();

        //     dispdata += "PM ";
        //     dispdata += String(pm25, 1U);
        //     dispdata += ",";
        //     dispdata += String(pm10, 1U);
        //     print_oled(dispdata.c_str(), 3, 1, false, false);
        //     dispdata.clear();

        //     dispdata += String(gps.location.lat(), 4U) + "N";
        //     dispdata += " ";
        //     dispdata += String(gps.location.lng(), 4U) + "E";
        //     print_oled(dispdata.c_str(), 4, 1, false, false);
        //     dispdata.clear();

        //     dispdata += "ALT " + String(gps.altitude.meters(), 2U) + "m";
        //     print_oled(dispdata.c_str(), 5, 1, false, false);
        //     dispdata.clear();
        // }
        else {
          dispdata += "PM ";
          dispdata += String(pm25, 1U);
          dispdata += ",";
          dispdata += String(pm10, 1U);
          print_oled(dispdata.c_str(), 1, 1, false, false);
        }

      Serial.println(strdata);
      writeFile(filename.c_str(), strdata);
      timet = millis();
    }
  }
  // ftpServer.handle();
}

static bool initSD() {
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    print_oled("SD INIT FAIL");
    return false;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return false;
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  print_oled("SD INIT SUCCESS");
  return true;
}

static bool initBME() {
  bool status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    print_oled("NO BME SENSOR");
    return false;
  }
  return true;
}

// static bool initGPS()
// {
//     unsigned long timer = millis();
//     print_oled("WAITING GPS FIX...");
//     Serial.println("Waiting GPS data");
//     while(true)
//     {
//         feedGpsSomeData(GPS_FEED_TIME);
//         if(isGPSDataValid()) break;
//         if ((timer - millis() > GPS_MAX_WAIT_TIME) && gps.charsProcessed() <
//         10)
//         {
//             print_oled("NO GPS DATA RECEIVED");
//             Serial.println(F("No GPS data received: check wiring"));
//             return false;
//         }
//     }
//     print_oled("GPS GOT 3D FIX");
//     Serial.println("GPS got 3D fix");
//     return true;
// }

// <MM-DD-YYYY$hh-mm-ss.csv>
static bool initFile() {
  if (IS_GPS_PRESENT) {
    feedGpsSomeData(GPS_FEED_TIME);
    if (!isGPSDataValid()) {
      print_oled("FAIL INIT FILE: GPS INVALID");
      Serial.println("Fail init file: GPS data not valid");
      return false;
    }
    TinyGPSDate date = gps.date;
    TinyGPSTime time = gps.time;

    filename = "/" + String(date.month()) + "-" + String(date.day()) + "-" +
               String(date.year()) + "$" + String(time.hour()) + "-" +
               String(time.minute()) + "-" + String(time.second()) + ".csv";
  } else {
    int counter = 0;
    do {
      counter++;
      filename = "/" + String(counter) + ".csv";
    } while (SD.exists(filename));
  }

  File file = SD.open(filename, FILE_WRITE, true);
  // see loop()
  file.println(
      "date,time,PM2.5,PM10,lat,lng,alt,bmeAlt,humidity,pressure,temp");
  file.close();

  return true;
}

static void shutDown() {
  Serial.println("Shut down");
  print_oled("SHUT DOWN");
  SD.end();
  Serial.end();
  if (IS_GPS_PRESENT)
    Serial1.end();
  Serial2.end();
  while (true)
    ;
}

// bool readPM(float& pm10, float& pm25)
// {
//     if (Serial2.available())
//     {
//         byte pm_buf[10];
//         Serial2.readBytes(pm_buf, 10);

//         unsigned int pm_25_count = 0;
//         pm_25_count = pm_buf[3] << 8;
//         pm_25_count |= pm_buf[2];
//         pm25 = (float)pm_25_count / 10.0f;

//         unsigned int pm_10_count = 0;
//         pm_10_count = pm_buf[5] << 8;
//         pm_10_count |= pm_buf[4];
//         pm10 = (float)pm_10_count / 10.f;
//         return true;
//     }
//     else
//         return false;
// }

// static void feedGpsSomeData(unsigned long ms)
// {
//     unsigned long start = millis();
//     do
//     {
//         while (Serial1.available())
//             gps.encode(Serial1.read());
//     } while (millis() - start < ms);
// }

// bool isGPSDataValid()
// {
//     if(gps.location.isValid() && gps.time.isValid() && gps.date.isValid() &&
//     gps.altitude.isValid()) return true; return false;
// }

void readBME(float &alt, float &humidity, float &temperature, float &pressure) {
  alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
  humidity = bme.readHumidity();
  temperature = bme.readTemperature();
  pressure = bme.readPressure();
}

// void print_oled(const char* msg, int y, int x, bool clear, bool autoprint)
// {
//     if(clear) oled.clear();
//     oled.autoPrintln(autoprint);
//     oled.setCursor(x, y);
//     oled.print(msg);
//     oled.update();
// }

static void writeFile(const char *path, String &message) {
  Serial.printf("Writing file: %s\n", path);

  File file = SD.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for writing");
    print_oled("FAILED OPEN FILE FOR WRITE", 1, 1, true);
    while (true)
      ;
  }
  if (file.println(message)) {
    Serial.println("File written");
    print_oled("FILE WRITTEN", 6, 1, false);
  } else {
    Serial.println("Write failed");
    print_oled("WRITE FAIL", 1, 1, true);
    file.close();
    while (true)
      ;
  }
  file.close();
}

void IRAM_ATTR isr_button() {
  a = 1;
  unsigned long time = millis();
  if (time - btn1.last_debounce_time > 250) {
    btn1.pressed = true;
    btn1.last_debounce_time = time;
  }
}

void setupWiFi() {

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(ssid, password);

  Serial.print("WiFi AP created: ");
  Serial.println(WiFi.softAPIP());
}

bool initFTP() {
  ftpServer.addUser(FTP_USER, FTP_PASSWORD);
  ftpServer.addFilesystem("SD", &SD);
  if (!ftpServer.begin()) {
    Serial.println("FTP server creation error");
    return false;
  }
  Serial.println("FTP Server created");
  return true;
}
