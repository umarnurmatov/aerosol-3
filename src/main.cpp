#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include <map>
#include <vector>

#include "GyverOLED.h"
#include "Button.h"
#include "TinyGPSPlus.h"

///////////////////////////////////
#define SERIAL_BAUD         115200

#define FILE_WRITE_PERIOD   3000

#define GPS_RX_PIN          2
#define GPS_TX_PIN          4
#define GPS_BAUD            9600
#define GPS_MAX_WAIT_TIME   5000
#define GPS_FEED_TIME       1000 

#define PM_SENSOR_BAUD      9600

#define SHUTDOWN_BUTTON_PIN 15 
///////////////////////////////////


/////////////////////////////////////////////////
Button shutdownBtn {15, false};
GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;
TinyGPSPlus gps;
String filename;
/////////////////////////////////////////////////


////////////////////////
static bool initSD();
static bool initGPS();
static bool initFile();
static void shutDown();

bool get_pm(float& pm_10, float& pm_25);

static void feedGpsSomeData(unsigned long ms);
static bool isGPSDataValid();

static void print_oled(const char* msg, int y = 1, int x = 1, bool clear = true, bool autoprint = true);
static void writeFile(const char* path, String& message);

void IRAM_ATTR isr_button();
///////////////////////


void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial1.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    Serial2.begin(PM_SENSOR_BAUD, SERIAL_8N1, -1, -1, false);
    
    pinMode(shutdownBtn.PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(shutdownBtn.PIN), isr_button, RISING);

    /////////////////
    oled.init();

    if (!initSD())
        while(true);

    if(!initGPS())
        while(true);

    if(!initFile())
        while(true);
    ////////////////

    Serial.println("Init success!");
    print_oled("INIT SUCCESS!");
}


void loop() {
    float pm_10 = 0.f, pm_25 = 0.f;
    static unsigned long timet = 0;
    if(shutdownBtn.pressed)
    {
        shutDown();
    }

    feedGpsSomeData(GPS_FEED_TIME);

    if (get_pm(pm_10, pm_25) && isGPSDataValid()) {
        //"dd-mm-yyyy,hh:mm:ss,PM2.5,PM10,lat,lng,alt"
        //the "\n" is added in writeFile()
        if (millis() - timet >= FILE_WRITE_PERIOD) {
            String strdata;
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
            strdata += String(pm_25);
            strdata += ",";
            strdata += String(pm_10);
            strdata += ",";
            strdata += String(gps.location.lat(), 6U);
            strdata += ",";
            strdata += String(gps.location.lng(), 6U);
            strdata += ",";
            strdata += String(gps.altitude.meters(), 2U);

            String dispdata;
            dispdata += "DATE ";
            dispdata += String(gps.date.day());
            dispdata += "-";
            dispdata += String(gps.date.month());
            dispdata += "-";
            dispdata += String(gps.date.year());
            print_oled(dispdata.c_str(), 1, 1, true, false);
            dispdata.clear();

            dispdata += "TIME ";
            dispdata += String(gps.time.hour());
            dispdata += ":";
            dispdata += String(gps.time.minute());
            dispdata += ":";
            dispdata += String(gps.time.second());
            print_oled(dispdata.c_str(), 2, 1, false, false);
            dispdata.clear();

            dispdata += "PM ";
            dispdata += String(pm_25, 1U);
            dispdata += ",";
            dispdata += String(pm_10, 1U);
            print_oled(dispdata.c_str(), 3, 1, false, false);
            dispdata.clear();

            dispdata += String(gps.location.lat(), 4U) + "N";
            dispdata += " ";
            dispdata += String(gps.location.lng(), 4U) + "E";
            print_oled(dispdata.c_str(), 4, 1, false, false);
            dispdata.clear();


            dispdata += "ALT " + String(gps.altitude.meters(), 2U) + "m";
            print_oled(dispdata.c_str(), 5, 1, false, false);
            dispdata.clear();

            Serial.println(strdata);
            writeFile(filename.c_str(), strdata);
            timet = millis();
        }
    }

}

static bool initSD() 
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

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    print_oled("SD INIT SUCCESS");
    return true;
}

static bool initGPS()
{
    unsigned long timer = millis();
    print_oled("WAITING GPS FIX...");
    Serial.println("Waiting GPS data");
    while(true)
    {
        feedGpsSomeData(GPS_FEED_TIME);
        if(isGPSDataValid()) break;
        if ((timer - millis() > GPS_MAX_WAIT_TIME) && gps.charsProcessed() < 10)
        {
            print_oled("NO GPS DATA RECEIVEDCHECK WIRING");
            Serial.println(F("No GPS data received: check wiring"));
            return false;
        }
    }
    print_oled("GPS GOT 3D FIX");
    Serial.println("GPS got 3D fix");
    return true;
}

// <MM-DD-YYYY$hh-mm-ss.csv>
static bool initFile()
{
    feedGpsSomeData(GPS_FEED_TIME); 
    if(!isGPSDataValid())
    {
        print_oled("FAIL INIT FILE: GPS INVALID");
        Serial.println("Fail init file: GPS data not valid");
        return false;
    } 
    TinyGPSDate date = gps.date;
    TinyGPSTime time = gps.time;
    
    filename = String("/") + String(date.month()) + String("-") + String(date.day()) + String("-") 
                + String(date.year()) + String("$") + String(time.hour())+ String("-") + String(time.minute())
                + String("-") + String(time.second()) + String(".csv");

    if(!SD.exists(filename))
    {
        File file = SD.open(filename, FILE_WRITE, true);
        file.println("date,time,PM2.5,PM10,lat,lng,alt");
        file.close();
    }
    return true;
}

static void shutDown()
{
    Serial.println("Shut down");
    print_oled("SHUT DOWN");
    SD.end();
    Serial.end();
    Serial1.end();
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

static void feedGpsSomeData(unsigned long ms)
{
    unsigned long start = millis();
    do 
    {
        while (Serial1.available()) 
            gps.encode(Serial1.read());
    } while (millis() - start < ms);
}

bool isGPSDataValid()
{
    if(gps.location.isValid() && gps.time.isValid() && gps.date.isValid() && gps.altitude.isValid()) return true;
    return false;
}

void print_oled(const char* msg, int y, int x, bool clear, bool autoprint)
{
    if(clear) oled.clear();
    oled.autoPrintln(autoprint);
    oled.setCursor(x, y);
    oled.print(msg);
    oled.update();
}

static void writeFile(const char* path, String& message) {
    Serial.printf("Writing file: %s\n", path);

    File file = SD.open(path, FILE_APPEND);
    if (!file) 
    {
        Serial.println("Failed to open file for writing");
        print_oled("FAILED OPEN FILE FOR WRITE", 1, 1, true);
        while(true);
    }
    if (file.println(message)) 
    {
        Serial.println("File written");
        print_oled("FILE WRITTEN", 6, 1, false);
    } 
    else 
    {
        Serial.println("Write failed");
        print_oled("WRITE FAIL", 1, 1, true);
        file.close();
        while(true);
    }
    file.close();
}

void IRAM_ATTR isr_button()
{
    unsigned long time = millis();
    if (time - shutdownBtn.last_debounce_time > 250)
    {
        shutdownBtn.pressed = true;
        shutdownBtn.last_debounce_time = time;
    }
}