#pragma once
#include "Arduino.h"

struct Button
{
    const uint8_t PIN;
    bool pressed;
    unsigned long last_debounce_time;

    Button(uint8_t _PIN, bool _pressed)
        : PIN{_PIN}, pressed{_pressed},
        last_debounce_time{0}
    {}
};