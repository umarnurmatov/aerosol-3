#pragma once

namespace defines
{
	constexpr inline int SERIAL_BAUDRATE = 115200;
	constexpr inline int PM_SENSOR_SERIAL_BAUDRATE = 9600;
	constexpr inline int GPS_SERIAL_BAUDRATE = 9600;

	constexpr inline short GPS_FEED_TIME = 1000;
	constexpr inline int GPS_MAX_WAIT_TIME = 1200'000;
	constexpr inline short FILE_WRITE_PEERIOD = 1500;
	constexpr inline short GPS_RX_PIN = 2;
	constexpr inline short GPS_TX_PIN = 4;
	constexpr inline short ENCODER_DT_PIN = 39;
	constexpr inline short ENCODER_CLK_PIN = 34;
	constexpr inline short ENCODER_SW_PIN = 35;
	constexpr inline short ENCODER_STEP_CNT = 5;

	constexpr inline float SEALVLPRESSURE_HPA = 1013.25f;

	constexpr inline bool INIT_PM_AT_START = false;
	constexpr inline bool INIT_SD_AT_START = false;
	constexpr inline bool INIT_BME_AT_START = false;
	constexpr inline bool INIT_GPS_AT_START = false;

	constexpr inline bool FRAMETIME_MS = 100;

	constexpr inline const char *GUI_HUB_NET_NAME = "Aerosol_NET";
	constexpr inline const char *GUI_HUB_DEVICE_NAME = "Aerosol_DEVICE";

	constexpr inline const char *ESP_LOG_TAG = "aerosol_debug";

	constexpr inline const char *SOFTAP_SSID = "Aerosol_AP";
	constexpr inline const char *SOFTAP_PASSWORD = "123456789";

} // namespace defines
