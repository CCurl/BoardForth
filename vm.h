#ifndef __VM_H__
#define __VM_H__

#include <avr/pgmspace.h>

// #define ATMEGA_2560
// #define LEONARDO
#define REDBOARD_TURBO
// #define UNO_R3
// #define XIAO

#ifdef ATMEGA_2560
#define SERIAL Serial
#define MEM_SZ 8*1024
#define DICT_SZ 6*1024
#define ADDR_SZ 2
typedef unsigned long ulong;
typedef unsigned short ushort;
#endif

#ifdef LEONARDO
#define SERIAL Serial
#define DICT_SZ 13*128
#define MEM_SZ  20*128
#define ADDR_SZ 2
typedef unsigned long ulong;
typedef unsigned short ushort;
// #define HAS_KEYBOARD
#endif

#ifdef REDBOARD_TURBO
#define SERIAL SerialUSB
#define MEM_SZ 32*1024
#define DICT_SZ 28*1024
// #define NEEDS_ALIGN
#define ADDR_SZ 2
#endif

#ifdef UNO_R3
#define SERIAL Serial
#define MEM_SZ 8*256
#define DICT_SZ 3*256
#define ADDR_SZ 2
typedef unsigned long ulong;
typedef unsigned short ushort;
#endif

#ifdef XIAO
#define SERIAL Serial
#define MEM_SZ 32*1024
#define DICT_SZ 28*1024
#define ADDR_SZ 2
typedef unsigned long ulong;
typedef unsigned short ushort;
// #define HAS_KEYBOARD
#endif

#ifdef HAS_KEYBOARD
#include <Keyboard.h>
#endif

#define sendOutput(str) SERIAL.print(str)

#define FLASH(num) const PROGMEM char string_ ## num[]
#define CELL long
#define WORD short
#define byte unsigned char

#define STK_SZ 32
#define T dstk[DSP]
#define N dstk[DSP-1]
#define R rstk[RSP]

// NimbleText generated ...
#define NOOP          0     // nop
#define CLIT          1     // cliteral
#define WLIT          2     // wliteral
#define LIT           3     // literal
#define CFETCH        4     // c@
#define WFETCH        5     // w@
#define FETCH         6     // @
#define CSTORE        7     // c!
#define WSTORE        8     // w!
#define STORE         9     // !
#define CALL         10     // call
#define RET          11     // ;
#define JMP          12     // jmp
#define JMPZ         13     // jmpz
#define JMPNZ        14     // jmpnz
#define ONEMINUS     15     // 1-
#define ONEPLUS      16     // 1+
#define DUP          17     // dup
#define SWAP         18     // swap
#define DROP         19     // drop
#define OVER         20     // over
#define ADD          21     // +
#define SUB          22     // -
#define MULT         23     // *
#define DIV          24     // /
#define LSHIFT       25     // <<
#define RSHIFT       26     // >>
#define AND          27     // and
#define OR           28     // or
#define XOR          29     // xor
#define NOT          30     // not
#define EMIT         31     // emit
#define DOT          32     // .
#define DTOR         33     // >r
#define RFETCH       34     // r@
#define RTOD         35     // r>
// END of NimbleText generated

void push(CELL);
CELL pop();
void rpush(CELL);
CELL rpop();
void swap();

void cComma();
void wComma();
void lComma();

void CCOMMA(CELL);
void WCOMMA(CELL);
void COMMA(CELL);

CELL cStore();
CELL wStore();
CELL lStore();

void cFetch();
void wFetch();
void lFetch();

extern byte dict[];
extern CELL HERE;
extern CELL LAST;
extern CELL BASE;
extern CELL STATE;
void parseLine(char *);
void vm_init();
void ok();
void Dot(long num, int base, int width);
void autoRun();
void runProgram(CELL start);

#endif
