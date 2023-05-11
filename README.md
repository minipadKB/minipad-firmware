[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
<a href="https://discord.gg/minipad"><img src="https://discordapp.com/api/guilds/1056311828344483840/widget.png"></a>


# minipad-firmware

This is the firmware for the minipad, an RP2040-based 2-to-3-key keypad for the rhythm game osu!.
This firmware is designed specifically to work with our open-source PCB, which can be found [here](https://github.com/minipadkb/minipad).

Although this firmware is made for aforementioned PCB, it can be used for different kinds of (hall effect) keypad/keyboard projects due to it's support of both digital and hall effect buttons, as well as no real limitation on how many keys to use. Support for multiplexing is planned but has not started development.

# Features

- An unlimited amount of Hall Effect or Digital keys
- Rapid Trigger (explained [here](https://github.com/minipadKB/minipad-firmware/blob/master/src/handlers/keypad_handler.cpp#L13)) with 0.01mm resolution
- Flexible, configurable travel distance of switches
- Adjustable actuation point (0.01mm resolution)
- Software-based low pass filter for analog stability
- Configurable keychar pressed upon key interaction

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

Note: Uploading the firmware only works if the micro controller is set into bootloader mode. This can be done using the BOOTSEL button on development boards or setting the minipad into bootloader mode via minitility. A guide on the latter can be found [here]().

# Licenses

[arduino-pico](https://github.com/minipadkb/arduino-pico) by [minipadkb](https://github.com/minipadkb), fork of [arduino-pico](https://github.com/earlephilhower/arduino-pico) by [earlephilhower](https://github.com/earlephilhower), distributed under [LGPL v2.1](https://github.com/earlephilhower/arduino-pico/blob/master/LICENSE)

[Keyboard](https://github.com/minipadKB/minipad-firmware) by [minipadkb](https://github.com/minipadKB), fork of [Keyboard](https://github.com/earlephilhower/Keyboard) by [earlephilhower](https://github.com/earlephilhower), distributed under [LGPL v3.0](https://github.com/minipadkb/Keyboard/blob/master/LICENSE)

[pico-sdk](https://github.com/minipadKB/pico-sdk) by [minipadkb](https://github.com/minipadKB), fork of [pico-sdk](https://github.com/raspberry/pico-sdk) by [raspberry](https://github.com/raspberry), distributed under [BSD-3-Clause](https://github.com/minipadkb/pico-sdk/blob/master/LICENSE)

[Keyboard](https://github.com/minipadKB/minipad-firmware) by [minipadkb](https://github.com/minipadKB), fork of [Keyboard](https://github.com/earlephilhower/Keyboard) by [earlephilhower](https://github.com/earlephilhower), distributed under [LGPL v3.0](https://github.com/minipadkb/Keyboard/blob/master/LICENSE)
