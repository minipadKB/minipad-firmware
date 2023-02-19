import os
import os.path
import sys
import requests
import itertools

# Get the firmware version from the definitions.hpp file
def get_firmware_version() -> str:
    lines = open("./include/definitions.hpp", "r").readlines()
    for line in lines:
        if line.startswith("#define FIRMWARE_VERSION"):
            version = line.split('"')[1]
            print(f"::notice::Parsed firmware version '{version}'")
            return version

    return None

def get_changelog(version: str) -> str:
    lines = open("./CHANGELOG.md", "r").readlines()
    start = itertools.dropwhile(lambda x: not x.startswith(f"# {version}"), lines)
    changelog = itertools.takewhile(lambda x: not x.startswith("# "), start)
        
    return "\n".join(changelog)

def main():
    
    # Check if exactly one argument was specified
    if len(sys.argv) != 2:
        print("::error::Invalid argument count")
        sys.exit(1)
        
    # Get firmware version
    version = get_firmware_version()
    
    if not version:
        print("::error::Cannot find FIRMWARE_VERSION definition in definitions.hpp")
        sys.exit(1)

    # Return the version of the firmware
    elif sys.argv[1] == "--version":
        print(version)

    # Get the newest version of the firmware from the releases on GitHub and compare the versions for changes
    # If no change was found, return an exit code > 0 in order to make the workflow ci fail
    elif sys.argv[1] == "--fail-on-no-version-increment":
        json = requests.get("https://api.github.com/repos/minipadkb/minipad-firmware-reloaded/releases").json()
       
        if len(json) > 0 and json[0]["tag_name"] == version:
           print("::error::Firmware version of this build and the latest release match")
           sys.exit(1)
           
        print("::notice::Firmware versions are different, continuing...")
        
    # Check if a changelog for the new version exists. If not, make the workflow ci fail. If it exists, return it
    elif sys.argv[1] == "--changelog":
        changelog = get_changelog(version)
        
        if changelog == "":
            print("::error::Changelog not found")
            sys.exit(1)
            
        print(changelog)

if __name__ == "__main__":
    main()