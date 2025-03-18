#pragma once

#include <GyverOLED.h>
#include <TinyGPS++.h>
#include <Adafruit_BME280.h>
#include <GyverHub.h>
#include "defines.hpp"

namespace devices
{
    static GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;
    static Adafruit_BME280 bme;
    static TinyGPSPlus gps;
    static GyverHub hub { defines::GUI_HUB_NET_NAME, defines::GUI_HUB_DEVICE_NAME };
}