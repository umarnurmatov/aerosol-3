#pragma once

#include "GyverHub.h"

// BRIDGES
// #define GH_NO_STREAM  // отключить встроенный модуль связи stream
// #define GH_NO_HTTP    // отключить встроенный модуль связи http (для esp)
// #define GH_NO_WS      // отключить встроенный модуль связи ws (для esp)
// #define GH_NO_MQTT    // отключить встроенный модуль связи mqtt (для esp)

// #define GH_BRIDGE_AMOUNT 5       // количество мостов подключения
// #define GH_WS_PORT 81            // websocket порт
// #define GH_MQTT_RECONNECT 10000  // период переподключения встроенного MQTT (для esp)

// MODULES
// #define GH_NO_PAIRS    // отключить поддержку типа Pairs
// #define GH_NO_GET      // отключить отправку на get-топик (mqtt)
// #define GH_NO_INFO     // отключить info билдер
// #define GH_NO_MODULES  // отключить "модули" (все включены)
// #define GH_NO_REQUEST  // отключить поддержку запросов

// FS
// #define GH_NO_FS                 // отключить файловую систему (для esp)
// #define GH_FETCH_CHUNK_SIZE 512  // размер чанка при скачивании с платы (для esp)
// #define GH_UPL_CHUNK_SIZE 512    // размер чанка при загрузке на плату (для esp)
// #define GH_FS_DEPTH 5            // глубина сканирования файловой системы (esp32)

// TRANSFER
// #define GH_NO_FETCH    // отключить скачивание файлов (для esp)
// #define GH_NO_UPLOAD   // отключить загрузку файлов (для esp)
// #define GH_NO_OTA      // отключить ОТА обновления (для esp)
// #define GH_NO_OTA_URL  // отключить ОТА по ссылке (для esp)
// #define GH_REBOOT_DELAY 1500    // задержка перед перезагрузкой после OTA (для esp)

// PORTAL
// #define GH_FILE_PORTAL     // загружать сайт из памяти есп (положи файлы в папку /hub/) (для esp)
// #define GH_INCLUDE_PORTAL  // включить сайт в память программы, ~50кБ (не нужно загружать файлы в память) (для esp)

// HTTP
// #define GH_PUBLIC_PATH "/www"          // путь к папке с файлами с HTTP доступом (для esp)
// #define GH_HTTP_PORT 80                // http порт (для esp)
// #define GH_CACHE_PRD "max-age=604800"  // период кеширования файлов для портала (для esp)

// #define GH_NO_HTTP_TRANSFER       // отключить загрузку/скачивание/ОТА по http (для esp)
// #define GH_NO_HTTP_DNS            // отключить DNS сервер (для esp)
// #define GH_NO_HTTP_OTA            // отключить ОТА обновления по http (для esp)
// #define GH_NO_HTTP_PUBLIC         // отключить доступ к файлам по http c ip/www (для esp)
// #define GH_NO_HTTP_FETCH          // отключить скачивание файлов по http (для esp)
// #define GH_NO_HTTP_UPLOAD         // отключить загрузку файлов по http (для esp)
// #define GH_NO_HTTP_UPLOAD_PORTAL  // упрощённую загрузку файлов с ip/hub/upload_portal (для esp)

#define GH_FS SD

namespace modules
{
class GUI_GuverHub
{
public:
  GUI_GuverHub();

  void init();
  void tick();

private:
  static void build(gh::Builder& b);
};
}
