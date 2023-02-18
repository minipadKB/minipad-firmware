#pragma once

// Define this if the firmware should be compiled with 3-key support.
// This define is purely for development purposes, the CI/CD workflow defines
// or undefines the it via a build flag, disregarding this line completely.
// #define KEYS_3

// Define the ports used for the HE sensors.
#ifdef KEYS_3
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

// The version of this firmware in the YYYYMMDD.PATCH format. (e.g. 20221219.2-2k for the 2nd release on the 19th december 2022 and the 2-key version)
#define FIRMWARE_VERSION "20230215.1"
#ifdef KEYS_3
#define FIRMWARE_VERSION_FULL FIRMWARE_VERSION "-3k"
#else
#define FIRMWARE_VERSION_FULL FIRMWARE_VERSION "-2k"
#endif

//
//                                           WARNING
//
//       DO NOT CHANGE UNLESS ADVICED. MODIFIED VALUES MAY CAUSE AN INCONSISTENT KEYPAD BEHAVIOR
//    THAT VIOLATES OSU!'S RULES. CHANGING THIS MAY LEAD TO INCORRECT KEY PRESSES AND A RESTRICTION.
//
#define HYSTERESIS_TOLERANCE 10
#define RAPID_TRIGGER_TOLERANCE 10