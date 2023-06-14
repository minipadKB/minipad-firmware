#pragma once

#include <Adafruit_NeoPixel.h>
#include "definitions.hpp"

inline class LEDHandler
{
public:
    void handle();

private:
    Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);

} LEDHandler;
