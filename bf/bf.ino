#include "defs.h"

char buf[80];
int len;

void setup() {
    mySerial.begin(19200);
    while (!mySerial) {}
    while (mySerial.available()) {}
    printString("BoardForth v0.0.1 - Chris Curl\n");
    printString("Source: https://github.com/CCurl/BoardForth \n");
    printStringF("Dictionary size is: %d ($%04x) bytes.\n", (int)DICT_SZ, (int)DICT_SZ);
    vmInit();
    loadBaseSystem();
    loadUserWords();
    printStringF("Hello. here=%ld", sys->HERE);
    ok();
    len = 0;
}

void loop() {
  while (mySerial.available()) {
    char c = mySerial.read();
    if (c == 13) {
      buf[len] = (char)0;
      printString(buf);
      printString("\n");
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
  mySerial.print(str);
}

void loadSource(const PROGMEM char *source) {
    char buf[128];
    strcpy_PF(buf, source);
    printSerial(buf);
    printSerial("\n");
    parseLine(buf);
}
