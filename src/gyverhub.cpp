#include "gui/gyverhub.hpp"
#include "devices.hpp"

using namespace modules;
using namespace devices;

GUI_GuverHub::GUI_GuverHub()
{
    hub.onBuild(build);
}

void GUI_GuverHub::init()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(defines::SOFTAP_SSID, defines::SOFTAP_PASSWORD);
    hub.begin();
}

void GUI_GuverHub::tick()
{
    hub.tick();
}

void GUI_GuverHub::build(gh::Builder& b)
{

}
