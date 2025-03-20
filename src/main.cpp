#include "devices.hpp"
#include "bmesensor.hpp"
#include "gpssensor.hpp"
#include "pmsensor.hpp"
#include "sdmodule.hpp"
#include "gyverhub.hpp"
#include "enc.hpp"

static modules::BMEsensor bme;
static modules::GPSsensor gps;
static modules::PMsensor pm;
static modules::SDmodule sd;
static modules::Encoder enc;
static modules::GUI_GuverHub gui_gyverhub;

enum app_state_t
{
  INIT,
  PAGE_MAIN,
  PAGE_WEBINTERFACE,
  PAGE_DATA_COLLECTING,
  PAGE_SETTINGS,
  PAGE_SETTINGS_GPS,
  PAGE_SETTINGS_SD,
  PAGE_SETTINGS_GPS_INIT,
  PAGE_SETTINGS_SD_INIT
};
static app_state_t state, next_state;

#include <vector>
std::vector<String> page_main_items = 
{
  "Start",
  "Web interface",
  "Settings"
};

std::vector<String> page_data_collecting_items = 
{
  "Back"
};

std::vector<String> page_webinterface_items = 
{
  "Back"
};

std::vector<String> page_settings_items = 
{
  "GPS",
  "SD card",
  "Back"
};

std::vector<String> page_settings_gps_items = 
{
  "Initialize",
  "Back"
};

std::vector<String> page_settings_sd_items = 
{
  "Initialize",
  "Back"
};

uint8_t bitmap_bat_low[] = {
    0x7e, 0x7e, 0x7e, 0x42, 0x42, 0x42, 0x7e, 0x3c
};

uint8_t bitmap_bat_mid[] = {
    0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x42, 0x7e, 0x3c
};

uint8_t bitmap_bat_high[] = {
    0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x3c
};

int16_t line;

#include <GyverTimer.h>
GTimer timer(MS);

void setup()
{
  Serial.begin(115200);
  //////////// INIT STATE MACHINE ////////////
  state = PAGE_MAIN;
  line = 0;

  //////////// INIT DISPLAY AND ENC ////////////
  enc.init();
  devices::oled.init();
  devices::oled.setScale(3);
  devices::oled.setCursor(0, 4);
  devices::oled.println("AEROSOL");
  devices::oled.update();
  devices::oled.setScale(1);

  //////////// INIT OTHER MODULES ////////////
  if (defines::INIT_SD_AT_START) {
    sd.init();
    sd.initFile(gps.getFileName());
  }
  if (defines::INIT_PM_AT_START) pm.init();
  if (defines::INIT_GPS_AT_START) gps.init();
  if (defines::INIT_BME_AT_START) bme.init();

  timer.setInterval(defines::FRAMETIME_MS);

  //////////// CLEAR DISPLAY ////////////
  devices::oled.clear();
  devices::oled.update();
}

void loop()
{
  // static unsigned long timet = 0;

  // defines::oled.clear();

  // if (millis() - timet > defines::FILE_WRITE_PEERIOD)
  // {
  //   sd.writeFile(gps.getDateTimeString() + pm.getDataString() +
  //                gps.getDataString() + bme.getDataString() + '\n');
  //   timet = millis();
  // }
  // pm.getDataString();
  // pm.showDataOnOled();

  // devices::hub.tick();

  if(timer.isReady()) {

  enc.tick();
  if(enc.isClick()) ESP_LOGI(defines::ESP_LOG_TAG, "Encoder btn clicked");
  if(enc.isRight()) ESP_LOGI(defines::ESP_LOG_TAG, "Encoder turned right");
  if(enc.isLeft()) ESP_LOGI(defines::ESP_LOG_TAG, "Encoder turned left");

  next_state = state;

  // STATE SWITCH //
  switch (state) {
    case PAGE_MAIN:
    if (enc.isClick()) {
      switch (line) {
        case 0: next_state = PAGE_DATA_COLLECTING; break;
        case 1: next_state = PAGE_WEBINTERFACE; break;
        case 2: next_state = PAGE_SETTINGS; break;
      }
    }
    break;
    case PAGE_DATA_COLLECTING:
    if (enc.isClick()) {
      switch (line) {
        case 0: next_state = PAGE_MAIN; break;
      }
    }
    break;
    case PAGE_WEBINTERFACE:
    if (enc.isClick()) {
      switch (line) {
        case 0: next_state = PAGE_MAIN; break;
      }
    }
    break;
    case PAGE_SETTINGS:
    if (enc.isClick()) {
      switch (line) {
        case 0: next_state = PAGE_SETTINGS_GPS; break;
        case 1: next_state = PAGE_SETTINGS_SD; break;
        case 2: next_state = PAGE_MAIN; break;
      }
    }
    break;
    case PAGE_SETTINGS_GPS:
    if (enc.isClick()) {
      switch (line) {
        case 0: next_state = PAGE_SETTINGS_GPS_INIT; break;
        case 1: next_state = PAGE_SETTINGS; break;
      }
    }
    break;
    case PAGE_SETTINGS_SD:
    if (enc.isClick()) {
      switch (line) {
        case 0: next_state = PAGE_SETTINGS_SD_INIT; break;
        case 1: next_state = PAGE_SETTINGS; break;
      }
    }
    break;
  }

  // MENU NAVIGATION //
  switch (state) {
    case PAGE_MAIN:
      if (enc.isLeft()) line = utils::clamp(line + 1, 0, page_main_items.size());
      else if (enc.isRight()) line = utils::clamp(line - 1, 0, page_main_items.size());
      break;
    case PAGE_SETTINGS:
      if (enc.isLeft()) line = utils::clamp(line + 1, 0, page_settings_items.size());
      else if (enc.isRight()) line = utils::clamp(line - 1, 0, page_settings_items.size());
      break;
    case PAGE_SETTINGS_GPS:
      if (enc.isLeft()) line = utils::clamp(line + 1, 0, page_settings_gps_items.size());
      else if (enc.isRight()) line = utils::clamp(line - 1, 0, page_settings_gps_items.size());
      break;
    case PAGE_SETTINGS_SD:
      if (enc.isLeft()) line = utils::clamp(line + 1, 0, page_settings_sd_items.size());
      else if (enc.isRight()) line = utils::clamp(line - 1, 0, page_settings_sd_items.size());
      break;
  }

  // DISPLAY BUFFER UPDATING //
  #define FONT_WIDTH 6
  #define FONT_HEIGHT 8
  #define OLED_Y 7
  #define OLED_X 127
  #define BITMAP_BAT_H 6
  #define BITMAP_BAT_W 8

  devices::oled.setCursor(0, 0);
  devices::oled.print("AEROSOL");
  devices::oled.drawBitmap(OLED_X - BITMAP_BAT_W - 1, 0, bitmap_bat_high, BITMAP_BAT_W, BITMAP_BAT_H);
  devices::oled.line(0, FONT_HEIGHT * 1.5, OLED_X, FONT_HEIGHT * 1.5);

  String BUILD_NAME = "Build v1.0";
  devices::oled.setCursor(OLED_X - BUILD_NAME.length() * FONT_WIDTH, OLED_Y);
  devices::oled.print(BUILD_NAME);

  #define OFFSET 2

  switch (state) {
    case PAGE_MAIN:
      for (int i = 0; i < page_main_items.size(); ++i) {
        devices::oled.setCursor(0, i + OFFSET);
        devices::oled.printf("  %s", page_main_items[i]);
      }
      devices::oled.setCursor(0, line + OFFSET);
      devices::oled.print(">");
      break;
    case PAGE_DATA_COLLECTING:
      for (int i = 0; i < page_data_collecting_items.size(); ++i) {
        devices::oled.setCursor(0, i + OFFSET);
        devices::oled.printf("  %s", page_data_collecting_items[i]);
      }
      devices::oled.setCursor(0, line + OFFSET);
      devices::oled.print(">");
      devices::oled.setCursor(0, page_data_collecting_items.size() + OFFSET + 1);
      devices::oled.print("Collecting data");
      break;
    case PAGE_WEBINTERFACE:
      for (int i = 0; i < page_webinterface_items.size(); ++i) {
        devices::oled.setCursor(0, i + OFFSET);
        devices::oled.printf("  %s", page_webinterface_items[i]);
      }
      devices::oled.setCursor(0, line + OFFSET);
      devices::oled.print(">");
      devices::oled.setCursor(0, page_webinterface_items.size() + OFFSET + 1);
      devices::oled.print("Webinterface");
      break;
    case PAGE_SETTINGS:
      for (int i = 0; i < page_settings_items.size(); ++i) {
        devices::oled.setCursor(0, i + OFFSET);
        devices::oled.printf("  %s", page_settings_items[i]);
      }
      devices::oled.setCursor(0, line + OFFSET);
      devices::oled.print(">");
      break;
    case PAGE_SETTINGS_GPS:
      for (int i = 0; i < page_settings_gps_items.size(); ++i) {
        devices::oled.setCursor(0, i + OFFSET);
        devices::oled.printf("  %s", page_settings_gps_items[i]);
      }
      devices::oled.setCursor(0, line + OFFSET);
      devices::oled.print(">");
      break;
     case PAGE_SETTINGS_SD:
      for (int i = 0; i < page_settings_sd_items.size(); ++i) {
        devices::oled.setCursor(0, i + OFFSET);
        devices::oled.printf("  %s", page_settings_sd_items[i]);
      }
      devices::oled.setCursor(0, line + OFFSET);
      devices::oled.print(">");
      break;
  }

  devices::oled.update();
  if (next_state != state) {
    devices::oled.clear();
    line = 0;
  }
  state = next_state;
  }
}
