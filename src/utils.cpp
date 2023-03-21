#include <Arduino.h>
#include "utils.hpp"

char *Utils::getArgumentAt(char *input, char delimiter, uint8_t index)
{
    // Remember the amount of found elements, string index of the current one and the total length.
    int found = 0;
    int strIndex[] = {0, -1};
    int length = strlen(input) - 1;

    // Go through all characters and count the delimiters until the end or the desired index was reached.
    for (int i = 0; i <= length && found <= index; i++)
    {
        // Check if a delimiter was found (element ended) or the end was reached.
        if (input[i] == delimiter || i == length)
        {
            // Remember that another element was found, as well as the indexes of the string's start and end.
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == length) ? i + 1 : i;
        }
    }

    // Return either the found element or an empty string.
    return found > index ? substring(input, strIndex[0], strIndex[1] - strIndex[0]) : (char *)"";
}

void Utils::toLower(char *input)
{
    // Go through all characters and replace them with their lowercase version.
    for (int i = 0; i < strlen(input); i++)
        input[i] = tolower(input[i]);
}

void Utils::replace(char *input, char target, char replacement)
{
    // Go through all characters and replace it if it matches the target character.
    for (int i = 0; i < strlen(input); i++)
        if (input[i] == target)
            input[i] = replacement;
}

void Utils::makeSafename(char *str)
{
    // Get a separate pointer for the char array that iterates over the whole character array.
    // Our two pointers src and str will progress differently, src is used to iterate over
    // the input char array while str points to the next character to be written for our output.
    const char *src = str;

    // Skip all characters by moving the src pointer forward until the character array reached the end or
    // a non-whitespace character is encountered. This ignores all leading whitespaces of the character array.
    while (*src && isspace(*src))
        src++;

    // Go through the input char array with our separate pointer until it reached the zero terminator.
    while (*src)
    {
        // If the current character is not a whitespace or a whitespace and different from the previous character,
        // put the character into the position our str pointer is currently pointing at, replacing the character array on the fly.
        if (!isspace(*src) || *src != *(src - 1))
        {
            *str = *src;

            // Move the input character array pointer forward so it writes to the next position on the next iteration.
            str++;
        }

        // Move the separate pointer used to iterate over the input string forward.
        src++;
    }

    // Remove a whitespace at the end if one exists. Only one at most can exist there as we compacted consecutive whitespaces before.
    if (isspace(*(str - 1)))
        str--;

    // Append the null terminator that finishes the string at that position. This is important since we are modifying the input
    // character array on the fly and not doing this would cause the char array to have old letters from the input at the end.
    // e.g. "  hello  world  " turns "hello worldrld  " so we place a zero terminator after the "world" word.
    *str = '\0';
}

char *Utils::substring(char *input, int offset, int length)
{
    // Allocate a new character array with an extra character for the zero terminator.
    char *res = new char[length + 1];

    // Go from 0 to the length of the substring and add the character at that index + the offset to the new character array.
    for (int i = 0; i < length; i++)
        res[i] = *(input + offset + i);

    // Put the zero terminator at the end of the character array.
    res[length] = 0;

    return res;
}