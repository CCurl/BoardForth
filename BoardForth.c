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

#include "BoardForth.ino"
#include "compiler.ino"
#include "ports.ino"
#include "vm.ino"

int main() {
    setup();
    return 0;
}