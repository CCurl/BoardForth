#ifndef __DEFS_H__
#define __DEFS_H__

#define _CRT_SECURE_NO_WARNINGS

#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef void (*FP)();
typedef long  CELL;
typedef unsigned long  UCELL;
typedef unsigned short ADDR;
typedef unsigned short WORD;
typedef unsigned char  BYTE;

#define CELL_SZ (4)
#define WORD_SZ (2)
#define ADDR_SZ (2)

#define T dstk[sys->DSP]
#define N dstk[sys->DSP-1]
#define R rstk[sys->RSP]

typedef struct {
    ADDR prev;
    BYTE flags;
    BYTE len;
    BYTE name[32]; // not really 32 ... but we need a number
} DICT_T;

typedef struct {
    CELL autoRun;
    CELL RESERVED1;
    CELL TIB;
    CELL TOIN;
    CELL HERE;
    CELL LAST;
    CELL BASE;
    CELL STATE;
    CELL DSTACK;
    CELL RSTACK;
    CELL DSP;
    CELL RSP;
} SYSVARS_T;

typedef struct {
    CELL addr;
    BYTE available;
    WORD sz;
} ALLOC_T;

#define ADDR_AUTORUN    (CELL_SZ*0)
#define ADDR_RES_1      (CELL_SZ*1)
#define ADDR_TIB        (CELL_SZ*2)
#define ADDR_TOIN       (CELL_SZ*3)
#define ADDR_HERE       (CELL_SZ*4)
#define ADDR_LAST       (CELL_SZ*5)
#define ADDR_BASE       (CELL_SZ*6)
#define ADDR_STATE      (CELL_SZ*7)
#define ADDR_DSTK       (CELL_SZ*8)
#define ADDR_RSTK       (CELL_SZ*9)
#define ADDR_DSP        (CELL_SZ*10)
#define ADDR_RSP        (CELL_SZ*11)

#define ADDR_HERE_BASE  (CELL_SZ*12)

extern BYTE IR;
extern CELL PC;
extern int DSP, RSP;
extern CELL HERE, LAST;
extern CELL *dstk, *rstk;
extern CELL BASE, STATE;
extern FP prims[];
extern BYTE dict[];
extern SYSVARS_T *sys;
extern CELL toIn;
extern CELL allocAddrBase;
extern CELL allocCurFree;
extern CELL loopDepth;
extern void dumpDict();


void vmInit();
void push(CELL);
CELL pop();
void rpush(CELL);
CELL rpop();
void run(CELL, CELL);
CELL allocSpace(int);
void allocFree(CELL);
CELL stringToDict(char *, CELL);
BYTE getOpcode(char *);
CELL align2(CELL);
CELL align4(CELL);
BYTE nextChar();
void is_hex(char *);
void is_decimal(char *);
void is_binary(char *);
void CCOMMA(BYTE v);
void WCOMMA(WORD v);
void COMMA(CELL v);
void ACOMMA(ADDR v);
void parseLine(char *);
void loadBaseSystem();
BYTE getOpcode(char *w);
void allocFreeAll();
void printString(const char *str);
void printStringF(const char *fmt, ...);
void loadUserWords();
void ok();
void autoRun();
CELL cellAt(CELL);
CELL wordAt(CELL);
CELL addrAt(CELL);
void fDUMPDICT();
void wordStore(CELL addr, CELL val);
void cellStore(CELL addr, CELL val);
void addrStore(CELL addr, CELL val);

// ---------------------------------------------------------------------
/* NimbleText script for below (https://nimbletext.com/Live)
$once
// vvvvv - NimbleText generated - vvvvv
$each
#define $1 $rownum // $4
$once 
// ^^^^^ - NimbleText generated - ^^^^^
void runProgram() {
    BYTE IR = *(PC++);
    while (1) {
        switch (IR) {
$each
            case $1:     // $4 (#$rownum)
                N += T; pop();
                break;
$once 
        }
    }
}
*/

// vvvvv - NimbleText generated - vvvvv
#define OP_NOOP          0 // noop
#define OP_CLIT          1 // cliteral
#define OP_WLIT          2 // wliteral
#define OP_LIT           3 // literal
#define OP_CFETCH        4 // c@
#define OP_WFETCH        5 // w@
#define OP_AFETCH        6 // a@
#define OP_FETCH         7 // @
#define OP_CSTORE        8 // c!
#define OP_WSTORE        9 // w!
#define OP_ASTORE       10 // a!
#define OP_STORE        11 // !
#define OP_CCOMMA       12 // c,
#define OP_WCOMMA       13 // w,
#define OP_COMMA        14 // ,
#define OP_ACOMMA       15 // a,
#define OP_CALL         16 // call
#define OP_RET          17 // exit
#define OP_JMP          18 // -n-
#define OP_JMPZ         19 // -n-
#define OP_JMPNZ        20 // -n-
#define OP_ONEMINUS     21 // 1-
#define OP_ONEPLUS      22 // 1+
#define OP_DUP          23 // dup
#define OP_SWAP         24 // swap
#define OP_DROP         25 // drop
#define OP_OVER         26 // over
#define OP_SQUOTE       27 // s"
#define OP_SUB          28 // -
#define OP_MULT         29 // *
#define OP_SLMOD        30 // /mod
#define OP_LSHIFT       31 // <<
#define OP_RSHIFT      ' ' // >>
#define OP_AND         '!' // and
#define OP_OR          '"' // or
#define OP_XOR         '#' // xor
#define OP_NOT         '$' // not
#define OP_DTOR        '%' // >r
#define OP_RFETCH      '&' // r@
#define OP_RTOD        '\'' // r>
#define OP_PAREN       '(' // (
#define OP_TYPE        ')' // type
#define OP_DOTS        '*' // .s
#define OP_ADD         '+' // +
#define OP_EMIT        ',' // emit
#define OP_WDTFEED     '-' // wdtfeed
#define OP_BREAK       '.' // brk
#define OP_CMOVE       '/' // cmove
#define OP_CMOVE2      '0' // cmove>
#define OP_FILL        '1' // fill
#define OP_OPENBLOCK   '2' // open-block
#define OP_FILECLOSE   '3' // file-close
#define OP_FILEREAD    '4' // file-read
#define OP_LOAD        '5' // load
#define OP_THRU        '6' // thru
#define OP_DO          '7' // do
#define OP_LOOP        '8' // loop
#define OP_LOOPP       '9' // loop+
#define OP_DEBUGGER    ':' // -n-
#define OP_PARSEWORD   ';' // parse-word
#define OP_LESS        '<' // parse-line
#define OP_GETXT       '=' // >body
#define OP_ALIGN2      '>' // align2
#define OP_ALIGN4      '?' // align4
#define OP_CREATE      '@' // create
#define OP_FIND        'A' // find
#define OP_NEXTWORD    'B' // next-word
#define OP_ISNUMBER    'C' // number?
#define OP_NJMPZ       'D' // -n-
#define OP_NJMPNZ      'E' // -n-
#define OP_PARSELINE   'F' // < 
#define OP_EQUALS      'G' // =
#define OP_GREATER     'H' // >
#define OP_I           'I' // i
#define OP_J           'J' // j
#define OP_INPUTPIN    'K' // input
#define OP_OUTPUTPIN   'L' // output
#define OP_DELAY       'M' // delay
#define OP_TICK        'N' // ms
#define OP_APINSTORE   'O' // 
#define OP_DPINSTORE   'P' // dp!
#define OP_APINFETCH   'Q' // ap@
#define OP_DPINFETCH   'R' // dp@
#define OP_MWFETCH     'S' // mw@
#define OP_MCSTORE     'T' // mc!
#define OP_NUM2STR     'U' // num>str
#define OP_COM         'V' // com
#define OP_BYE         'W' // bye
// ^^^^^ - NimbleText generated - ^^^^^


void fSTORE();           // OP_STORE
void fEMIT();            // OP_EMIT
void fTYPE();            // OP_TYPE
void fDOTS();            // OP_DOTS
void fPARSEWORD();       // OP_PARSEWORD
void fPARSELINE();       // OP_PARSELINE
void fCREATE();          // OP_CREATE
void fFIND();            // OP_FIND
void fNEXTWORD();        // OP_NEXTWORD
void fISNUMBER();        // OP_ISNUMBER
void fNUM2STR();         // OP_NUM2STR

#endif // __DEFS_H__
