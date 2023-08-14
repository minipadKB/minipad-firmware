import os
import os.path
import sys
import requests
import itertools

from typing import Optional

# Get the firmware version from the definitions.hpp file
def get_firmware_version() -> Optional[str]:
    with open("./include/definitions.hpp", encoding="utf8") as f:
        for line in f.readlines():
            if line.startswith("#define FIRMWARE_VERSION"):
                version = line.split('"')[1]
                return version

    return None

# Get the changelog from the CHANGELOG.md file via # headers
def get_changelog(version: str) -> tuple[str, list[str]]:
    with open("./CHANGELOG.md", "r") as f:
        # Get all lines in the file
        full_changelog = f.readlines()

        # Check if a section for the version exists
        if not any(line.startswith(f"# {version}") for line in full_changelog):
            print(f"::error::Changelog for version '{version}' not found")
            sys.exit(1)

        # Skip the lines while it does not start with # <version>
        from_version_start = list(itertools.dropwhile(lambda line: not line.startswith(f"# {version}"), full_changelog))

        # Remember the version title and remove the line afterwards
        version_title = from_version_start[0][2:-1]
        without_version_header = itertools.islice(from_version_start, 1, None)

        # Get the body of the version section until another version header or the end of the file was reached
        changelog = itertools.takewhile(lambda x: not x.startswith("# "), without_version_header)

        # Remove all blank lines at the start of the changelog
        changelog = itertools.dropwhile(lambda x: x == "\n", changelog)

        return (version_title, list(changelog))

def main() -> None:
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
        print(f"::notice::Set firmware_version variable in $GITHUB_OUTPUT to '{version}'")
        os.system(f'echo "firmware_version={version}" >> $GITHUB_OUTPUT')

    # Get the newest version of the firmware from the releases on GitHub and compare the versions for changes
    # If no change was found, return an exit code > 0 in order to make the workflow ci fail
    elif sys.argv[1] == "--fail-on-no-version-increment":

        json = requests.get("https://api.github.com/repos/minipadkb/minipad-firmware/releases").json()

        # Check whether there is a release and the versions match
        if len(json) > 0 and json[0]["tag_name"] == version:
           print(f"::error::Firmware version of this build and the latest release are identical ('{version}')")
           sys.exit(1)
        elif len(json) == 0:
            print(f"::notice::No previous release found")
        else:
            print(f"::notice::Firmware versions are different (this: '{json[0]['tag_name']}', release: '{version}')")

    # Check if a changelog for the new version exists. If not, make the workflow ci fail. If it exists, return it
    elif sys.argv[1] == "--generate-changelog":

        changelog_title, changelog = get_changelog(version)

        # Check whether getting the changelog was successful
        if len(changelog) == 0:
            print(f"::error::Changelog for version '{version}' not found")
            sys.exit(1)

        # Write the version title (after the '# ') to $GITHUB_OUTPUT
        os.system(f'echo "changelog_title={changelog_title}" >> $GITHUB_OUTPUT')

        # Write the changelog to the file
        with open("CURRENT_CHANGELOG.md", "w") as f:
            f.writelines(changelog)

        print(f"::notice::Changelog for version '{version}' was saved in 'CURRENT_CHANGELOG.md'")

if __name__ == "__main__":
    main()
