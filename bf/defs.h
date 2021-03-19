#ifndef __DEFS_H__
#define __DEFS_H__

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
CELL allocSpace(WORD);
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
#define OP_NOOP 0 // noop
#define OP_CLIT 1 // cliteral
#define OP_WLIT 2 // wliteral
#define OP_LIT 3 // literal
#define OP_CFETCH 4 // c@
#define OP_WFETCH 5 // w@
#define OP_AFETCH 6 // a@
#define OP_FETCH 7 // @
#define OP_CSTORE 8 // c!
#define OP_WSTORE 9 // w!
#define OP_ASTORE 10 // a!
#define OP_STORE 11 // !
#define OP_CCOMMA 12 // c,
#define OP_WCOMMA 13 // w,
#define OP_COMMA 14 // ,
#define OP_ACOMMA 15 // a,
#define OP_CALL 16 // call
#define OP_RET 17 // exit
#define OP_JMP 18 // -n-
#define OP_JMPZ 19 // -n-
#define OP_JMPNZ 20 // -n-
#define OP_ONEMINUS 21 // 1-
#define OP_ONEPLUS 22 // 1+
#define OP_DUP 23 // dup
#define OP_SWAP 24 // swap
#define OP_DROP 25 // drop
#define OP_OVER 26 // over
#define OP_ADD 27 // +
#define OP_SUB 28 // -
#define OP_MULT 29 // *
#define OP_SLMOD 30 // /mod
#define OP_LSHIFT 31 // <<
#define OP_RSHIFT 32 // >>
#define OP_AND 33 // and
#define OP_OR 34 // or
#define OP_XOR 35 // xor
#define OP_NOT 36 // not
#define OP_DTOR 37 // >r
#define OP_RFETCH 38 // r@
#define OP_RTOD 39 // r>
#define OP_EMIT 40 // emit
#define OP_TYPE 41 // type
#define OP_DOTS 42 // .s
#define OP_DOTQUOTE 43 // .\"
#define OP_PAREN 44 // (
#define OP_WDTFEED 45 // wdtfeed
#define OP_BREAK 46 // brk
#define OP_CMOVE 47 // cmove
#define OP_CMOVE2 48 // cmove>
#define OP_FILL 49 // fill
#define OP_OPENBLOCK 50 // open-block
#define OP_FILECLOSE 51 // file-close
#define OP_FILEREAD 52 // file-read
#define OP_LOAD 53 // load
#define OP_THRU 54 // thru
#define OP_DO 55 // do
#define OP_LOOP 56 // loop
#define OP_LOOPP 57 // loop+
#define OP_UNUSED7 58 // -n-
#define OP_PARSEWORD 59 // parse-word
#define OP_PARSELINE 60 // parse-line
#define OP_GETXT 61 // >body
#define OP_ALIGN2 62 // align2
#define OP_ALIGN4 63 // align4
#define OP_CREATE 64 // create
#define OP_FIND 65 // find
#define OP_NEXTWORD 66 // next-word
#define OP_ISNUMBER 67 // number?
#define OP_NJMPZ 68 // -n-
#define OP_NJMPNZ 69 // -n-
#define OP_LESS 70 // <
#define OP_EQUALS 71 // =
#define OP_GREATER 72 // >
#define OP_I 73 // i
#define OP_J 74 // j
#define OP_INPUTPIN 75 // input
#define OP_OUTPUTPIN 76 // output
#define OP_DELAY 77 // ms
#define OP_TICK 78 // ms
#define OP_APINSTORE 79 // 
#define OP_DPINSTORE 80 // dp!
#define OP_APINFETCH 81 // ap@
#define OP_DPINFETCH 82 // dp@
#define OP_MWFETCH 83 // mw@
#define OP_MCSTORE 84 // mc!
#define OP_NUM2STR 85 // num>str
#define OP_COM 86 // com
#define OP_BYE 87 // bye
// ^^^^^ - NimbleText generated - ^^^^^


void fNOOP();            // OP_NOOP
void fCLIT();            // OP_CLIT
void fWLIT();            // OP_WLIT
void fLIT();             // OP_LIT
void fCFETCH();          // OP_CFETCH
void fWFETCH();          // OP_WFETCH
void fAFETCH();          // OP_AFETCH
void fFETCH();           // OP_FETCH
void fCSTORE();          // OP_CSTORE
void fWSTORE();          // OP_WSTORE
void fASTORE();          // OP_ASTORE
void fSTORE();           // OP_STORE
void fCCOMMA();          // OP_CCOMMA
void fWCOMMA();          // OP_WCOMMA
void fCOMMA();           // OP_COMMA
void fACOMMA();          // OP_ACOMMA
void fCALL();            // OP_CALL
void fRET();             // OP_RET
void fJMP();             // OP_JMP
void fJMPZ();            // OP_JMPZ
void fJMPNZ();           // OP_JMPNZ
void fONEMINUS();        // OP_ONEMINUS
void fONEPLUS();         // OP_ONEPLUS
void fDUP();             // OP_DUP
void fSWAP();            // OP_SWAP
void fDROP();            // OP_DROP
void fOVER();            // OP_OVER
void fADD();             // OP_ADD
void fSUB();             // OP_SUB
void fMULT();            // OP_MULT
void fSLMOD();           // OP_SLMOD
void fLSHIFT();          // OP_LSHIFT
void fRSHIFT();          // OP_RSHIFT
void fAND();             // OP_AND
void fOR();              // OP_OR
void fXOR();             // OP_XOR
void fNOT();             // OP_NOT
void fDTOR();            // OP_DTOR
void fRFETCH();          // OP_RFETCH
void fRTOD();            // OP_RTOD
void fEMIT();            // OP_EMIT
void fTYPE();            // OP_TYPE
void fDOTS();            // OP_DOTS
void fDOTQUOTE();        // OP_DOTQUOTE
void fPAREN();           // OP_PAREN
void fWDTFEED();         // OP_WDTFEED
void fBREAK();           // OP_BREAK
void fCMOVE();           // OP_CMOVE
void fCMOVE2();          // OP_CMOVE2
void fFILL();            // OP_FILL
void fOPENBLOCK();       // OP_OPENBLOCK
void fFILECLOSE();       // OP_FILECLOSE
void fFILEREAD();        // OP_FILEREAD
void fLOAD();            // OP_LOAD
void fTHRU();            // OP_THRU
void fDO();              // OP_DO
void fLOOP();            // OP_LOOP
void fLOOPP();           // OP_LOOPP
void fUNUSED7();         // OP_UNUSED7
void fPARSEWORD();       // OP_PARSEWORD
void fPARSELINE();       // OP_PARSELINE
void fGETXT();           // OP_GETXT
void fALIGN2();          // OP_ALIGN2
void fALIGN4();          // OP_ALIGN4
void fCREATE();          // OP_CREATE
void fFIND();            // OP_FIND
void fNEXTWORD();        // OP_NEXTWORD
void fISNUMBER();        // OP_ISNUMBER
void fNJMPZ();           // OP_NJMPZ
void fNJMPNZ();          // OP_NJMPNZ
void fLESS();            // OP_LESS
void fEQUALS();          // OP_EQUALS
void fGREATER();         // OP_GREATER
void fI();               // OP_I
void fJ();               // OP_J
void fINPUTPIN();        // OP_INPUTPIN
void fOUTPUTPIN();       // OP_OUTPUTPIN
void fDELAY();           // OP_DELAY
void fTICK();            // OP_TICK
void fAPINSTORE();       // OP_APINSTORE
void fDPINSTORE();       // OP_DPINSTORE
void fAPINFETCH();       // OP_APINFETCH
void fDPINFETCH();       // OP_DPINFETCH
void fMWFETCH();         // OP_MWFETCH
void fMCSTORE();         // OP_MCSTORE
void fNUM2STR();         // OP_NUM2STR
void fBYE();             // OP_BYE

#endif // __DEFS_H__
