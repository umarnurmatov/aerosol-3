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
protected:
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
      return m_next_state;
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
    return *m_val ? String("[*]") : String("[ ]");
  }
private:
  bool *m_val;
};

struct Field_Bool : public Field
{
  Field_Bool(String name, std::function<bool()> func)
    : Field { name }, m_func{ func }
  {
  }
  app_state_t process_state(modules::Encoder& enc, app_state_t& state) override
  {
    return state;
  }
  String value_string() const override
  {
    return m_func() ? String("[*]") : String("[ ]");
  }
private:
  std::function<bool()> m_func;
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

struct Field_Func_Bool : public Field
{
public:
  Field_Func_Bool(String name, std::function<bool()> func, app_state_t next_state)
    : Field { name }, m_next_state { next_state }, m_func { func }
  {
  }
  app_state_t process_state(modules::Encoder& enc, app_state_t& state) override
  {
    m_func();
    return m_next_state;
  }
  String value_string() const override
  {
    return String();
  } 
private:
  app_state_t m_next_state;
  std::function<bool()> m_func;
};

#include <vector>
typedef std::vector<Field*> page_t;
page_t page_main;
page_t page_data_collecting;
page_t page_webinterface;
page_t page_settings;
page_t page_settings_gps;
page_t page_settings_gps_init;
page_t page_settings_sd;
page_t page_settings_sd_init;
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

  // Name len no more than 16 !
  // Otherwise nonsense will be printed
  page_main.push_back(new Field_Transition("Start", PAGE_DATA_COLLECTING));
  page_main.push_back(new Field_Transition("Web Interface", PAGE_WEBINTERFACE));
  page_main.push_back(new Field_Transition("Settings", PAGE_SETTINGS));

  page_data_collecting.push_back(new Field_Transition("Back", PAGE_MAIN));

  page_webinterface.push_back(new Field_Transition("Back", PAGE_MAIN));

  page_settings.push_back(new Field_Transition("GPS", PAGE_SETTINGS_GPS));
  page_settings.push_back(new Field_Transition("SD", PAGE_SETTINGS_SD));
  page_settings.push_back(new Field_Transition("Back", PAGE_MAIN));

  page_settings_gps.push_back(new Field_Bool("GPS State", [&]() -> bool { return gps.isWorking(); }));
  page_settings_gps.push_back(new Field_Transition("Init GPS", PAGE_SETTINGS_GPS_INIT));
  page_settings_gps.push_back(new Field_Transition("Back", PAGE_SETTINGS));

  page_settings_gps_init.push_back(new Field_Func_Bool("Wait...", [&]() -> bool { return gps.init(); }, PAGE_SETTINGS_GPS));

  page_settings_sd.push_back(new Field_Bool("SD State", [&]() -> bool { return sd.isWorking(); }));
  page_settings_sd.push_back(new Field_Transition("Init SD", PAGE_SETTINGS_SD_INIT));
  page_settings_sd.push_back(new Field_Transition("Back", PAGE_SETTINGS));

  page_settings_sd_init.push_back(new Field_Func_Bool("Wait...", [&]() -> bool { return sd.init(); }, PAGE_SETTINGS_SD));


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

  static page_t *current_page = &page_main;

  if(timer.isReady())
  {

  enc.tick();
  if(enc.isClick()) ESP_LOGI(defines::ESP_LOG_TAG, "Encoder btn clicked");
  if(enc.isRight()) ESP_LOGI(defines::ESP_LOG_TAG, "Encoder turned right");
  if(enc.isLeft()) ESP_LOGI(defines::ESP_LOG_TAG, "Encoder turned left");

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
    case PAGE_SETTINGS_GPS_INIT:
    current_page = &page_settings_gps_init;
    break;
    case PAGE_SETTINGS_SD_INIT:
    current_page = &page_settings_sd_init;
    break;
    default:
    ESP_LOGD(defines::ESP_LOG_TAG, "State %d not recognized", state);
    break;
  }

  // MENU NAVIGATION //
  if (enc.isLeft()) field = utils::clamp(field + 1, 0, current_page->size());
  else if (enc.isRight()) field = utils::clamp(field - 1, 0, current_page->size());

  // DISPLAY BUFFER UPDATING //
  #define FONT_WIDTH 6
  #define FONT_HEIGHT 8
  #define OLED_WIDTH 128
  #define OLED_HEIGHT 8
  #define OLED_X OLED_WIDTH - 1
  #define OLED_Y OLED_HEIGHT - 1
  #define BITMAP_BAT_H 6
  #define BITMAP_BAT_W 8
  #define OFFSET 2

  devices::oled.setCursor(0, 0);
  devices::oled.print("AEROSOL");
  devices::oled.drawBitmap(OLED_X - BITMAP_BAT_W - 1, 0, bitmap_bat_high, BITMAP_BAT_W, BITMAP_BAT_H);
  devices::oled.line(0, FONT_HEIGHT * 1.5, OLED_X, FONT_HEIGHT * 1.5);

  String BUILD_NAME = "Build v1.0";
  devices::oled.setCursor(OLED_X - BUILD_NAME.length() * FONT_WIDTH, OLED_Y);
  devices::oled.print(BUILD_NAME);

  for (int i = 0; i < current_page->size(); ++i) {
    devices::oled.setCursor(0, i + OFFSET);
    auto& c_field = current_page->at(i);
    devices::oled.printf("  %s%*s", c_field->name(), OLED_WIDTH / FONT_WIDTH - c_field->name().length() - 2, c_field->value_string());
  }
  devices::oled.setCursor(0, field + OFFSET);
  devices::oled.print(">");

  // UPDATE DISPLAY FROM BUFFER //
  devices::oled.update();

  // STATE SWITCH //
  next_state = current_page->at(field)->process_state(enc, state);

  // CLEAR DISPLAY IF NECCESARY //
  if (next_state != state) {
    devices::oled.clear();
    field = 0;
  }

  // UPDATING STATE //
  state = next_state;

  }
}
