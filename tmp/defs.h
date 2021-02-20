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
    CELL TOIN;
    CELL HERE;
    CELL LAST;
    CELL BASE;
    CELL STATE;
    CELL RESERVED2;
    CELL RESERVED3;
    CELL RESERVED4;
    CELL RESERVED5;
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
#define ADDR_RES_2      (CELL_SZ*8)
#define ADDR_RES_3      (CELL_SZ*9)
#define ADDR_RES_4      (CELL_SZ*10)
#define ADDR_RES_5      (CELL_SZ*11)

#define ADDR_HERE_BASE  (CELL_SZ*12)
#define ADDR_ALLOC_BASE (DICT_SZ-TIB_SZ-1)

extern BYTE IR;
extern CELL PC;
extern int DSP, RSP;
extern CELL HERE, LAST;
extern CELL dstk[], rstk[];
extern CELL BASE, STATE;
extern FP prims[];
extern BYTE dict[];
extern SYSVARS_T *sys;
extern CELL toIn;

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

// vvvvv - NimbleText generated - vvvvv
#define OP_NOOP          0     // noop
#define OP_CLIT          1     // cliteral
#define OP_WLIT          2     // wliteral
#define OP_LIT           3     // literal
#define OP_CFETCH        4     // c@
#define OP_WFETCH        5     // w@
#define OP_AFETCH        6     // a@
#define OP_FETCH         7     // @
#define OP_CSTORE        8     // c!
#define OP_WSTORE        9     // w!
#define OP_ASTORE        10     // a!
#define OP_STORE         11     // !
#define OP_CCOMMA        12     // c,
#define OP_WCOMMA        13     // w,
#define OP_COMMA         14     // ,
#define OP_ACOMMA        15     // a,
#define OP_CALL          16     // call
#define OP_RET           17     // ret
#define OP_JMP           18     // -n-
#define OP_JMPZ          19     // -n-
#define OP_JMPNZ         20     // -n-
#define OP_ONEMINUS      21     // 1-
#define OP_ONEPLUS       22     // 1+
#define OP_DUP           23     // dup
#define OP_SWAP          24     // swap
#define OP_DROP          25     // drop
#define OP_OVER          26     // over
#define OP_ADD           27     // +
#define OP_SUB           28     // -
#define OP_MULT          29     // *
#define OP_SLMOD         30     // /mod
#define OP_LSHIFT        31     // <<
#define OP_RSHIFT        32     // >>
#define OP_AND           33     // and
#define OP_OR            34     // or
#define OP_XOR           35     // xor
#define OP_NOT           36     // not
#define OP_DTOR          37     // >r
#define OP_RFETCH        38     // r@
#define OP_RTOD          39     // r>
#define OP_EMIT          40     // emit
#define OP_DOT           41     // .
#define OP_DOTS          42     // .s
#define OP_DOTQUOTE      43     // .\"
#define OP_PAREN         44     // (
#define OP_WDTFEED       45     // wdtfeed
#define OP_BREAK         46     // brk
#define OP_TIB           47     // tib
#define OP_NTIB          48     // #tib
#define OP_TOIN          49     // >in
#define OP_OPENBLOCK     50     // open-block
#define OP_FILECLOSE     51     // file-close
#define OP_FILEREAD      52     // file-read
#define OP_LOAD          53     // load
#define OP_THRU          54     // thru
#define OP_BASE          55     // base
#define OP_STATE         56     // state
#define OP_HERE          57     // (here)
#define OP_LAST          58     // (last)
#define OP_PARSEWORD     59     // parse-word
#define OP_PARSELINE     60     // parse-line
#define OP_GETXT         61     // get-xt
#define OP_ALIGN2        62     // align2
#define OP_ALIGN4        63     // align4
#define OP_CREATE        64     // create
#define OP_FIND          65     // find
#define OP_NEXTWORD      66     // next-word
#define OP_ISNUMBER      67     // number?
#define OP_NJMPZ         68     // -n-
#define OP_NJMPNZ        69     // -n-
#define OP_BYE           70     // bye
// ------- NimbleText generated continues
void fNOOP();            // OP_NOOP (#0, noop)
void fCLIT();            // OP_CLIT (#1, cliteral)
void fWLIT();            // OP_WLIT (#2, wliteral)
void fLIT();             // OP_LIT (#3, literal)
void fCFETCH();          // OP_CFETCH (#4, c@)
void fWFETCH();          // OP_WFETCH (#5, w@)
void fAFETCH();          // OP_AFETCH (#6, a@)
void fFETCH();           // OP_FETCH (#7, @)
void fCSTORE();          // OP_CSTORE (#8, c!)
void fWSTORE();          // OP_WSTORE (#9, w!)
void fASTORE();          // OP_ASTORE (#10, a!)
void fSTORE();           // OP_STORE (#11, !)
void fCCOMMA();          // OP_CCOMMA (#12, c,)
void fWCOMMA();          // OP_WCOMMA (#13, w,)
void fCOMMA();           // OP_COMMA (#14, ,)
void fACOMMA();          // OP_ACOMMA (#15, a,)
void fCALL();            // OP_CALL (#16, call)
void fRET();             // OP_RET (#17, ret)
void fJMP();             // OP_JMP (#18, -n-)
void fJMPZ();            // OP_JMPZ (#19, -n-)
void fJMPNZ();           // OP_JMPNZ (#20, -n-)
void fONEMINUS();        // OP_ONEMINUS (#21, 1-)
void fONEPLUS();         // OP_ONEPLUS (#22, 1+)
void fDUP();             // OP_DUP (#23, dup)
void fSWAP();            // OP_SWAP (#24, swap)
void fDROP();            // OP_DROP (#25, drop)
void fOVER();            // OP_OVER (#26, over)
void fADD();             // OP_ADD (#27, +)
void fSUB();             // OP_SUB (#28, -)
void fMULT();            // OP_MULT (#29, *)
void fSLMOD();           // OP_SLMOD (#30, /mod)
void fLSHIFT();          // OP_LSHIFT (#31, <<)
void fRSHIFT();          // OP_RSHIFT (#32, >>)
void fAND();             // OP_AND (#33, and)
void fOR();              // OP_OR (#34, or)
void fXOR();             // OP_XOR (#35, xor)
void fNOT();             // OP_NOT (#36, not)
void fDTOR();            // OP_DTOR (#37, >r)
void fRFETCH();          // OP_RFETCH (#38, r@)
void fRTOD();            // OP_RTOD (#39, r>)
void fEMIT();            // OP_EMIT (#40, emit)
void fDOT();             // OP_DOT (#41, .)
void fDOTS();            // OP_DOTS (#42, .s)
void fDOTQUOTE();        // OP_DOTQUOTE (#43, .\")
void fPAREN();           // OP_PAREN (#44, ()
void fWDTFEED();         // OP_WDTFEED (#45, wdtfeed)
void fBREAK();           // OP_BREAK (#46, brk)
void fTIB();             // OP_TIB (#47, tib)
void fNTIB();            // OP_NTIB (#48, #tib)
void fTOIN();            // OP_TOIN (#49, >in)
void fOPENBLOCK();       // OP_OPENBLOCK (#50, open-block)
void fFILECLOSE();       // OP_FILECLOSE (#51, file-close)
void fFILEREAD();        // OP_FILEREAD (#52, file-read)
void fLOAD();            // OP_LOAD (#53, load)
void fTHRU();            // OP_THRU (#54, thru)
void fBASE();            // OP_BASE (#55, base)
void fSTATE();           // OP_STATE (#56, state)
void fHERE();            // OP_HERE (#57, (here))
void fLAST();            // OP_LAST (#58, (last))
void fPARSEWORD();       // OP_PARSEWORD (#59, parse-word)
void fPARSELINE();       // OP_PARSELINE (#60, parse-line)
void fGETXT();           // OP_GETXT (#61, get-xt)
void fALIGN2();          // OP_ALIGN2 (#62, align2)
void fALIGN4();          // OP_ALIGN4 (#63, align4)
void fCREATE();          // OP_CREATE (#64, create)
void fFIND();            // OP_FIND (#65, find)
void fNEXTWORD();        // OP_NEXTWORD (#66, next-word)
void fISNUMBER();        // OP_ISNUMBER (#67, number?)
void fNJMPZ();           // OP_NJMPZ (#68, -n-)
void fNJMPNZ();          // OP_NJMPNZ (#69, -n-)
void fBYE();             // OP_BYE (#70, bye)
// ^^^^^ - NimbleText generated - ^^^^^

#endif // __DEFS_H__
