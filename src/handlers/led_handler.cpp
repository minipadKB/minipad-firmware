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
    for (int i = 0; i < 6; i++)
    {
        pixels.setPixelColor(i, ConfigController.config.leds[i].argb);
    }

    pixels.show();
}
