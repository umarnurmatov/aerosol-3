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
  VOID,
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

class Field
{
public:
  Field(String name) : m_name { name } {}
  virtual app_state_t process_state(modules::Encoder& enc, app_state_t& state) = 0;
  virtual String value_string() const= 0;
  virtual String name() { return m_name; }
private:
  String m_name;
};

struct Field_Transition : public Field
{
  Field_Transition(String name, app_state_t next_state)
    : Field { name }, m_next_state { next_state }
  {
  }
  app_state_t process_state(modules::Encoder& enc, app_state_t& state) override
  {
    if(enc.isClick()) {
      return next_state;
    }
    return state;
  }
  String value_string() const override
  {
    return String();
  } 
private:
  app_state_t m_next_state;
};

struct Field_Bool_Mod : public Field
{
  Field_Bool_Mod(String name, bool *val)
    : Field { name }, m_val{ val }
  {
  }
  app_state_t process_state(modules::Encoder& enc, app_state_t& state) override
  {
    if(enc.isClick() && (enc.isRight() || enc.isLeft())) {
      *m_val = !*m_val;
    }
    return state;
  }
  String value_string() const override
  {
    return String(*m_val);
  }
private:
  bool *m_val;
};

struct Field_Val : public Field
{
public:
  Field_Val(String name, int32_t *val)
    : Field { name }, m_val{ val }
  {
  }
  app_state_t process_state(modules::Encoder& enc, app_state_t& state) override
  {
    return state;
  }
  String value_string() const override
  {
    return String(*m_val);
  } 
private:
  int32_t *m_val;
};

#include <vector>
typedef std::vector<Field*> page_t;
page_t page_main;
page_t page_data_collecting;
page_t page_webinterface;
page_t page_settings;
page_t page_settings_gps;
page_t page_settings_sd;
page_t *current_page;
int32_t field;

bool gps_state = false;

uint8_t bitmap_bat_low[] = {
    0x7e, 0x7e, 0x7e, 0x42, 0x42, 0x42, 0x7e, 0x3c
};

uint8_t bitmap_bat_mid[] = {
    0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x42, 0x7e, 0x3c
};

uint8_t bitmap_bat_high[] = {
    0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x3c
};

#include <GyverTimer.h>
GTimer timer(MS);

void setup()
{
  Serial.begin(defines::SERIAL_BAUDRATE);
  //////////// INIT STATE MACHINE ////////////
  state = PAGE_MAIN;
  field = 0;

  page_main.push_back(new Field_Transition("Start", PAGE_DATA_COLLECTING));
  page_main.push_back(new Field_Transition("Web Interface", PAGE_WEBINTERFACE));
  page_main.push_back(new Field_Transition("Settings", PAGE_SETTINGS));

  page_data_collecting.push_back(new Field_Transition("Back", PAGE_MAIN));

  page_webinterface.push_back(new Field_Transition("Back", PAGE_MAIN));

  page_settings.push_back(new Field_Transition("GPS", PAGE_SETTINGS_GPS));
  page_settings.push_back(new Field_Transition("SD", PAGE_SETTINGS_SD));
  page_settings.push_back(new Field_Transition("Back", PAGE_MAIN));

  page_settings_gps.push_back(new Field_Transition("Initialize", PAGE_SETTINGS_GPS_INIT));
  page_settings_gps.push_back(new Field_Bool_Mod("GPS State: ", &gps_state));
  page_settings_gps.push_back(new Field_Transition("Back", PAGE_SETTINGS));

  page_settings_sd.push_back(new Field_Transition("Initialize", PAGE_SETTINGS_SD_INIT));
  page_settings_sd.push_back(new Field_Transition("Back", PAGE_SETTINGS));

  current_page = &page_main;

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

  if(timer.isReady())
  {

  enc.tick();
  if(enc.isClick()) ESP_LOGI(defines::ESP_LOG_TAG, "Encoder btn clicked");
  if(enc.isRight()) ESP_LOGI(defines::ESP_LOG_TAG, "Encoder turned right");
  if(enc.isLeft()) ESP_LOGI(defines::ESP_LOG_TAG, "Encoder turned left");


  // STATE SWITCH //
  next_state = current_page->at(field)->process_state(enc, state);

  switch (state) {
    case PAGE_MAIN:
    current_page = &page_main;
    break;
    case PAGE_DATA_COLLECTING:
    current_page = &page_data_collecting;
    break;
    case PAGE_WEBINTERFACE:
    current_page = &page_webinterface;
    break;
    case PAGE_SETTINGS:
    current_page = &page_settings;
    break;
    case PAGE_SETTINGS_GPS:
    current_page = &page_settings_gps;
    break;
    case PAGE_SETTINGS_SD:
    current_page = &page_settings_sd;
    break;
  }

  // MENU NAVIGATION //
  if (enc.isLeft()) field = utils::clamp(field + 1, 0, current_page->size());
  else if (enc.isRight()) field = utils::clamp(field - 1, 0, current_page->size());

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
      for (int i = 0; i < page_main.size(); ++i) {
        devices::oled.setCursor(0, i + OFFSET);
        devices::oled.printf("  %s", page_main[i]->name());
      }
      devices::oled.setCursor(0, field + OFFSET);
      devices::oled.print(">");
      break;
    case PAGE_DATA_COLLECTING:
      for (int i = 0; i < page_data_collecting.size(); ++i) {
        devices::oled.setCursor(0, i + OFFSET);
        devices::oled.printf("  %s", page_data_collecting[i]->name());
      }
      devices::oled.setCursor(0, field + OFFSET);
      devices::oled.print(">");
      devices::oled.setCursor(0, page_data_collecting.size() + OFFSET + 1);
      devices::oled.print("Collecting data");
      break;
    case PAGE_WEBINTERFACE:
      for (int i = 0; i < page_webinterface.size(); ++i) {
        devices::oled.setCursor(0, i + OFFSET);
        devices::oled.printf("  %s", page_webinterface[i]->name());
      }
      devices::oled.setCursor(0, field + OFFSET);
      devices::oled.print(">");
      devices::oled.setCursor(0, page_webinterface.size() + OFFSET + 1);
      devices::oled.print("Webinterface");
      break;
    case PAGE_SETTINGS:
      for (int i = 0; i < page_settings.size(); ++i) {
        devices::oled.setCursor(0, i + OFFSET);
        devices::oled.printf("  %s", page_settings[i]->name());
      }
      devices::oled.setCursor(0, field + OFFSET);
      devices::oled.print(">");
      break;
    case PAGE_SETTINGS_GPS:
      for (int i = 0; i < page_settings_gps.size(); ++i) {
        devices::oled.setCursor(0, i + OFFSET);
        devices::oled.printf("  %s %s", page_settings_gps[i]->name(), page_settings_gps[i]->value_string());
      }
      devices::oled.setCursor(0, field + OFFSET);
      devices::oled.print(">");
      break;
     case PAGE_SETTINGS_SD:
      for (int i = 0; i < page_settings_sd.size(); ++i) {
        devices::oled.setCursor(0, i + OFFSET);
        devices::oled.printf("  %s", page_settings_sd[i]->name());
      }
      devices::oled.setCursor(0, field + OFFSET);
      devices::oled.print(">");
      break;
  }

  // UPDATING DISPLAY FROM BUFFER //
  devices::oled.update();
  if (next_state != state) {
    devices::oled.clear();
    field = 0;
  }
  // UPDATEING STATE //
  state = next_state;

  }
}
