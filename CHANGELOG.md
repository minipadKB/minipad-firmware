# 2023.813.1 - QoL Update

This release does a lot of QoL changes, as well as doing the final preparations of the firmware for compabitility with the upcoming configuration software, minitility!</br>
This update was supposed to include the new auto-calibration functionality but it was deemed to not be production-ready, therefore all current changes are pushed with this smaller update.

## Features

- The `out` command now supports specifying no parameter at all, in which case it outputs the sensor values once
- The fields `lastSensorValue` and `lastMappedValue` have been added to the `HEKeyState`, allowing to access them across the firmware
- The key chars supported are no longer restricted to a-z, any character from 0-255 can now be chosen, including modifier keys
- The `hkey/dkey.char` command now allows characters like `a` or `7` as the parameter, instead of their ASCII number equivalent

## Bug Fixes

- Fixed rare cases in which command handling failed

## Changes

- Serial input handling is now using Arduino's built-in `serialEvent` function
- All includes of `stdint.h` have been replaced with C++'s `cstdint`
- The default values of the configuration have been migrated from the `getDefaultConfig` function to their structs

# 2023.516.1 - Digital key support, support for more keys!

This release adds support for digital keys, which might not be useful for minipad owners *yet*, but is for people DIY-ing a keypad or messing around with the firmware, as well as the collaboratoring commercial products of the Minipad Project. 

This release also prepares the communication with minitility, our configuration software which is coming closer to be ready to use.

## Features

- Added support for digital keys (push buttons, mechanical switches, ...)
- Added debounce for digital keys with a default of 50ms
- Made the amount of hall effect/digital keys fully dynamic, not limited to 2-3 but rather limited to the hardware specifications of the RP2040 micro controller

## Bug Fixes

- Refactored `getArgumentAt` function
- Fix `name` command not null-terminating

## Changes

- Migrated info retrieved via the `ping` to the `get` command
- Split the `key` identifier in the serial protocol into `hkey` (hall effect) and `dkey` (digital)
- Changed the command for changing the key pressed from `key` to `char`
- Removed config versions other than the main `Configuration` object, removed mutations
- Switched from hardcoded pin arrays to a formula macro
- Changed the default values for rest and down positions to 4095 and 0 respectively
- Added definition for reversing the sensor values

# 2023.410.1 - Initial release!

Initial release.

## Features

- Full serial communication for minitility
- Support for a custom actuation point
- Support for Rapid Trigger, Continuous Rapid Trigger and custom up/down sensitivity
- Support for changing keys
- Support for enabling/disabling keys
- Support for 2-3 keys
- Simple moving average implementation for analog stability
- Config being saved in EEPROM

## Bug Fixes

None in this release.

## Changes

None in this release.
