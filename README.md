<div align="center">

# minipad-firmware

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
<a href="https://discord.gg/minipad"><img src="https://discordapp.com/api/guilds/1056311828344483840/widget.png"></a>

The firmware for the minipad, an RP2040-based 2-3-key keypad for the rhythm game osu!.

This firmware is designed specifically to work with our open-source PCB, which can be found in our GitHub repository [here](https://github.com/minipadkb/minipad).

[Features](#features) • [Installation](#installation-on-an-rp2040-micro-controller) • [Development Setup](#setup-for-development)</br>
[Serial Protocol](#minipad-serial-protocol-msp) • [Licenses](#licenses)
</div>

# Features

Although this firmware is made for the aforementioned PCB, it can be used for different kinds of (hall effect) keypad/keyboard projects due to it's support of both digital and hall effect buttons, as well as no real limitation on how many keys to use.

Here is a list of features that are both planned and available:
- An unlimited amount of Hall Effect or Digital keys
- Rapid Trigger (explained [here](https://github.com/minipadKB/minipad-firmware/blob/master/src/handlers/keypad_handler.cpp#L13)) with 0.01mm resolution
- Flexible, configurable travel distance of switches
- Adjustable actuation point (0.01mm resolution)
- Software-based low pass filter for analog stability
- Configurable keychar pressed upon key interaction
- Serial communication protocol for configuration
- UI application for configuration, [minitility](https://github.com/minipadkb/minitility)

Planned Features
-
- Support for RGB lights, including configurable colors and effects
- Usage of multiplexers to allow for more keys to be used on the hardware

# Installation on an RP2040 micro controller

To flash this firmware on your minipad or other RP2040 board, please refer to our [firmware installation guide](https://minipad.minii.moe/docs/minipad/install-firmware). In there, you can find the instructions on how to initially load the firmware on the keypad, and how to update it later on.

# Setup for development

Setting up this project is really simple. The repository is set up using PlatformIO, an IDE in form of an extension for Visual Studio Code.
You can find a download link for Visual Studio Code [here](https://code.visualstudio.com/). In there, search and install the extension `PlatformIO`.

After that is done, clone the repository with `git clone https://github.com/minipadkb/minipad-firmware` and open the cloned folder using Visual Studio Code. PlatformIO will perform an installation of all dependencies for this project (Arduino-Pico framework, various drivers, ...). This might take up some time, depending on your network connection.

If you are not familiar with the usage of PlatformIO, a Quick Start guide can be found [here](https://docs.platformio.org/en/stable/integration/ide/vscode.html).

Note: Uploading the firmware only works if the micro controller is set into bootloader mode. This can be done using the BOOTSEL button on development boards or setting the minipad into bootloader mode via minitility. A guide on the latter can be found [here](https://minipad.minii.moe/docs/minitility/get-started).

# Minipad Serial Protocol (MSP)

The firmware is being configured and accessed from the host device via Serial communication at a baud rate of 115200.

All data sent via the serial interface is being interpreted as a command with the following syntax:
`command arg0 arg1 arg2 ...`. The command and it's arguments are split by whitespaces, ending with a newline character.

There is a differention between a global and key-related command. As for keys, namingly hall effect keys (identifier `hkey`) and digital keys (identifier `dkey`), the command syntax looks the following: `identifier.command arg0 arg1 arg2 ...`.

Either a single key or all keys at one can be targetted. If you wish to target a single key, you can put the one-based index of the key after the identifier. (e.g. `hkey1`, `dkey3`)

Here is a list of commands and examples for them:

**Global commands**
-
*Command*: `boot`</br>
*Syntax*: `boot`</br>
*Example*: `boot`</br>
*Description*: Sets the device into bootloader mode.

*Command*: `save`</br>
*Syntax*: `save`</br>
*Example*: `save`</br>
*Description*: Writes the current configuration of the keypad to the EEPROM.

*Command*: `get`</br>
*Syntax*: `get`</br>
*Example*: `get`</br>
*Description*: Returns the configuration of the keypad, in the `GET key=value` format.

*Command*: `name`</br>
*Syntax*: `name <string>`</br>
*Example*: `name mini's minipad`</br>
*Description*: Sets the name of the minipad, used to distinguish different devices visually.

*Command*: `out`</br>
*Syntax*: `out <bool>`</br>
*Example*: `out true`, `out 0`</br>
*Description*: Enables/Disables the output mode. The output mode writes the sensor values to the serial monitor, allowing them to be read by software on the host device.

*Command*: `echo` (debug-exclusive)</br>
*Syntax*: `echo <string>`</br>
*Example*: `echo I am a string.`</br>
*Description*: Echoes the specified string, used for development purposes.

**Key-related commands**
-
*Command*: `hkey.rt`</br>
*Syntax*: `hkey.rt <bool>`</br>
*Example*: `hkey.rt 1`</br>
*Description*: Enables/Disables Rapid Trigger functionality on the specified key(s).

*Command*: `hkey.crt`</br>
*Syntax*: `hkey.crt <bool>`</br>
*Example*: `hkey.crt false`</br>
*Description*: Enables/Disables Continuous Rapid trigger functionality on the specified key(s).

*Command*: `hkey.rtus`</br>
*Syntax*: `hkey.rtus <uint16>`</br>
*Example*: `hkey.rtus 45`</br>
*Description*: Sets the sensitivity for an upwards movement on the Rapid Trigger feature. The unit of the value is 0.01mm.

*Command*: `hkey.rtds`</br>
*Syntax*: `hkey.rtds <uint16>`</br>
*Example*: `hkey.rtds 10`</br>
*Description*: Sets the sensitivity for a downwards movement on the Rapid Trigger feature. The unit of the value is 0.01mm.

*Command*: `hkey.lh`</br>
*Syntax*: `hkey.lh <uint16>`</br>
*Example*: `hkey.lh 250`</br>
*Description*: Sets the lower hysteresis for the actuation point below which the key is being pressed. The unit of the value is 0.01mm.

*Command*: `hkey.uh`</br>
*Syntax*: `hkey.uh <uint16>`</br>
*Example*: `hkey.uh 320`</br>
*Description*: Sets the upper hysteresis for the actuation point above which the key is no longer being pressed. The unit of the value is 0.01mm.

*Command*: `hkey.rest`</br>
*Syntax*: `hkey.rest <uint16>`</br>
*Example*: `hkey.rest 1820`</br>
*Description*: Sets the upper calibration value for the specified key(s). This value acts as the upper boundary of the values read from the hall effect sensor across the travel distance. (when the key is fully lifted)

*Command*: `hkey.down`</br>
*Syntax*: `hkey.down <uint16>`</br>
*Example*: `hkey.down 1159`</br>
*Description*: Sets the lower calibration value for the specified key(s). This value acts as the lower boundary of the values read from the hall effect sensor across the travel distance. (when the key is fully pressed)

*Command*: `hkey.char`, `dkey.char`</br>
*Syntax*: `?key.char <uint8>`</br>
*Example*: `dkey.char 97`</br>
*Description*: Sets the character pressed when the specified key(s) is/are pressed down. The value is the ASCII number of the character. Only lower-case a-z is supported.

*Command*: `hkey.hid` / `dkey.hid`</br>
*Syntax*: `?key.hid <bool>`</br>
*Example*: `dkey.hid false`</br>
*Description*: Enables/Disables the HID output (meaning whether the key signal is sent to the host device) on the specified key(s).

# Licenses

[arduino-pico](https://github.com/minipadkb/arduino-pico) by [minipadkb](https://github.com/minipadkb), fork of [arduino-pico](https://github.com/earlephilhower/arduino-pico) by [earlephilhower](https://github.com/earlephilhower), distributed under [LGPL v2.1](https://github.com/earlephilhower/arduino-pico/blob/master/LICENSE)

[Keyboard](https://github.com/minipadKB/minipad-firmware) by [minipadkb](https://github.com/minipadKB), fork of [Keyboard](https://github.com/earlephilhower/Keyboard) by [earlephilhower](https://github.com/earlephilhower), distributed under [LGPL v3.0](https://github.com/minipadkb/Keyboard/blob/master/LICENSE)

[pico-sdk](https://github.com/minipadKB/pico-sdk) by [minipadkb](https://github.com/minipadKB), fork of [pico-sdk](https://github.com/raspberry/pico-sdk) by [raspberry](https://github.com/raspberry), distributed under [BSD-3-Clause](https://github.com/minipadkb/pico-sdk/blob/master/LICENSE)

[Keyboard](https://github.com/minipadKB/minipad-firmware) by [minipadkb](https://github.com/minipadKB), fork of [Keyboard](https://github.com/earlephilhower/Keyboard) by [earlephilhower](https://github.com/earlephilhower), distributed under [LGPL v3.0](https://github.com/minipadkb/Keyboard/blob/master/LICENSE)
