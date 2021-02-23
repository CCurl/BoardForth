#define __DEV_BOARD__
#define SERIAL Serial

#include "defs.h"

void setup() {
    while (!SERIAL) {}
    Serial.begin(19200);
    while (SERIAL.available()) {}
    printString("\nHello world!\n");
}

void loop() {

}
