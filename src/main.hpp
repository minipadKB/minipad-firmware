
#include <Arduino.h>

void handleSerialInput();
int mapToRange400(int value, int min, int max);
void pressKey1();
void releaseKey1();
void pressKey2();
void releaseKey2();
String getValue(String data, int index);