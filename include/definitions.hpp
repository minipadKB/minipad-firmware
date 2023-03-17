#pragma once

// Define the ports used for the HE sensors.
#if KEYS == 3
#define HE_PINS    \
    {              \
        A0, A1, A2 \
    }
#else
#define HE_PINS \
    {           \
        A0, A1  \
    }
#endif

// The version of this firmware in the YYYYMMDD.PATCH format. (e.g. 20221219.2 for the 2nd release on the 19th december 2022)
#define FIRMWARE_VERSION "20230215.1"

//
//                                           WARNING
//
//       DO NOT CHANGE UNLESS ADVICED. MODIFIED VALUES MAY CAUSE AN INCONSISTENT KEYPAD BEHAVIOR
//    THAT VIOLATES OSU!'S RULES. CHANGING THIS MAY LEAD TO INCORRECT KEY PRESSES AND A RESTRICTION.
//
#define HYSTERESIS_TOLERANCE 10
#define RAPID_TRIGGER_TOLERANCE 10
#define ANALOG_RESOLUTION 12