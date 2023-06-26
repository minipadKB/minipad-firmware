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
    // Run the effect configured in the config.
    switch (ConfigController.config.leds.effect)
    {
    case LedEffectType::Static:
        effect_static();
        break;
    case LedEffectType::Analog:
        effect_analog();
        break;
    }

    // Update the pixels.
    pixels.show();
}

#pragma region Effects

void LEDHandler::effect_static()
{
    // Go through all LEDs and set the configured color.
    for (int i = 0; i < LEDS; i++)
        pixels.setPixelColor(i, ConfigController.config.leds.leds[i].rgb);

    // Set the brightness of the LEDs to full brightness. This is scaled with the configured brightness afterwards.
    setBrightness(100);
}

void LEDHandler::effect_analog()
{
    // Go through all LEDs and set the configured color.
    for (int i = 0; i < LEDS; i++)
        pixels.setPixelColor(i, ConfigController.config.leds.leds[i].rgb);

    // Get the mapped value of the furthest down-pressed button.
    uint16_t minimum = TRAVEL_DISTANCE_IN_0_01MM;
    for (int i = 0; i < HE_KEYS; i++)
        minimum = min(minimum, KeypadHandler.heKeyStates[i].lastMappedValue);

    // Set the brightness on the LEDs to the minimum mapped to a range of 0-100.
    setBrightness((TRAVEL_DISTANCE_IN_0_01MM - minimum) * 100 / TRAVEL_DISTANCE_IN_0_01MM);
}

#pragma endregion

void LEDHandler::setBrightness(uint8_t value)
{
    // Set the brightness to the specified value scaled with the value in the config.
    pixels.setBrightness(value * ConfigController.config.leds.brightness / 100);
}
