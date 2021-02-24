#include "defs.h"

char buf[80];
int len;

void setup() {
    SERIAL.begin(19200);
    while (!SERIAL) {}
    while (SERIAL.available()) {}
    printString("BoardForth v0.0.1 - Chris Curl\n");
    printString("Source: https://github.com/CCurl/BoardForth \n");
    printStringF("Dictionary size is: %d ($%04x) bytes.\n", (int)DICT_SZ, (int)DICT_SZ);
    printStringF("Hello.");
    vmInit();
    loadBaseSystem();
    loadUserWords();
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
  autoRun();
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
