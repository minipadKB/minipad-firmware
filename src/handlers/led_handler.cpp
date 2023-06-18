#include "handlers/led_handler.hpp"
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

    // Set the brightness of the LEDs.
    pixels.setBrightness(ConfigController.config.leds.brightness);

    // Update the pixels.
    pixels.show();
}
