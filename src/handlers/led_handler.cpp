#include "handlers/led_handler.hpp"
#include "handlers/keypad_handler.hpp"
#include "config/configuration_controller.hpp"
#include "definitions.hpp"

void LEDHandler::setup()
{
    // Initialize the Adafruit_NeoPixel instance.
    pixels.begin();
}

void LEDHandler::loop()
{
    // Go through all LEDs and set the configured color.
    for (int i = 0; i < LEDS; i++)
        pixels.setPixelColor(i, ConfigController.config.leds.leds[i].rgb);

    // Set the brightness of the LEDs to full brightness. This is scaled with the configured brightness afterwards.
    setBrightness(100);

    // Update the pixels.
    pixels.show();
}

void LEDHandler::setBrightness(uint8_t value)
{
    // Set the brightness to the specified value scaled with the value in the config.
    pixels.setBrightness(value * ConfigController.config.leds.brightness / 100);
}
