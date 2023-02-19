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
            return version

    return None

# Get the changelog from the CHANGELOG.md file via # headers
def get_changelog(version: str) -> list[str]:
    lines = open("./CHANGELOG.md", "r").readlines()
    
    # Skip the lines while it does not start with # <version>, then skip the header line and take until there's another header (or end of file)
    start = list(itertools.dropwhile(lambda x: not x.startswith(f"# {version}"), lines))
    changelog = list(itertools.takewhile(lambda x: not x.startswith("# "), start[1:]))
    print(changelog)
    # Remove all blank lines at the start of the changelog
    changelog = list(itertools.dropwhile(lambda x: x == "\n", changelog))
    print(changelog)
        
    return changelog

def main():
    
    # Check if exactly one argument was specified
    if len(sys.argv) != 2:
        print("::error::Invalid argument count")
        sys.exit(1)
        
    # Get firmware version
    version = get_firmware_version()
    
    # Check if getting the firmware version was successful
    if not version:
        print("::error::Cannot find FIRMWARE_VERSION definition in definitions.hpp")
        sys.exit(1)

    # Set the version of the firmware in the workflow environment variable $GITHUB_OUTPUT
    elif sys.argv[1] == "--set-version":
        os.system(f'echo "firmware_version={version}" >> $GITHUB_OUTPUT')

    # Get the newest version of the firmware from the releases on GitHub and compare the versions for changes
    # If no change was found, return an exit code > 0 in order to make the workflow ci fail
    elif sys.argv[1] == "--fail-on-no-version-increment":
        
        json = requests.get("https://api.github.com/repos/minipadkb/minipad-firmware-reloaded/releases").json()
       
        # Check whether there is a release and the versions match
        if len(json) > 0 and json[0]["tag_name"] == version:
           print(f"::error::Firmware version of this build and the latest release are identical ('{version}')")
           sys.exit(1)
           
        print(f"::notice::Firmware versions are different (this: '{json[0]['tag_name']}', release: '{version}')")
        
    # Check if a changelog for the new version exists. If not, make the workflow ci fail. If it exists, return it
    elif sys.argv[1] == "--generate-changelog":
        
        changelog = get_changelog(version)
        
        # Check whether getting the changelog was successful
        if len(changelog) == 0:
            print(f"::error::Changelog for version '{version}' not found")
            sys.exit(1)
            
        # Write the changelog to the file
        with open("CURRENT_CHANGELOG.md", "w") as f:
            f.writelines(changelog)
            
        print(f"::notice::Changelog for version '{version}' was saved in 'CURRENT_CHANGELOG.md'")

if __name__ == "__main__":
    main()