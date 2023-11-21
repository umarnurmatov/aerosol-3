#pragma once

#include <FS.h>
#include <GyverOLED.h>

namespace defines {
constexpr inline short SERIAL_BAUD = 115200;
constexpr inline short PM_SENSOR_BAUD = 9600;
constexpr inline short GPS_BAUD = 9600;

constexpr inline short GPS_FEED_TIME = 1000;
constexpr inline short GPS_MAX_WAIT_TIME = 5000;
constexpr inline short FILE_WRITE_PEERIOD = 3000;
constexpr inline short GPS_RX_PIN = 2;
constexpr inline short GPS_TX_PIN = 4;
constexpr inline short BUTTON_1_PIN = 15;
constexpr inline short BUTTON_2_PIN = 34;

constexpr inline float SEALVLPRESSURE_HPA = 1013.25f;

constexpr inline bool IS_BME_PRESENT = false;
constexpr inline bool IS_GPS_PRESENT = false;

constexpr inline char *wifiSSID = "ESP32_AEROSOL";
constexpr inline char *wifiPasswd = "xtcf7080";

constexpr inline char *FTP_USER = "ESP32_FTP";
constexpr inline char *FTP_PASSWD = "xtcf7080";

const inline IPAddress ipLocal{192, 168, 0, 1};
const inline IPAddress ipGateway{192, 168, 0, 1};
const inline IPAddress ipSubnet{255, 255, 255, 0};

GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;
} // namespace defines