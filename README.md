# minipad-firmware

This is the firmware for the minipad, an RP2040-based 2-to-3-key keypad for the rhythm game osu!.
This firmware is designed specifically to work with our open-source PCB, which can be found [here](https://github.com/minipadkb/minipad).

# Installation

To flash this firmware on your keypad, please refer to our [firmware installation guide](https://minipad.minii.moe/docs/minipad/install-firmware). In there, you can find the instructions on how to initially load the firmware on the keypad, and how to update it later on.

# Setup

Setting up this project is really simple. The repository is set up using PlatformIO, an IDE in form of an extension for Visual Studio Code.
You can find a download link for Visual Studio Code [here](https://code.visualstudio.com/). In there, search and install the extension `PlatformIO`.

After that is done, clone the repository with `git clone https://github.com/minipadkb/minipad-firmware` and open the cloned folder using Visual Studio Code. PlatformIO will perform an installation of all dependencies for this project (Arduino-Pico framework, various drivers, ...). This might take up to ~10 minutes, as a lot files need to be downloaded.

At the bottom of Visual Studio Code, you can find three important buttons, you can see them below. The checkmark compiles the project on the currently selected environment (seen on the right, in this case "env:minipad-3k").</br>
The arrow to the right uploads the firmware to your RP2040-based device (e.g. our PCB), given that it is in bootloader mode. At the right, you can select the environment. You can choose between the 2k firmware, 3k firmware and it's development versions. The development versions enable certain features that helps while development. If you want to find out more, search for the `DEBUG` definition across the source code.

![PlatformIO Buttons](https://raw.githubusercontent.com/minipadKB/minipad-firmware/master/.github/images/platformio_buttons.png)
