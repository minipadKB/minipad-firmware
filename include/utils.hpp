#include <Arduino.h>

class Utils
{
public:
  static char *getArgumentAt(char *input, char delimiter, uint8_t index);
  static void toLower(char *input);
  static void replace(char *input, char target, char replacement);
  static void makeSafename(char* str);

private:
  static char *substring(char *input, int offset, int length);
};