#include "board.h"
#ifndef __DEV_BOARD__
#include <windows.h>
#endif

#include "defs.h"
#pragma warning(disable: 4996)

void init() {
    printString("BoardForth v0.0.1 - Chris Curl\n");
    printString("Source: https://github.com/CCurl/BoardForth \n");
    printStringF("Dictionary size is: %d ($%04x) bytes.\n", (int)DICT_SZ, (int)DICT_SZ);
    printStringF("Hello.");
    vmInit();
    loadBaseSystem();
    loadUserWords();
    ok();
}

#ifdef __DEV_BOARD__
void startUp() {
    init();
}

void loop() {

}
#else

void loadSource(const char* src) {
    parseLine((char*)src);
}

void doHistory(char* l) {
    FILE* fp = fopen("..\\history.txt", "at");
    if (fp) {
        fprintf(fp, "%s\n", l);
        fclose(fp);
    }
}

void rtrim(char* str) {
    int l = strlen(str);
    while ((l > 0) && (str[l - 1] < 33)) str[--l] = 0;
}

void repl() {
    char* tib = (char*)&dict[sys->TIB];
    while (1) {
        fgets(tib, TIB_SZ, stdin);
        rtrim(tib);
        if (strcmp(tib, "bye") == 0) return;
        doHistory(tib);
        push(sys->TIB);
        fPARSELINE();
        ok();
    }
}

int main() {
    init();
    repl();
    FILE *fp = fopen("..\\f-dump.txt", "wt");
    if (fp) {
        push((CELL)fp);
        fDUMPDICT();
        fclose(fp);
    }
    // allocDump();
    // printStringF("\n");
}
#endif // __DEV_BOARD__
