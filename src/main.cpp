#include <Arduino.h>
#include <EEPROM.h>
#include <Keyboard.h>
#include "config/configuration_controller.hpp"
#include "handlers/serial_handler.hpp"
#include "handlers/keypad_handler.hpp"

void setup()
{
    // Initialize the EEPROM and load the configuration from it.
    EEPROM.begin(1024);
    ConfigController.loadConfig();

    // Initialize the serial and HID interface.
    Serial.begin(115200);
    Keyboard.begin();
    Keyboard.setAutoReport(false);

    // Set the amount of bits for the ADC to the defined one for a better resolution on the analog readings.
    analogReadResolution(ANALOG_RESOLUTION);

    // Allows to boot into UF2 bootloader mode by pressing the reset button twice.
    rp2040.enableDoubleResetBootloader();
}

void loop()
{
    // Check for any serial commands received.
    while (Serial.available() > 0)
    {
        char buff[INPUT_BUFF_SIZE];

        const size_t bytesRead = Serial.readBytesUntil('\n', buff, INPUT_BUFF_SIZE - 1);
        buff[bytesRead] = '\0';

        SerialHandler.handleSerialInput(buff);
    }

    // Run the keypad handler checks to handle the actual keypad functionality.
    KeypadHandler.handle();
}
