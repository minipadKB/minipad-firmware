#pragma once

#include <Adafruit_NeoPixel.h>
#include "definitions.hpp"

inline class LEDHandler
{
public:
    void setup();
    void loop();

private:
    Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LEDS, LED_PIN, LED_TYPE);

} LEDHandler;
