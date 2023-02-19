import os
import os.path
import sys
import requests

# Get the firmware version from the definitions.hpp file
def get_firmware_version() -> str:
    lines = open("./include/definitions.hpp", "r").readlines()
    for line in lines:
        if line.startswith("#define FIRMWARE_VERSION"):
            return line.split('"')[1]

    return None


def main():
    
    # Check if exactly one argument was specified
    if len(sys.argv) != 2:
        print("::error file=ci-util.py,line=15::Invalid argument count")

    # Return the version of the firmware
    elif sys.argv[1] == "--set-version-env-var":
        version = get_firmware_version()
        
        if not version:
            print("::error Cannot find FIRMWARE_VERSION definition in definitions.hpp")
            return
        
        print(f"::notice Parsed firmware version '{version}'")
        with open("$GITHUB_OUTPUT", "a+") as f:
            f.write(f"firmware_version={version}\n")

    # Get the newest version of the firmware from the releases on GitHub and compare the versions for changes
    # If no change was found, return an exit code > 0 in order to make the workflow ci fail
    elif sys.argv[1] == "--fail-on-no-version-increment":
       json = requests.get("https://api.github.com/repos/minipadkb/minipad-firmware-reloaded/releases").json()
       if len(json) > 0 and json[-1]["tag_name"] != get_firmware_version():
           print("::error Firmware version of this build and the latest release match")

if __name__ == "__main__":
    main()