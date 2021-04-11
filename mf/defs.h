// ---------------------------------------------------------------------
// defs.h
// ---------------------------------------------------------------------
#ifndef __DEFS_H__
    
// #define __DEV_BOARD__

#ifdef __DEV_BOARD__
    // #include <Arduino.h>
    // #include <Keyboard.h>
    #include "mbed.h"
    #define __HAS_KEYBOARD__
    #define mySerial Serial
    // #define mySerial SerialUSB
    void printSerial(const char *);
    void loadSource(const char *source);
    #define DICT_SZ (24*1024)   // Boards with 32K SRAM
    // #define DICT_SZ (4 * 256)      // LEONARDO
    #define STK_SZ 16
    #define TIB_SZ 100
    #define ALLOC_SZ 16
    #define F(str) (char *)str
    #define PSTR(str) (char *)str
    #define strcmp_PF(str1, str2) strcmp(str1, str2)
#else
    #define DICT_SZ (128*1024)
    #define STK_SZ 32
    #define TIB_SZ 100
    #define ALLOC_SZ 32
    #define F(str) (char *)str
    #define PSTR(str) (char *)str
    #define strcmp_PF(str1, str2) strcmp(str1, str2)
    void loadSource(const char *source);
#endif


#ifndef __DEV_BOARD__
    #include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef void (*FP)();
typedef long  CELL;
typedef unsigned long  ulong;
typedef unsigned short WORD;
typedef unsigned char  BYTE;
typedef BYTE *ADDR;

#define CELL_SZ (sizeof(CELL))
#define WORD_SZ (2)
#define ADDR_SZ (sizeof(ADDR))

#define T dstk[DSP]
#define N dstk[DSP-1]
#define R rstk[RSP]

typedef struct {
    ADDR prev;
    BYTE flags;
    BYTE len;
    char name[32]; // not really 32 ... but we need a number
} DICT_T;

typedef struct {
    ADDR addr;
    BYTE available;
    WORD sz;
} ALLOC_T;

extern BYTE IR;
extern ADDR PC;
extern CELL DSP, RSP;
extern ADDR HERE, LAST;
extern CELL dstk[], rstk[];
extern CELL BASE, STATE;
extern FP prims[];
extern BYTE dict[];
extern ADDR toIn;
extern ADDR allocAddrBase;
extern ADDR allocCurFree;
extern CELL loopDepth;
extern char TIB[];
extern int numTIB;

void dumpDict();
void startUp();
void init_handlers();
void vmInit();
void push(CELL);
CELL pop();
void rpush(CELL);
CELL rpop();
void run(ADDR , CELL);
ADDR allocSpace(WORD);
void allocFree(ADDR);
BYTE getOpcode(char *);
ADDR align4(ADDR);
BYTE nextChar();
void is_hex(char *);
void is_decimal(char *);
void is_binary(char *);
void CCOMMA(BYTE v);
void WCOMMA(WORD v);
void COMMA(CELL);
void ACOMMA(ADDR);
void parseLine(char *);
void loadBaseSystem();
BYTE getOpcode(char *);
void allocFreeAll();
void printString(const char *);
void printStringF(const char *, ...);
void loadUserWords();
void ok();
void autoRun();
CELL cellAt(ADDR);
CELL wordAt(ADDR);
ADDR addrAt(ADDR);
void fDUMPDICT();

// ---------------------------------------------------------------------
/* NimbleText script for below (https://nimbletext.com/Live)
$once
// vvvvv - NimbleText generated - vvvvv
$each
#define OP_<%($0 + '               ').substring(0,13)%> $rownum     // $1
$once
// ------- NimbleText generated continues
$each
void f<%($0 + '();               ').substring(0,13)%>      // OP_$0
$once
// ^^^^^ - NimbleText generated - ^^^^^
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
// ------- NimbleText generated continues
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
void fCOM();             // OP_COM
void fBYE();             // OP_BYE
// ^^^^^ - NimbleText generated - ^^^^^

#endif // __DEFS_H__
