#include <stdio.h>
#include <string.h>

#define IS_PC

void SERIAL_begin(int baud) {
}

int SERIAL_available() {
    return 0;
}

unsigned char SERIAL_read() {
    return 0;
}

void sendOutput_Char(char c) {
    printf("%c", c);
}

void sendOutput_Num(long num) {
    printf(" %d", num);
}

void sendOutput(const char *str) {
    printf("%s", str);
}

unsigned long pgm_read_word(const char * const* addr) {
    return (unsigned long)*addr;
}

unsigned long pgm_read_dword(const char * const* addr) {
    return (unsigned long)*addr;
}

#include "../BoardForth.ino"
#include "../compiler.ino"
#include "../ports.ino"
#include "../vm.ino"

void doHistory(char *str) {
    FILE *fp = fopen("history.txt", "at");
    if (fp) {
        fprintf(fp, "%s\n", str);
        fclose(fp);
    }
}

void load() {
    long num = pop();
    char fn[32];
    sprintf(fn, "block-%03d.fs", num);
    printf("opening [%s]", fn);
    FILE *fp = fopen(fn, "rt");
    if (fp) {
        char buf[256];
        while (1) {
            if (fgets(buf, 256, fp) == buf) {
                printf(".");
                parseLine(buf);
            } else {
                break;
            }
        }
        fclose(fp);
    }
}

void repl() {
    char buf[128];
    while (1) {
        printf(" ok.\n");
        gets(buf);
        if (strcmp(buf, "bye") == 0) return;
        if (strcmp(buf, "load") == 0) {
            load();
        } else {
            doHistory(buf);
            parseLine(buf);
        }
    }
}

int main() {
    setup();
    repl();
    return 0;
}
