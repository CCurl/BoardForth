#ifndef __VM_H__
#define __VM_H__
#include <stdarg.h>

// #define LOG_DEBUG
#define MAX_CYCLES 0

// Board            SRAM    Keyboard  Serial        
// ---------------- ------- -------- ------------
// ATMEGA 2560      8k      No        Serial
// Redboard_Turbo   32k     Yes       SerialUSB
// Seeduino XIAO    32k     Yes       Serial
// ESP8266          96k     Yes       Serial

#ifdef IS_PC
  #define MEM_SZ  64*1024
  #define DICT_SZ 64*1024
  #define ADDR_SZ 2
  #define LOW 0
  #define HIGH 255
  #define PROGMEM
  #define F(str) str
  #define strcpy_P strcpy
  #define SERIAL 1
#else
  #include <avr/pgmspace.h>
  // #include <Keyboard.h>
  #define MEM_8
  #define SERIAL Serial
  #define ADDR_SZ 2
  // #define SERIAL SerialUSB
  #define sendOutput(str) SERIAL.print(str)
  #define SERIAL_begin(baud) SERIAL.begin(baud)
  #define SERIAL_available(baud) SERIAL.available()
  #define SERIAL_read() SERIAL.read()
  #define sendOutput_Char(c) SERIAL.print(c)
#endif

#ifdef MEM_ESP8266
  #define MEM_SZ  80*1024
  #define DICT_SZ 32*1024
#endif

#ifdef MEM_64
  #define MEM_SZ  64*1024
  #define DICT_SZ 60*1024
#endif

#ifdef MEM_32
  #define MEM_SZ  32*1024
  #define DICT_SZ 28*1024
#endif

#ifdef MEM_8
  #define MEM_SZ   32*256
  #define DICT_SZ  24*256
#endif

#ifdef LOG_DEBUG
  #define DBG_LOG(str)  writePort_String(str)
  #define DBG_LOGF(...) writePort_StringF(__VA_ARGS__)
#else
  #define DBG_LOG(str) DoNothing(str)
  #define DBG_LOGF(...) DoNothingF(__VA_ARGS__)
#endif

#define FLASH(num) const PROGMEM char string_ ## num[]

typedef unsigned long ulong;
typedef unsigned short ushort;
#define byte unsigned char

#define CELL unsigned long
#define CELL_SZ 4
#define WORD unsigned short

#define STK_SZ 32
#define T dstk[DSP]
#define N dstk[DSP-1]
#define R rstk[RSP]

// Special STATEs
#define LOAD_HERE_LAST 0x100
#define LOAD_LINE      0x101

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
#define SLASHMOD     24     // /mod
#define LSHIFT       25     // <<
#define RSHIFT       26     // >>
#define AND          27     // and
#define OR           28     // or
#define XOR          29     // xor
#define NOT          30     // not
#define DTOR         31     // >r
#define RFETCH       32     // r@
#define RTOD         33     // r>
#define WDTFEED      34     // wdtfeed
#define GTHAN        35     // >
#define LTHAN        36     // <
#define EQUAL        37     // =
// END of NimbleText generated

// ***************************
// Ports
// ***************************
// Pins
#define PORT_PINS     0x10000
#define PORT_APINS    0x11000
// Input/Output
#define PORT_EMIT        0x20001
#define PORT_DOT         0x20002
#define PORT_COM_OPEN    0x28001
#define PORT_COM_IO      0x28002
#define PORT_FILE_OPEN   0x29001
#define PORT_FILE_IO     0x29002
// System variables
#define PORT_HERE        0x30001
#define PORT_LAST        0x30002
#define PORT_BASE        0x30003
#define PORT_STATE       0x30004
#define PORT_DSP         0x30005
#define PORT_DICT_SZ     0x30006
// ***************************

void push(CELL);
CELL pop();
void rpush(CELL);
CELL rpop();
void swap();

//void cComma();
//void wComma();
//void Comma();

void CCOMMA(CELL);
void WCOMMA(CELL);
void COMMA(CELL);
void addrCOMMA(CELL);
CELL addrAt(CELL);

//CELL cStore();
//CELL wStore();
//CELL Store();
void addrStore();

//void cFetch();
//void wFetch();
//void Fetch();

extern byte dict[];
extern CELL HERE;
extern CELL LAST;
extern CELL BASE;
extern CELL STATE;
extern byte DSP;
extern CELL dstk[];
void parseLine(char *);
void vm_init();
void ok();
void dotS();
void autoRun();
void runProgram(CELL start);
void readPort(CELL portNumber);
void writePort(CELL portNumber);
byte *vm_Alloc(CELL sz);
void vm_FreeAll();
void writePort_String(const char *str);
void writePort_StringF(const char *fmt, ...);
void DoNothing(const char *str);
void DoNothingF(const char *fmt, ...);
void dumpDSTK();
void char_in(char c);
#endif
