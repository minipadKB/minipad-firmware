#pragma once

// The version of this firmware in the YYYY.MDD.PATCH format. (e.g. 2022.1219.2 for the 2nd release on the 19th december 2022)
#define FIRMWARE_VERSION "2023.410.1"

// ┌───────────────────────────────────────────────────────────────────────────────────────────────────┐
// │                                                                                                   │
// │                                           !!!WARNING!!!                                           │
// │                                                                                                   │
// │      DO NOT CHANGE UNLESS ADVISED. MODIFIED VALUES MAY CAUSE AN INCONSISTENT KEYPAD BEHAVIOR      │
// │   THAT VIOLATES OSU'S RULES. CHANGING THIS MAY LEAD TO INCORRECT KEY PRESSES AND A RESTRICTION.   │
// │                                                                                                   │
// └───────────────────────────────────────────────────────────────────────────────────────────────────┘

// The minimum difference between the lower and upper hysteresis. This is important to not have the key continuously
// actuate if you do very very slight movements or if the fluctuation is simply too high. It also defines the gap there
// has to be between the defined TRAVEL_DISTANCE_IN_0_01MM and the upper hysteresis so the actuation logic cannot
// possibly get stuck if you press the key down but happen to not be able to get back up enough.
#define HYSTERESIS_TOLERANCE 10

// The minimum value for the rapid trigger sensitivities. This is important to not have the key continuously
// continuously actuate if you do very very slight movements or if the fluctuation is simply too high.
#define RAPID_TRIGGER_TOLERANCE 10

// The threshold when a key is considered fully released. 10 would mean if the key is <0.1mm pressed.
// This value is important to reset the rapid trigger state properly with continuous rapid trigger.
#define CONTINUOUS_RAPID_TRIGGER_THRESHOLD 10

// The resolution for the ADCs on the RP2040. The maximum compatible value on it is 12 bit.
#define ANALOG_RESOLUTION 12

// The exponent for the amount of samples for the SMA filter. This filter reduces fluctuation of analog values.
// A value too high may cause unresponsiveness. 1 = 1 sample, 2 = 4 samples, 3 = 8 samples, 4 = 16 samples, ...
#define SMA_FILTER_SAMPLE_EXPONENT 4

// The travel distance of the switches, where 1 unit equals 0.01mm. This is used to map the values properly to
// guarantee that the unit for the numbers used across the firmware actually matches the milimeter metric.
#define TRAVEL_DISTANCE_IN_0_01MM 400

// Uncomment this line if the sensor readings go up when the key is being pressed down.
// By default, the firmware is made to handle the readings going down and not up.
// #define INVERT_SENSOR_READINGS

// Macro for getting the hall effect sensor pin of the specified key index. The pin order is being swapped here,
// meaning on a 3-key device the pins are 28, 27 and 26. This macro has to be adjusted, depending on how the PCB
// and hardware of the device using this firmware has been designed. The A0 constant is 26 in the RP2040 environment.
// NOTE: By the uint8 datatype, the amount of keys is limited to 255.
// NOTE: By the default config initialization, the amount of keys is limited to
//       around 26 since the characters are assigned backwards started from 'z'.
// NOTE: By the RP2040, the amount of analog pins (and therefore keys) is limited o 4.
#define HE_PIN(index) A0 + HE_KEYS - index - 1

// Macro for getting the pin of the specified index of the digital key. The pin order is not swapped here, meaning
// the first digital key is on pin 0, the second on 1, and so on.
// NOTE: This way, the amount of keys is limited to 26 since the 27th key overlaps with the first analog port, 26.
#define DIGITAL_PIN(index) 0 + DIGITAL_KEYS - index - 1

// Add a compiler error if the firmware is being tried to built with more than the supported 4 keys.
// (only 4 ADC pins available)
#if HE_KEYS > 4
#error As of right now, the firmware only supports up to 4 hall effect keys.
#endif

// Add a compiler error if the firmware is being tried to built with more than the supported 26 digital keys.
// (limited amount of ports + characters)
#if DIGITAL_KEYS > 26
#error As of right now, the firmware only supports up to 26 digital keys.
#endif
