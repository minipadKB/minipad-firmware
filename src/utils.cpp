#include "utils.hpp"

char *Utils::getArgumentAt(char *input, char delimiter, uint8_t index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int length = strlen(input) - 1;

  for (int i = 0; i <= length && found <= index; i++)
  {
    if (input[i] == delimiter || i == length)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == length) ? i + 1 : i;
    }
  }

  return found > index ? substring(input, strIndex[0], strIndex[1] - strIndex[0]) : (char *)"";
}

char *Utils::substring(char *input, int offset, int length)
{
  char *res = new char[length + 1];

  for (int i = 0; i < length; i++)
    res[i] = *(input + offset + i);
  res[length] = 0;

  return res;
}