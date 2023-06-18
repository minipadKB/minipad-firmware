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
        uint8_t alpha = (ConfigController.config.leds[i].argb >> 24) & 0xFF;
        uint8_t red = (ConfigController.config.leds[i].argb >> 16) & 0xFF;
        uint8_t green = (ConfigController.config.leds[i].argb >> 8) & 0xFF;
        uint8_t blue = ConfigController.config.leds[i].argb & 0xFF;
        pixels.setPixelColor(i, red, green, blue);
    }

    pixels.show();
}
