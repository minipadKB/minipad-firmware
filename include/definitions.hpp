#pragma once

// Define this if the firmware should be compiled with 3-key support
#define KEYS_3

// Define the ports used for the HE sensors
#ifdef KEYS_3
#define HE_PINS { A1, A2, A3 }
#else
#define HE_PINS { A1, A2 }
#endif