#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DICT_SZ (24*1024)
#define STK_SZ 32
#define TIB_SZ 0x80
#define ALLOC_SZ 32

typedef void (*FP)();
typedef unsigned long  CELL;
typedef unsigned short ADDR;
typedef unsigned short WORD;
typedef unsigned char  BYTE;

#define CELL_SZ (4)
#define WORD_SZ (2)
#define ADDR_SZ (2)

#define DIGITAL_PIN_BASE 0x10000000
#define ANALOG_PIN_BASE  0x20000000

#define T dstk[DSP]
#define N dstk[DSP-1]
#define R rstk[RSP]
#define F(s) s

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
    CELL RESERVED3;
    CELL HERE;
    CELL LAST;
    CELL BASE;
    CELL STATE;
} SYSVARS_T;

typedef struct {
    CELL addr;
    BYTE available;
    WORD sz;
} ALLOC_T;

#define ADDR_AUTORUN    (CELL_SZ*0)
#define ADDR_RES1       (CELL_SZ*1)
#define ADDR_RES2       (CELL_SZ*2)
#define ADDR_RES3       (CELL_SZ*3)
#define ADDR_HERE       (CELL_SZ*4)
#define ADDR_LAST       (CELL_SZ*5)
#define ADDR_BASE       (CELL_SZ*6)
#define ADDR_STATE      (CELL_SZ*7)
#define ADDR_HERE_BASE  (CELL_SZ*8)
#define ADDR_ALLOC_BASE (DICT_SZ - TIB_SZ - 1)

extern BYTE IR;
extern CELL PC;
extern int DSP, RSP;
extern CELL HERE, LAST;
extern CELL dstk[], rstk[];
extern CELL BASE, STATE;
extern FP prims[];
extern BYTE dict[];
extern SYSVARS_T *sys;

void push(CELL);
CELL pop();
void rpush(CELL);
CELL rpop();
void run(CELL, CELL);

CELL stringToDict(char *, CELL);
void CCOMMA(BYTE v);
void WCOMMA(WORD v);
void COMMA(CELL v);
void ACOMMA(ADDR v);

// vvvvv -- NimbleText generated -- vvvvv
#define OP_NOOP         0     // noop
#define OP_CLIT         1     // cliteral
#define OP_WLIT         2     // wliteral
#define OP_LIT          3     // literal
#define OP_CFETCH       4     // c@
#define OP_WFETCH       5     // w@
#define OP_AFETCH       6     // a@
#define OP_FETCH        7     // @
#define OP_CSTORE       8     // c!
#define OP_WSTORE       9     // w!
#define OP_ASTORE       10     // a!
#define OP_STORE        11     // !
#define OP_CCOMMA       12     // c,
#define OP_WCOMMA       13     // w,
#define OP_COMMA        14     // ,
#define OP_ACOMMA       15     // 1,
#define OP_CALL         16     // call
#define OP_RET          17     // ret
#define OP_JMP          18     // jmp
#define OP_JMPZ         19     // jmpz
#define OP_JMPNZ        20     // jmpnz
#define OP_ONEMINUS     21     // 1-
#define OP_ONEPLUS      22     // 1+
#define OP_DUP          23     // dup
#define OP_SWAP         24     // swap
#define OP_DROP         25     // drop
#define OP_OVER         26     // over
#define OP_ADD          27     // +
#define OP_SUB          28     // -
#define OP_MULT         29     // *
#define OP_SLMOD        30     // /mod
#define OP_LSHIFT       31     // <<
#define OP_RSHIFT       32     // >>
#define OP_AND          33     // and
#define OP_OR           34     // or
#define OP_XOR          35     // xor
#define OP_NOT          36     // not
#define OP_DTOR         37     // >r
#define OP_RFETCH       38     // r@
#define OP_RTOD         39     // r>
#define OP_EMIT         40     // emit
#define OP_DOT          41     // .
#define OP_DOTS         42     // .s
#define OP_DOTQUOTE     43     // ."
#define OP_PAREN        44     // (
#define OP_WDTFEED      45     // wdtfeed
#define OP_BREAK        46     // brk
#define OP_TIB          47     // tib
#define OP_NTIB         48     // #tib
#define OP_TOIN         49     // >in
#define OP_OPENBLOCK    50     // open-block
#define OP_FILECLOSE    51     // file-close
#define OP_FILEREAD     52     // file-read
#define OP_LOAD         53     // load
#define OP_THRU         54     // thru
#define OP_BASE         55     // base
#define OP_STATE        56     // state
#define OP_HERE         57     // (here)
#define OP_LAST         58     // (last)
#define OP_BYE          59     // bye
// ^^^^^ -- NimbleText generated -- ^^^^^

// vvvvv -- NimbleText generated -- vvvvv
void fNOOP();
void fCLIT();
void fWLIT();
void fLIT();
void fCFETCH();
void fWFETCH();
void fAFETCH();
void fFETCH();
void fCSTORE();
void fWSTORE();
void fASTORE();
void fSTORE();
void fCCOMMA();
void fWCOMMA();
void fCOMMA();
void fACOMMA();
void fCALL();
void fRET();
void fJMP();
void fJMPZ();
void fJMPNZ();
void fONEMINUS();
void fONEPLUS();
void fDUP();
void fSWAP();
void fDROP();
void fOVER();
void fADD();
void fSUB();
void fMULT();
void fSLMOD();
void fLSHIFT();
void fRSHIFT();
void fAND();
void fOR();
void fXOR();
void fNOT();
void fDTOR();
void fRFETCH();
void fRTOD();
void fEMIT();
void fDOT();
void fDOTS();
void fDOTQUOTE();
void fPAREN();
void fWDTFEED();
void fBREAK();
void fTIB();
void fNTIB();
void fTOIN();
void fOPENBLOCK();
void fFILECLOSE();
void fFILEREAD();
void fLOAD();
void fTHRU();
void fBASE();
void fSTATE();
void fHERE();
void fLAST();
void fBYE();
// ^^^^^ -- NimbleText generated -- ^^^^^

#endif // __DEFS_H__
