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
    ADDR XT;
    BYTE dictionaryId;
    BYTE flagsLen;
    BYTE name[20];
} DICT_T;

typedef struct {
    CELL autoRun;
    BYTE currentDictId;
    BYTE Reserved1;
    BYTE Reserved2;
    BYTE Reserved3;
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

//typedef struct {
//    CELL addr;
//    BYTE available;
//    WORD sz;
//} ALLOC_T;

typedef struct {
    char name[16];
    byte opcode;
    byte makeWord;
} OPCODE_T;

typedef struct {
    CELL from, to, i;
} LOOPINFO_T;

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
extern int loopDepth;

void dumpDict();
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
void printString(const char *str);
void printStringF(const char *fmt, ...);
void loadUserWords();
void ok();
void autoRun();
CELL cellAt(CELL);
CELL wordAt(CELL);
CELL addrAt(CELL);
void fDUMPCODE();
void fDUMPDICT();
void wordStore(CELL addr, CELL val);
void cellStore(CELL addr, CELL val);
void addrStore(CELL addr, CELL val);
void genOpcodeWords();

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
#define OP_NOOP         0 // noop

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
#define OP_FREE_74     'J' // j
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
#define OP_SQUOTE      'W' // s"
#define OP_SUB         'X' // -
#define OP_MULT        'Y' // *
#define OP_SLMOD       'Z' // /mod
#define OP_LSHIFT      '[' // <<

#define UNUSED_92      '\\' // free
#define UNUSED_93      ']' // free
#define UNUSED_94      '^' // free
#define UNUSED_95      '_' // free
#define UNUSED_96      '`' // free

#define OP_CLIT        'a' // cliteral
#define OP_WLIT        'b' // wliteral
#define OP_LIT         'c' // literal
#define OP_CFETCH      'd' // c@
#define OP_WFETCH      'e' // w@
#define OP_AFETCH      'f' // a@
#define OP_FETCH       'g' // @
#define OP_CSTORE      'h' // c!
#define OP_WSTORE      'i' // w!
#define OP_ASTORE      'j' // a!
#define OP_STORE       'k' // !
#define OP_CCOMMA      'l' // c,
#define OP_WCOMMA      'm' // w,
#define OP_COMMA       'n' // ,
#define OP_ACOMMA      'o' // a,
#define OP_CALL        'p' // call
#define OP_RET         'q' // exit
#define OP_JMP         'r' // jmp
#define OP_JMPZ        's' // jmpz
#define OP_JMPNZ       't' // jmpnz
#define OP_ONEMINUS    'u' // 1-
#define OP_ONEPLUS     'v' // 1+
#define OP_DUP         'w' // dup
#define OP_SWAP        'x' // swap
#define OP_DROP        'y' // drop
#define OP_OVER        'z' // over

#define UNUSED_123     '{' // free
#define UNUSED_124     '|' // free
#define UNUSED_125     '}' // free
#define UNUSED_126     '~' // free

#define OP_BYE        127 // bye

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
