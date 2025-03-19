#include "gui.hpp"
#include "devices.hpp"

using namespace modules;
using namespace devices;

GUI::GUI()
{
hub.onBuild(build);
}

void GUI::init()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(defines::SOFTAP_SSID, defines::SOFTAP_PASSWORD);
    hub.begin();
}

void GUI::tick()
{
    hub.tick();
}

void GUI::build(gh::Builder& b)
{

}
