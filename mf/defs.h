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
    #define SERIAL Serial
    // #define SERIAL SerialUSB
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
#define OP_NOOP          0     // NOOP
#define OP_CLIT          1     // CLITERAL
#define OP_WLIT          2     // WLITERAL
#define OP_LIT           3     // LITERAL
#define OP_CFETCH        4     // C@
#define OP_WFETCH        5     // W@
#define OP_AFETCH        6     // A@
#define OP_FETCH         7     // @
#define OP_CSTORE        8     // C!
#define OP_WSTORE        9     // W!
#define OP_ASTORE        10     // A!
#define OP_STORE         11     // !
#define OP_CCOMMA        12     // C,
#define OP_WCOMMA        13     // W,
#define OP_COMMA         14     // ,
#define OP_ACOMMA        15     // A,
#define OP_CALL          16     // CALL
#define OP_RET           17     // EXIT
#define OP_JMP           18     // -N-
#define OP_JMPZ          19     // -N-
#define OP_JMPNZ         20     // -N-
#define OP_ONEMINUS      21     // 1-
#define OP_ONEPLUS       22     // 1+
#define OP_DUP           23     // DUP
#define OP_SWAP          24     // SWAP
#define OP_DROP          25     // DROP
#define OP_OVER          26     // OVER
#define OP_ADD           27     // +
#define OP_SUB           28     // -
#define OP_MULT          29     // *
#define OP_SLMOD         30     // /MOD
#define OP_LSHIFT        31     // <<
#define OP_RSHIFT        32     // >>
#define OP_AND           33     // AND
#define OP_OR            34     // OR
#define OP_XOR           35     // XOR
#define OP_NOT           36     // NOT
#define OP_DTOR          37     // >R
#define OP_RFETCH        38     // R@
#define OP_RTOD          39     // R>
#define OP_EMIT          40     // EMIT
#define OP_TYPE          41     // type
#define OP_DOTS          42     // .S
#define OP_DOTQUOTE      43     // .\"
#define OP_PAREN         44     // (
#define OP_WDTFEED       45     // WDTFEED
#define OP_BREAK         46     // BRK
#define OP_CMOVE         47     // CMOVE
#define OP_CMOVE2        48     // CMOVE>
#define OP_FILL          49     // FILL
#define OP_OPENBLOCK     50     // OPEN-BLOCK
#define OP_FILECLOSE     51     // FILE-CLOSE
#define OP_FILEREAD      52     // FILE-READ
#define OP_LOAD          53     // LOAD
#define OP_THRU          54     // THRU
#define OP_DO            55     // DO
#define OP_LOOP          56     // LOOP
#define OP_LOOPP         57     // LOOP+
#define OP_UNUSED7       58     // -n-
#define OP_PARSEWORD     59     // PARSE-WORD
#define OP_PARSELINE     60     // PARSE-LINE
#define OP_GETXT         61     // >BODY
#define OP_ALIGN2        62     // ALIGN2
#define OP_ALIGN4        63     // ALIGN4
#define OP_CREATE        64     // CREATE
#define OP_FIND          65     // FIND
#define OP_NEXTWORD      66     // NEXT-WORD
#define OP_ISNUMBER      67     // NUMBER?
#define OP_NJMPZ         68     // -N-
#define OP_NJMPNZ        69     // -N-
#define OP_LESS          70     // <
#define OP_EQUALS        71     // =
#define OP_GREATER       72     // >
#define OP_I             73     // I
#define OP_J             74     // J
#define OP_INPUTPIN      75     // input
#define OP_OUTPUTPIN     76     // output
#define OP_DELAY         77     // MS
#define OP_TICK          78     // TICK
#define OP_APINSTORE     79     // 
#define OP_DPINSTORE     80     // dp!
#define OP_APINFETCH     81     // ap@
#define OP_DPINFETCH     82     // dp@
#define OP_MWFETCH       83     // mw@
#define OP_MCSTORE       84     // mc!
#define OP_NUM2STR       85     // num>str
#define OP_COM           86     // com
#define OP_BYE           87     // BYE
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
