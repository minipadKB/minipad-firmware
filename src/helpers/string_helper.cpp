#include <Arduino.h>
#include "helpers/string_helper.hpp"

void getArgumentAt(const char* input, char delimiter, uint8_t index, char* output)
{
    // Remember the amount of found elements, start and end index of the current one and the total length.
    uint8_t found = 0;
    size_t start = 0;
    size_t length = strlen(input);

    // Go through all characters and count the delimiters until the desired index was reached.
    for (size_t i = 0; i <= length; i++)
    {
        // Check if a delimiter was found.
        if (i == length || input[i] == delimiter)
        {
           // If we found the desired index, fill the specified output buffer with the argument
           // as the substring of the input if the element was found.
            if (found == index) {
                memcpy(output, input + start, i - start);
                output[i - start] = '\0';
                return;
            }

            // Otherwise, increment the found count and update the start index.
            found++;
            start = i + 1;
        }
    }

    // If not enough elements for the desired index were found, set the output buffer to an empty string.
    output[0] = '\0';
}

void StringHelper::toLower(char *input)
{
    // Go through all characters and replace them with their lowercase version.
    for (size_t i = 0; i < strlen(input); i++)
        input[i] = tolower(input[i]);
}

void StringHelper::replace(char *input, char target, char replacement)
{
    // Go through all characters and replace it if it matches the target character.
    for (size_t i = 0; i < strlen(input); i++)
        if (input[i] == target)
            input[i] = replacement;
}

void StringHelper::makeSafename(char *str)
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
