#include "defs.h"

char buf[80];
int len;

void ok() {
    printSerial(" ok. ");
    fDOTS();
    printSerial("\n");
}

void setup() {
    SERIAL.begin(19200);
    while (!SERIAL) {}
    while (SERIAL.available()) {}
    printString("\nHello world!\n");
    vmInit();
    loadBaseSystem();
    loadUserWords();
    // dumpDict();
    ok();
    len = 0;
}

void loop() {
  while (SERIAL.available()) {
    char c = SERIAL.read();
    if (c == 13) {
      buf[len] = (char)0;
      printSerial(buf);
      printSerial("\n");
      parseLine(buf);
      len = 0;
      ok();
    } else {
      buf[len++] = c;
    }
  }
}

void printSerial(const char *str) {
  SERIAL.print(str);
}

void loadSource(const PROGMEM char *source) {
    char buf[128];
    strcpy_PF(buf, source);
    printSerial(buf);
    printSerial("\n");
    parseLine(buf);
}
