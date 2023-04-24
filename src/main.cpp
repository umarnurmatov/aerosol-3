#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <RTClib.h>
#include "GyverOLED.h"
#include <map>
#include <vector>
#include "Button.h"

#define WRITE_PERIOD 3000
#define RTC_SETUP_WAIT_TIME 10000

RTC_DS3231 rtc;
Button btn1 {15, false};
GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;
String filename;

bool initSD();
bool initRTC();
void initFile();
void shutDown();

bool get_pm(float& pm_10, float& pm_25);

/// @brief DD-MM-YYYY,hh:mm:ss"
/// @return current datetime of format ^
String get_datetime();

void print_oled(const char* msg, int y = 1, int x = 1, bool clear = true, bool autoprint = true);
void writeFile(const char* path, String& message);

void IRAM_ATTR isr_button();

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, -1, -1, false);

    oled.init();
    if (!initSD() || !initRTC())
        while (true);

    pinMode(btn1.PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(btn1.PIN), isr_button, RISING);

    initFile();

    print_oled("INIT SUCCESS!");
}

void loop() {
    float pm_10 = 0.f, pm_25 = 0.f;
    static unsigned long timet = 0;
    if(btn1.pressed)
    {
        shutDown();
    }
    if (get_pm(pm_10, pm_25)) {
        Serial.printf("PM2.5 = %.4f; PM10 = %.4f\n", pm_25, pm_10);
        //dd-mm-yyyy,hh:mm:ss,PM2.5,PM10
        if (millis() - timet >= WRITE_PERIOD) {
            String strdata = get_datetime();
            strdata += ",";
            strdata += String(pm_25);
            strdata += ",";
            strdata += String(pm_10);
            Serial.println(strdata);
            print_oled(strdata.c_str());
            writeFile(filename.c_str(), strdata);
            timet = millis();
        }
    }
}

bool initSD() 
{
    if (!SD.begin()) 
    {
        Serial.println("Card Mount Failed");
        print_oled("SD INIT FAIL");
        return false;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) 
    {
        Serial.println("No SD card attached");
        return false;
    }

    Serial.print("SD Card Type: ");
    switch(cardType)
    {
        case CARD_MMC:
        Serial.println("MMC");
        break;
        case CARD_SD:
        Serial.println("SD");
        break;
        case CARD_SDHC:
        Serial.println("SDHC");
        break;
        default:
        Serial.println("UNKNOWN");
        break;
    }


    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    print_oled("SD INIT SUCCESS");
    return true;
}

bool initRTC() 
{
    if (!rtc.begin()) {
        Serial.println("RTC start fail");
        print_oled("RTC INIT FAIL");
        return false;
    }

    Serial.println("RTC started");
    Serial.printf("RTC lost power: %s\n", rtc.lostPower() ? "yes" : "no");

    String choice = "N";

    unsigned long timer = millis();
    Serial.print("Wanna setup date&time? Y/N: ");
    while(!Serial.available() && (millis() - timer < RTC_SETUP_WAIT_TIME)) {}
    if(Serial.available()) choice = Serial.readStringUntil('\n');
    Serial.println(choice);

    if (choice == "Y") {
        Serial.println("rtc lost power, please setup date&time");
        std::map<String, uint16_t> m;
        static String s[] = {
            "day",
            "month",
            "year",
            "hour",
            "minute",
            "second",
        };
        for (auto& i : s) {
            Serial.printf("Enter %s:", i.c_str());
            while (!Serial.available());
            String inp = Serial.readStringUntil('\n');
            m[i] = inp.toInt();
            Serial.println(inp);
        }
        DateTime dt(m["year"], m["month"], m["day"], m["hour"], m["minute"], m["second"]);
        rtc.adjust(dt);
        Serial.printf("Adjusted time is: %s\n", get_datetime().c_str());
    }
    else
        Serial.println("skipped");

    print_oled("RTC INIT SUCCESS");
    return true;
}

void initFile()
{
    DateTime now = rtc.now();
    static char buf[] = "MM-DD-YYYY$hh-mm-ss";
    filename = String("/") + String(now.toString(buf)) + String(".txt");
    if(!SD.exists(filename))
    {
        File file = SD.open(filename, FILE_WRITE, true);
        file.println("date,time,PM2.5,PM10");
        file.close();
    }
}

void shutDown()
{
    Serial.println("Shut down");
    print_oled("SHUT DOWN");
    SD.end();
    Serial2.end();
    while(true);
}

bool get_pm(float& pm_10, float& pm_25) {
    if (Serial2.available()) 
    {
        byte pm_buf[10];
        Serial2.readBytes(pm_buf, 10);

        unsigned int pm_25_count = 0;
        pm_25_count = pm_buf[3] << 8;
        pm_25_count |= pm_buf[2];
        pm_25 = (float)pm_25_count / 10.0f;

        unsigned int pm_10_count = 0;
        pm_10_count = pm_buf[5] << 8;
        pm_10_count |= pm_buf[4];
        pm_10 = (float)pm_10_count / 10.f;
        return true;
    } 
    else
        return false;
}

String get_datetime() {
    DateTime now = rtc.now();
    char buf[] = "DD-MM-YYYY,hh:mm:ss";
    return String(now.toString(buf));
}

void print_oled(const char* msg, int y, int x, bool clear, bool autoprint)
{
    if(clear) oled.clear();
    oled.autoPrintln(autoprint);
    oled.setCursor(x, y);
    oled.print(msg);
    oled.update();
}

void writeFile(const char* path, String& message) {
    Serial.printf("Writing file: %s\n", path);

    File file = SD.open(path, FILE_APPEND);
    if (!file) 
    {
        Serial.println("Failed to open file for writing");
        print_oled("FAILED OPEN FILE FOR WRITE", 4, 1, false);
        while(true);
    }
    if (file.println(message)) 
    {
        Serial.println("File written");
        print_oled("FILE WRITTEN", 4, 1, false);
    } 
    else 
    {
        Serial.println("Write failed");
        print_oled("WRITE FAIL", 4, 1, false);
        file.close();
        while(true);
    }
    file.close();
}

void IRAM_ATTR isr_button()
{
    unsigned long time = millis();
    if (time - btn1.last_debounce_time > 250)
    {
        btn1.pressed = true;
        btn1.last_debounce_time = time;
    }
}