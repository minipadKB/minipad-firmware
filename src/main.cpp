#include <Arduino.h>
#include <EEPROM.h>
#include <Keyboard.h>
#include "config/configuration_controller.hpp"
#include "handlers/serial_handler.hpp"
#include "handlers/keypad_handler.hpp"
#include "definitions.hpp"

void setup()
{
    // Initialize the EEPROM with 1024 bytes and load the configuration from it.
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
    // Run the keypad handler checks to handle the actual keypad functionality.
    KeypadHandler.handle();
}

void serialEvent()
{
    // Handle incoming serial data.
    while(Serial.available() > 0)
    {
        // Read the incoming serial data until a newline into a buffer and terminate it with a null terminator.
        char input[SERIAL_INPUT_BUFFER_SIZE];
        const size_t inputLength = Serial.readBytesUntil('\n', input, SERIAL_INPUT_BUFFER_SIZE);
        input[inputLength] = '\0';

        // Pass the read input to the serial handler to handle it.
        SerialHandler.handleSerialInput(input);
    }
}
