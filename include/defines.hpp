#pragma once

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

	constexpr inline const char *GUI_HUB_NET_NAME = "Aerosol_NET";
	constexpr inline const char *GUI_HUB_DEVICE_NAME = "Aerosol_DEVICE";

	constexpr inline const char *ESP_LOG_TAG = "aerosol_debug";

	constexpr inline const char *SOFTAP_SSID = "Aerosol_AP";
	constexpr inline const char *SOFTAP_PASSWORD = "123456789";

} // namespace defines
