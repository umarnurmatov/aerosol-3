#pragma once

#include <FS.h>
#include <GyverOLED.h>

namespace defines
{
	constexpr inline int SERIAL_BAUD = 115200;
	constexpr inline int PM_SENSOR_BAUD = 9600;
	constexpr inline int GPS_BAUD = 9600;

	constexpr inline short GPS_FEED_TIME = 1000;
	constexpr inline int GPS_MAX_WAIT_TIME = 1200'000;
	constexpr inline short FILE_WRITE_PEERIOD = 1500;
	constexpr inline short GPS_RX_PIN = 2;
	constexpr inline short GPS_TX_PIN = 4;
	constexpr inline short BUTTON_1_PIN = 15;
	constexpr inline short BUTTON_2_PIN = 34;

	constexpr inline float SEALVLPRESSURE_HPA = 1013.25f;

	constexpr inline bool IS_BME_PRESENT = false;
	constexpr inline bool IS_GPS_PRESENT = true;

	static GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;
} // namespace defines