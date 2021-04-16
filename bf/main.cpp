// main.cpp

#include "defs.h"
#pragma warning(disable: 4996)

void sysInit() {
    printString("BoardForth v0.0.1 - Chris Curl\r\n");
    printString("Source: https://github.com/CCurl/BoardForth \r\n");
    printStringF("Dictionary size is: %d ($%04x) bytes.\r\n", (int)DICT_SZ, (int)DICT_SZ);
    printStringF("Hello.");
    vmInit();
    loadBaseSystem();
    loadUserWords();
    ok();
}

#ifdef __DEV_BOARD__
char buf[80];
int len, iLed = 13;
ulong nextBlink = 0;
int ledState = LOW;

void setup() {
    mySerial.begin(19200);
    while (!mySerial) {}
    while (mySerial.available()) {}
    sysInit();
    len = 0;
    pinMode(iLed, OUTPUT);
}

void loop() {
    ulong curTm = millis();
    if (nextBlink < curTm) {
        ledState = (ledState == LOW) ? HIGH : LOW;
        digitalWrite(iLed, ledState);
        nextBlink = curTm + 777;
    }

  while (mySerial.available()) {
    char c = mySerial.read();
    if (c == 13) {
      buf[len] = (char)0;
      printString(" ");
      // printString("\r\n");
      parseLine(buf);
      len = 0;
      ok();
    } else {
      buf[len++] = c;
      char b[2]; b[0] = c; b[1] = 0;
      printString(b);
    }
  }
  autoRun();
}

void printSerial(const char *str) {
  mySerial.print(str);
}

void loadSource(const char *source) {
    char buf[128];
    strcpy_PF(buf, source);
    printSerial(buf);
    printSerial("\r\n");
    parseLine(buf);
}

#else

void loadSource(const char* src) {
    parseLine((char*)src);
}

void doHistory(char* l) {
    FILE* fp = fopen("history.txt", "at");
    if (fp) {
        fprintf(fp, "%s\n", l);
        fclose(fp);
    }
}

void rtrim(char* str) {
    int l = strlen(str);
    while ((l > 0) && (str[l - 1] < 33)) str[--l] = 0;
}

void writeDict(FILE *to) {
    to = to ? to : stdout;
    fprintf(to, "; WORDS: LAST=%ld", sys->LAST);
    fprintf(to, "\r\n  #   XT   d  f  l word");
    fprintf(to, "\r\n---- ---- -- -- -- -----------------");
    for (int i = sys->LAST - 1; 0 <= i; i--) {
        DICT_T* dp = &words[i];
        int fl = dp->flagsLen;
        fprintf(to, "\r\n%4d %04lx %2d %2d %2d %s", i,
            (CELL)dp->XT, (int)dp->dictionaryId, (fl >> 6), (fl & 0x1F), dp->name);
    }
}

void writeCode(FILE *to) {
    char x[32];
    int n = 0;
    fprintf(to, "\r\n; CODE: HERE=%04lx (%ld), FREE: %lu", sys->HERE, sys->HERE, (sys->RSTACK - sys->HERE));
    for (int i = 0; i < sys->HERE; i++) {
        if (i % 16 == 0) {
            if (n) { x[n] = 0; fprintf(to, " ; %s", x); }
            fprintf(to, "\r\n%04x:", i);
            n = 0;
        }
        BYTE b = dict[i];
        x[n++] = ((31 < b) && (b < 128)) ? b : '.';
        fprintf(to, " %02x", dict[i]);
    }
    for (int i = sys->HERE; i < DICT_SZ; i++) {
        if (i % 16 == 0) { break; }
        fprintf(to, "   ");
    }
    if (n) { x[n] = 0; fprintf(to, " ; %s", x); }
}

void repl() {
    char* tib = (char*)&dict[sys->TIB];
    while (1) {
        fgets(tib, TIB_SZ, stdin);
        rtrim(tib);
        if (strcmp(tib, "bye") == 0) return;
        if (strcmp(tib, "ZZ") == 0) return;
        doHistory(tib);
        push(sys->TIB);
        fPARSELINE();
        ok();
    }
}

int main() {
    sysInit();
    repl();
    FILE *fp = fopen("vm-dump.txt", "wb");
    if (fp) {
        writeCode(fp);
        fclose(fp);
    }
    // printStringF("\r\n");
}
#endif // __DEV_BOARD__
