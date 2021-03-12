// ---------------------------------------------------------------------
// main.c
// ---------------------------------------------------------------------

//#include "mbed.h"
#include "defs.h"

void startUp();
void parseLine(char *);
int repl();
int isBYE = 0;

//Serial pc(PA_9, PA_10, "pc", 19200);

void printSerial(const char *str) {
    printf("%s", str);
}

void toTIB(int c) {
    if (c == 0) { return; }
    if (c == 13) {
        printStringF("[%s]\r\n", TIB);
        // parseLine(TIB);
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
    gets(TIB);
    if (strcmp(TIB, "bye") == 0) {
        isBYE = 1;
        return;
    }
    doHistory(TIB);
    push((CELL)TIB);
    fPARSELINE();
}
#endif

int main()
{
    // Initialise the digital pin LED13 as an output
    //DigitalOut led(PC_13);

    startUp();
    loadBaseSystem();
    loadUserWords();
    // push(0);
    // fDUMPDICT();
    int num = 0; 
    int x = 0;

    while (true) {
        loop();
        if (isBYE) { break; }
        num += 1;
        if (num > 499999) {
            num = 0;
            x = (x) ? 0 : 1;
            //led = x;
        }
    }
}
