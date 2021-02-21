#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DICT_SZ (28*1024)
#define STK_SZ 32
#define TIB_SZ 0x100
#define ALLOC_SZ 32

typedef void (*FP)();
typedef long  CELL;
typedef unsigned long  UCELL;
typedef unsigned short ADDR;
typedef unsigned short WORD;
typedef unsigned char  BYTE;

#define CELL_SZ (4)
#define WORD_SZ (2)
#define ADDR_SZ (2)

#define DIGITAL_PIN_BASE 0x10000000
#define ANALOG_PIN_BASE  0x20000000

#define T dstk[sys->DSP]
#define N dstk[sys->DSP-1]
#define R rstk[sys->RSP]
#define F(str) str

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

/* NimbleText script for below (https://nimbletext.com/Live)
$once
// vvvvv - NimbleText generated - vvvvv
$each
#define OP_<%($0 + '               ').substring(0,13)%> $rownum     // $1
$once
// ------- NimbleText generated continues
$each
void f<%($0 + '();               ').substring(0,13)%>      // OP_$0 (#$1)
$once
// ^^^^^ - NimbleText generated - ^^^^^
*/

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
#define OP_UNUSED1       47     // tib
#define OP_UNUSED2       48     // #tib
#define OP_UNUSED3       49     // >in
#define OP_OPENBLOCK     50     // open-block
#define OP_FILECLOSE     51     // file-close
#define OP_FILEREAD      52     // file-read
#define OP_LOAD          53     // load
#define OP_THRU          54     // thru
#define OP_UNUSED4       55     // base
#define OP_UNUSED5       56     // state
#define OP_UNUSED6       57     // (here)
#define OP_UNUSED7       58     // (last)
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
#define OP_LESS          70     // <
#define OP_EQUALS        71     // =
#define OP_GREATER       72     // >
#define OP_BYE           73     // bye
// ------- NimbleText generated continues
void fNOOP();            // OP_NOOP (#noop)
void fCLIT();            // OP_CLIT (#cliteral)
void fWLIT();            // OP_WLIT (#wliteral)
void fLIT();             // OP_LIT (#literal)
void fCFETCH();          // OP_CFETCH (#c@)
void fWFETCH();          // OP_WFETCH (#w@)
void fAFETCH();          // OP_AFETCH (#a@)
void fFETCH();           // OP_FETCH (#@)
void fCSTORE();          // OP_CSTORE (#c!)
void fWSTORE();          // OP_WSTORE (#w!)
void fASTORE();          // OP_ASTORE (#a!)
void fSTORE();           // OP_STORE (#!)
void fCCOMMA();          // OP_CCOMMA (#c,)
void fWCOMMA();          // OP_WCOMMA (#w,)
void fCOMMA();           // OP_COMMA (#,)
void fACOMMA();          // OP_ACOMMA (#a,)
void fCALL();            // OP_CALL (#call)
void fRET();             // OP_RET (#ret)
void fJMP();             // OP_JMP (#-n-)
void fJMPZ();            // OP_JMPZ (#-n-)
void fJMPNZ();           // OP_JMPNZ (#-n-)
void fONEMINUS();        // OP_ONEMINUS (#1-)
void fONEPLUS();         // OP_ONEPLUS (#1+)
void fDUP();             // OP_DUP (#dup)
void fSWAP();            // OP_SWAP (#swap)
void fDROP();            // OP_DROP (#drop)
void fOVER();            // OP_OVER (#over)
void fADD();             // OP_ADD (#+)
void fSUB();             // OP_SUB (#-)
void fMULT();            // OP_MULT (#*)
void fSLMOD();           // OP_SLMOD (#/mod)
void fLSHIFT();          // OP_LSHIFT (#<<)
void fRSHIFT();          // OP_RSHIFT (#>>)
void fAND();             // OP_AND (#and)
void fOR();              // OP_OR (#or)
void fXOR();             // OP_XOR (#xor)
void fNOT();             // OP_NOT (#not)
void fDTOR();            // OP_DTOR (#>r)
void fRFETCH();          // OP_RFETCH (#r@)
void fRTOD();            // OP_RTOD (#r>)
void fEMIT();            // OP_EMIT (#emit)
void fDOT();             // OP_DOT (#.)
void fDOTS();            // OP_DOTS (#.s)
void fDOTQUOTE();        // OP_DOTQUOTE (#.\")
void fPAREN();           // OP_PAREN (#()
void fWDTFEED();         // OP_WDTFEED (#wdtfeed)
void fBREAK();           // OP_BREAK (#brk)
void fNOOP();             // OP_TIB (#tib)
void fNOOP();            // OP_NTIB (##tib)
void fNOOP();            // OP_TOIN (#>in)
void fOPENBLOCK();       // OP_OPENBLOCK (#open-block)
void fFILECLOSE();       // OP_FILECLOSE (#file-close)
void fFILEREAD();        // OP_FILEREAD (#file-read)
void fLOAD();            // OP_LOAD (#load)
void fTHRU();            // OP_THRU (#thru)
void fNOOP();            // OP_BASE (#base)
void fNOOP();            // OP_STATE (#state)
void fNOOP();            // OP_HERE (#(here))
void fNOOP();            // OP_LAST (#(last))
void fPARSEWORD();       // OP_PARSEWORD (#parse-word)
void fPARSELINE();       // OP_PARSELINE (#parse-line)
void fGETXT();           // OP_GETXT (#get-xt)
void fALIGN2();          // OP_ALIGN2 (#align2)
void fALIGN4();          // OP_ALIGN4 (#align4)
void fCREATE();          // OP_CREATE (#create)
void fFIND();            // OP_FIND (#find)
void fNEXTWORD();        // OP_NEXTWORD (#next-word)
void fISNUMBER();        // OP_ISNUMBER (#number?)
void fNJMPZ();           // OP_NJMPZ (#-n-)
void fNJMPNZ();          // OP_NJMPNZ (#-n-)
void fLESS();            // OP_LESS (#<)
void fEQUALS();          // OP_EQUALS (#=)
void fGREATER();         // OP_GREATER (#>)
void fBYE();             // OP_BYE (#bye)
// ^^^^^ - NimbleText generated - ^^^^^

#endif // __DEFS_H__
