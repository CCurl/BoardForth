#include "defs.h"

void setup() {
    while (!SERIAL) {}
    Serial.begin(19200);
    while (SERIAL.available()) {}
    printString("\nHello world!\n");
    vmInit();
}

void loop() {
}

void printSerial(const char *str) {
  SERIAL.print(str);
}
