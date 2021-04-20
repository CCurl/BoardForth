// ---------------------------------------------------------------------
// main.c
// ---------------------------------------------------------------------

#include "board.h"
#include "defs.h"

#pragma warning(disable:4996)

void parseLine(char *);
int isBYE = 0;

#ifdef __DEV_BOARD__
Serial pc(PA_9, PA_10, "pc", 19200);

void printSerial(const char *str) {
    pc.printf("%s", str);
}
#endif

void toTIB(int c) {
    if (c == 0) { return; }
    if (c == 8) {
        if (numTIB > 0) {
            numTIB--;
            TIB[numTIB] = (char)0;
        }
        return;
    }
    if (c == 13) {
        // printStringF("[%s]\r\n", TIB);
        parseLine(TIB);
        ok();
        numTIB = 0;
        TIB[numTIB] = (char)0;
        return;
    }
    if (TIB_SZ < numTIB) {
        printString("-TIB full-");
    }
    if (c ==  9) { c = 32; }
    if (c == 10) { c = 32; }
    if (32 <= c) {
        TIB[numTIB++] = c;
        TIB[numTIB] = (char)0;
        printStringF("%c", c);
    }
} 

#ifdef __DEV_BOARD__
void loop() {
    while (pc.readable()) {
        int c = pc.getc();
        toTIB(c);
    }
}
#else
void doHistory(char *l) {
    FILE *fp = fopen("history.txt", "at");
    if (fp) {
        fprintf(fp, "%s\r\n", l);
        fclose(fp);
    }
}

void loop() {
    ok();
    fgets(TIB, TIB_SZ, stdin);
    if (strcmp(TIB, "bye\n") == 0) {
        isBYE = 1;
        return;
    }
    doHistory(TIB);
    push((CELL)TIB);
    fPARSELINE();
}
#endif

void loadSource(const char *src) {
    parseLine((char *)src);
}

void loadSourceF(const char *fmt, ...) {
    char buf[96];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    parseLine(buf);
}

void loadBaseSystem() {
    // ***MAX 96***  ...
    loadSourceF(": ds $%lx ;", (long)&dict[0]);
    loadSourceF(": (h) $%lx ; : here (h) @ ;", (long)&HERE);
    loadSourceF(": (l) $%lx ; : last (l) @ ;", (long)&LAST);
    loadSourceF(": base $%lx ;", (long)&BASE);
    loadSourceF(": state $%lx ;", (long)&STATE);
    loadSourceF(": tib $%lx ;", (long)&TIB[0]);
    loadSourceF(": dsp $%lx ; : rsp $%lx ;", (long)&DSP, (long)&RSP);
    loadSourceF(": dstack $%lx ;", (long)&dstk[0]);
    loadSourceF(": rstack $%lx ;", (long)&rstk[0]);

    loadSource(PSTR(": hex $10 base ! ;"));
    loadSource(PSTR(": decimal #10 base ! ;"));
    loadSource(PSTR(": binary %10 base ! ;"));
    loadSource(PSTR(": +! tuck @ + swap ! ;"));
    loadSource(PSTR(": ?dup if- dup then ;"));
    loadSource(PSTR(": abs dup 0 < if 0 swap - then ;"));
    loadSource(PSTR(": min over over < if drop else nip then ;"));
    loadSource(PSTR(": max over over > if drop else nip then ;"));
    loadSource(PSTR(": between rot dup >r min max r> = ;"));
    loadSource(PSTR(": cr #13 emit #10 emit ;"));
    loadSource(PSTR(": . 0 num>str space type ;"));
    loadSource(PSTR(": allot here + (h) ! ;"));
    loadSource(PSTR(": .word cr dup . space dup >body . addr +"));
    loadSource(PSTR("    dup c@ . 1+ space count type ;"));
    loadSource(PSTR(": words last begin dup .word a@ while- drop ;"));
    // ***MAX 96***  ...
}

void ok() {
    printString(" ok. ");
    fDOTS();
    printString("\r\n");
}

void startUp() {
    printString("BoardForth v0.0.1 - Chris Curl\r\n");
    printString("Source: https://github.com/CCurl/BoardForth \r\n");
    printStringF("Dictionary size is: %d ($%04x) bytes. \r\n", (int)DICT_SZ, (int)DICT_SZ);
    vmInit();
    printString("Hello.");
}

int main()
{
    // Initialise the digital pin LED13 as an output
    #ifdef __DEV_BOARD__
    DigitalOut led(PC_13);
    #endif

    startUp();
    loadBaseSystem();
    loadUserWords();
    // push(0);
    // fDUMPDICT();
    int num = 0; 
    int x = 0;
    numTIB = 0;

    while (true) {
        loop();
        #ifdef __DEV_BOARD__
        num += 1;
        if (num > 499999) {
            num = 0;
            x = (x) ? 0 : 1;
            led = x;
        }
        #else
        if (isBYE) { break; }
        #endif
    }
}
