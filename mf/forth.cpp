// ---------------------------------------------------------------------
// forth.c
// ---------------------------------------------------------------------

#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef __DEV_BOARD__
Serial pc(PA_9, PA_10, "pc", 19200)

void printSerial(const char* str) {
    pc.printf("%s", str);
}
#endif

typedef void (*FP)();
typedef long CELL;
typedef ulong UCELL;
typedef unsigned short WORD;
typedef unsigned char  BYTE;
typedef BYTE* ADDR;

#define CELL_SZ (4)
#define WORD_SZ (2)
#define ADDR_SZ (sizeof(ADDR))

#define T dstk[DSP]
#define N dstk[DSP-1]
#define R rstk[RSP]
#define A ((ADDR)dstk[DSP])
#define DROP1 DSP = ((0 < DSP) ? DSP-1 : 0)
#define DROP2 DROP1; DROP1

typedef struct {
    ADDR prev;
    ADDR XT;
    BYTE flags;
    BYTE len;
    char name[32]; // not really 32 ... but we need a number
} DICT_T;

typedef struct {
    ADDR addr;
    BYTE available;
    WORD sz;
} ALLOC_T;

void push(CELL);
CELL pop();
void rpush(CELL);
CELL rpop();
void allocFreeAll();
void printString(const char*);
void printStringF(const char*, ...);
void ok();
CELL cellAt(ADDR);
CELL wordAt(ADDR);
ADDR addrAt(ADDR);
void doParse(char sep);
int doNumber(const char *);
void cellStore(ADDR addr, CELL val);
void wordStore(ADDR addr, CELL val);
void doParseWord();
void doCreate(const char* name);
void doSlMod();
void doUSlMod();
void doType();
void doDotS();

BYTE IR;
ADDR PC;
ADDR HERE;
ADDR LAST;
BYTE dict[DICT_SZ];
char TIBBuf[TIB_SZ];
char *TIB = TIBBuf;
char *toIN, *TIBEnd;
CELL BASE, STATE, DSP, RSP;
CELL loopSTK[12];
CELL dstk[STK_SZ+1];
CELL rstk[STK_SZ+1];
CELL loopDepth;
ALLOC_T allocTbl[ALLOC_SZ];
int num_alloced = 0, numTIB = 0;
ADDR allocAddrBase, allocCurFree;
FP prims[256];
int lastWasCall = 0;
int isBYE = 0;

#ifndef __DEV_BOARD__
#pragma warning(disable:4996)
int digitalRead(int pin) { return 0; }
int analogRead(int pin) { return 0; }
void digitalWrite(int pin, int val) {}
void analogWrite(int pin, int val) {}
void delay(int ms) { Sleep(ms); }
long millis() { return GetTickCount(); }
#endif

#define Y(op, code) X(#op, op, code)

#define OPCODES \
    X("NOOP", NOOP, ) \
    X("DUP", DUP, push(T)) \
    X("SWAP", SWAP, CELL x = T; T = N; N = x) \
    X("DROP", DROP, DROP1) \
    X("OVER", OVER, push(N)) \
    X("c@", CFETCH, T = (BYTE)*A) \
    X("w@", WFETCH, T = wordAt(A)) \
    X("@",  FETCH,  T = cellAt(A)) \
    X("a@", AFETCH, (ADDR_SZ == 2) ? fWFETCH() : fFETCH()) \
    X("c!", CSTORE, *A = (N & 0xFF); DROP2) \
    X("!",  STORE,  cellStore(A, N); DROP2) \
    X("w!", WSTORE, wordStore(A, N); DROP2) \
    X("a!", ASTORE, (ADDR_SZ == 2) ? wordStore(A, N) : cellStore(A, N); DROP2) \
    X("+", ADD, N += T; pop()) \
    X("-", SUB, N -= T; pop()) \
    X("*", MULT, N *= T; pop()) \
    X("/MOD", SLMOD, doSlMod()) \
    X("U/MOD", USLMOD, doUSlMod()) \
    X("2/", RSHIFT, T = T >> 1) \
    X("2*", LSHIFT, T = T << 1) \
    X("1-", ONEMINUS, --T) \
    X("1+", ONEPLUS, ++T) \
    X(".S", DOTS, doDotS()) \
    X("AND", AND, N &= T; pop()) \
    X("OR", OR, N |= T; pop()) \
    X("XOR", XOR, N ^= T; pop()) \
    X("<", LESS, N = (N < T) ? 1 : 0; pop()) \
    X("=", EQUALS, N = (N == T) ? 1 : 0; pop()) \
    X("0=", NOT, T = (T == 0) ? -1 : 0) \
    X(">", GREATER, N = (N > T) ? 1 : 0; pop()) \
    X(">r", DTOR, rpush(pop())) \
    X("r@", RFETCH, push(R)) \
    X("r>", RTOD, push(rpop())) \
    X("(", PAREN, ) \
    X("TYPE", TYPE, doType() ) \
    X("EMIT", EMIT, printStringF("%c", (char)pop())) \
    X("WDTFEED", WDTFEED, ) \
    X("BREAK", BREAK, ) \
    X("OPENBLOCK", OPENBLOCK, ) \
    X("FILECLOSE", FILECLOSE, ) \
    X("FILEREAD", FILEREAD, ) \
    X("LOAD", LOAD, ) \
    X("THRU", THRU, ) \
    X("DO", DO, ) \
    X("LOOP", LOOP, ) \
    X("LOOPP", LOOPP, ) \
    X("DEBUGGER", DEBUGGER, ) \
    X("PARSEWORD", PARSEWORD, doParseWord()) \
    X("NUMBER?", ISNUMBER, doNumber((char *)pop());) \
    X("PARSELINE", PARSELINE, doParse(' ')) \
    X("I", I, ) \
    X("J", J, ) \
    X("INPUT-PIN", INPUT_PIN, DROP1) \
    X("INPUT-PULLUP", INPUT_PULLUP, DROP1) \
    X("INPUT-PULLDOWN", INPUT_PULLDOWN, DROP1) \
    X("OUTPUT-PIN", OUTPUT_PIN, DROP1) \
    X("MS", DELAY, delay(pop())) \
    X("TICK", TICK, push(millis())) \
    X("ap!", APIN_STORE, DROP2 ) \
    X("dp!", DPIN_STORE, DROP2 ) \
    X("ap@", APIN_FETCH, DROP1 ) \
    X("dp@", DPIN_FETCH, DROP1 ) \
    X("COM", COM, T = ~T ) \
    X("SQUOTE", SQUOTE, ) \
    X("C,", CCOMMA, *(HERE++) = (BYTE)pop()) \
    X("W,", WCOMMA, push((CELL)HERE); fWSTORE(); HERE += WORD_SZ) \
    X(",", COMMA, push((CELL)HERE); fSTORE(); HERE += CELL_SZ) \
    X("A,", ACOMMA, (ADDR_SZ == 2) ? fWCOMMA() : fCOMMA()) \
    X("CALL", CALL, rpush((CELL)PC + ADDR_SZ); PC = addrAt(PC)) \
    X("RET", RET, PC = (ADDR)rpop()) \
    X("JMP", JMP, PC = addrAt(PC)) \
    X("JMPZ", JMPZ, PC = (T == 0)? addrAt(PC) : PC + ADDR_SZ; pop()) \
    X("JMPNZ", JMPNZ, PC = (T != 0)? addrAt(PC) : PC + ADDR_SZ; pop()) \
    X("NJMPZ",  NJMPZ,  PC = (T == 0) ? addrAt(PC) : PC + ADDR_SZ) \
    X("NJMPNZ", NJMPNZ, PC = (T != 0) ? addrAt(PC) : PC + ADDR_SZ) \
    Y(CLIT, push(*(PC++))) \
    Y(WLIT, push(wordAt(PC)); PC += WORD_SZ) \
    Y(LIT, push(cellAt(PC)); PC += CELL_SZ) \
    X("BYE", BYE, isBYE = 1) \

#define X(name, op, code) OP_ ## op,
typedef enum {
    OPCODES
} OPCODE_T;

#undef X
#define X(name, op, code) void f ## op() { code; }
OPCODES

#undef X
#define X(name, op, code) prims[OP_ ## op] = f ## op;
void init_handlers() {
    for (int i = 0; i < 256; i++) {
        prims[i] = 0;
    }
    OPCODES
}

#undef X
#define X(name, op, code) if (strcmpi(w, name) == 0) { return OP_ ## op; }
BYTE getOpcode(char* w) {
    OPCODES
        return 0xFF;
}

#undef X

void run(ADDR start, CELL max_cycles) {
    PC = start;
    // printStringF("\r\nrun: %d (%04lx), %d cycles ... ", PC, PC, max_cycles);
    while (1) {
        OPCODE_T IR = (OPCODE_T)*(PC++);
        if (IR == OP_BYE) { return; }
        if (IR == OP_RET) {
            if (RSP < 1) { return; }
            PC = (ADDR)rpop();
        } else if (prims[IR]) {
            prims[IR]();
        } else {
            printStringF("-unknown opcode: %d ($%02x) at %04lx-", IR, IR, PC-1);
            throw(2);
        }
        if (max_cycles) {
            if (--max_cycles < 1) { return; }
        }
    }
}

void push(CELL v) {
    DSP = (DSP < STK_SZ) ? DSP + 1 : STK_SZ;
    T = v;
}
CELL pop() {
    DSP = (DSP > 0) ? DSP - 1 : 0;
    return dstk[DSP + 1];
}

void rpush(CELL v) {
    RSP = (RSP < STK_SZ) ? RSP + 1 : STK_SZ;
    R = v;
}
CELL rpop() {
    RSP = (RSP > 0) ? RSP - 1 : 0;
    return rstk[RSP + 1];
}

void doDotS() {
    printString("(");
    for (int i = 1; i <= DSP; i++) {
        printStringF(" #%d/$%lx", dstk[i], dstk[i]);
    }
    printString(" )");
}

void doSlMod() {
    CELL x = N, y = T;
    if (y) {
        N = x % y;
        T = x / y;
    }
    else {
        printString("-divide by 0-");
        throw(3);
    }
}

void doUSlMod() {
    UCELL x = N, y = T;
    if (y) {
        N = x % y;
        T = x / y;
    }
    else {
        printString("-divide by 0-");
        throw(3);
    }
}

void doType() {
    CELL l = pop();
    ADDR a = (ADDR)pop();
    char x[2];
    x[1] = 0;
    for (int i = 0; i < l; i++) {
        x[0] = *(a++);
        printString(x);
    }
}

ADDR align4(ADDR x) {
    CELL y = (CELL)x;
    while (y % 4) { ++y; }
    return (ADDR)y;
}

void doCreate(const char *name) {
    HERE = align4(HERE);
    // printStringF("\n-define [%s] at %ld (%lx)", name, HERE, HERE);

    DICT_T* dp = (DICT_T*)HERE;
    dp->prev = (ADDR)LAST;
    dp->flags = 0;
    dp->len = (BYTE)strlen(name);
    strcpy(dp->name, name);
    LAST = HERE;
    HERE += (ADDR_SZ*2) + dp->len + 3;
    dp->XT = HERE;
    // printStringF(",XT:%lx (HERE=%lx)-", dp->XT, HERE);
}

int matches(char ch, char sep) {
    if (ch == sep) { return 1; }
    if ((sep == ' ') && (ch < sep)) { return 1; }
    return 0;
}

CELL getNextWord(char *to, char sep) {
    while (*toIN && matches(*toIN, sep)) {
        ++toIN;
    }
    CELL len = 0;
    while (*toIN && !matches(*toIN, sep)) {
        ++len;
        *(to++) = *(toIN++);
    }
    *to = 0;
    return len;
}

void doParse(char sep) {
    toIN = (char*)pop();
    TIBEnd = toIN + strlen(toIN);
    try {
        while (1) {
            char* w = (char*)HERE + 0x100;
            char* wp = w;
            CELL len = getNextWord(w, sep);
            if (len == 0) { return; }
            push((CELL)w);
            doParseWord();
        }
    }
    catch (...) {
        printString("error caught");
    }
}

void vmInit() {
    init_handlers();
    HERE = &dict[0];
    LAST = 0;
    BASE = 10;
    STATE = 0;
    DSP = 0;
    RSP = 0;
    TIB = TIBBuf;
    TIBEnd = TIBBuf;
    allocAddrBase = &dict[DICT_SZ];
    allocCurFree = allocAddrBase;
    allocAddrBase = allocCurFree;
    allocFreeAll();
    loopDepth = 0;
}


void autoRun() {
    ADDR addr = addrAt(&dict[0]);
    if (addr) {
        run(addr, 0);
    }
}

// ---------------------------------------------------------------------
void printString(const char *str) {
    #ifdef __DEV_BOARD__
        printSerial(str);
    #else
        printf("%s", str);
    #endif
}

// ---------------------------------------------------------------------
void printStringF(const char *fmt, ...) {
    char buf[96];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    printString(buf);
}

CELL cellAt(ADDR loc) {
    return (*(loc+3) << 24) + (*(loc+2) << 16) + (*(loc+1) <<  8) + *(loc);
}
CELL wordAt(ADDR loc) {
    return (*(loc+1) <<  8) + *(loc);
}
ADDR addrAt(ADDR loc) {         // opcode #16
    return (ADDR_SZ == 2) ? (ADDR)wordAt(loc) : (ADDR)cellAt(loc);
}

void wordStore(ADDR addr, CELL val) {
    *(addr)   = (val & 0xFF);
    *(addr+1) = (val >>  8) & 0xFF;
}
void cellStore(ADDR addr, CELL val) {
    *(addr)   = (val & 0xFF);
    *(addr+1) = (val >>  8) & 0xFF;
    *(addr+2) = (val >> 16) & 0xFF;
    *(addr+3) = (val >> 24) & 0xFF;
}

#pragma region allocation
void allocDump() {
    printStringF("\r\nAlloc table (sz %d, %d used)", ALLOC_SZ, num_alloced);
    printString("\r\n-------------------------------");
    for (int i = 0; i < num_alloced; i++) {
        printStringF("\r\n%2d %04lx %4d %s", i, allocTbl[i].addr, (int)allocTbl[i].sz, (int)allocTbl[i].available ? "available" : "in-use");
    }
}

int allocFind(ADDR addr) {
    for (int i = 0; i < num_alloced; i++) {
        if (allocTbl[i].addr == addr) return i;
    }
    return -1;
}

void allocFree(ADDR addr) {
    // printStringF("-allocFree:%d-", (int)addr);
    int x = allocFind(addr);
    if (x >= 0) {
        // printStringF("-found:%d-", (int)x);
        allocTbl[x].available = 1;
        if ((x+1) == num_alloced) { -- num_alloced; }
        if (num_alloced == 0) { allocCurFree = allocAddrBase; }
    }
}

void allocFreeAll() {
    allocCurFree = allocAddrBase;
    for (int i = 0; i < ALLOC_SZ; i++) allocTbl[i].available = 1;
    num_alloced = 0;
}

int allocFindAvailable(WORD sz) {
    // allocDump();
    for (int i = 0; i < num_alloced; i++) {
        if ((allocTbl[i].available) && (allocTbl[i].sz >= sz)) return i;
    }
    return -1;
}

ADDR allocSpace(WORD sz) {
    int x = allocFindAvailable(sz);
    if (x >= 0) {
        // printStringF("-alloc:reuse:%d-", x);
        allocTbl[x].available = 0;
        return allocTbl[x].addr;
    }
    // printStringF("-alloc:%d,%d-\r\n", (int)sz, (int)allocCurFree);
    allocCurFree -= (sz);
    if (allocCurFree <= HERE) {
        printString("-out of space!-");
        allocCurFree += sz;
        return 0;
    }
    if (num_alloced < ALLOC_SZ) {
        allocTbl[num_alloced].addr = allocCurFree;
        allocTbl[num_alloced].sz = sz;
        allocTbl[num_alloced++].available = 0;
    } else {
        printString("-allocTbl too small-");
    }
    return allocCurFree;
}
#pragma endregion

void CCOMMA(BYTE v) { push(v); fCCOMMA(); }
void WCOMMA(WORD v) { push(v); fWCOMMA(); }
void COMMA(CELL v)  { push(v); fCOMMA();  }
void ACOMMA(ADDR v) { push((CELL)v); fACOMMA(); }

int compiling(char *w, int errIfNot) {
    if ((STATE == 0) && (errIfNot)) {
        printStringF("[%s]: Compile only.", w);
    }
    return (STATE == 1) ? 1 : 0;
}

int interpreting(char *w, int errIfNot) {
    if ((STATE != 0) && (errIfNot)) {
        printStringF("[%s]: Interpreting only.", w);
    }
    return (STATE == 0) ? 1 : 0;
}

void compileOrExecute(int num, ADDR bytes) {
    if (STATE == 1) {
        for (int i = 0; i < num; i++) {
            CCOMMA(bytes[i]);
        }
    } else {
        ADDR xt = (HERE+0x0010);
        for (int i = 0; i < num; i++) {
            *(xt+i) = bytes[i];
        }
        *(xt+num) = OP_RET;
        run(xt, 0);
    }
}

int isInlineWord(char *w) {

    if (strcmp_PF(w, PSTR("inline")) == 0) {
        DICT_T *dp = (DICT_T *)LAST;
        dp->flags = 2;
        return 1;
    }

    if (strcmp_PF(w, PSTR("immediate")) == 0) {
        DICT_T *dp = (DICT_T *)LAST;
        dp->flags = 1;
        return 1;
    }
    /*
    */

    if (strcmp_PF(w, PSTR("count")) == 0) {
        BYTE xx[] = {OP_DUP, OP_ONEPLUS, OP_SWAP, OP_CFETCH };
        compileOrExecute(4, xx);
        return 1;
    }

    if (strcmp_PF(w, PSTR("rot")) == 0) {
        BYTE xx[] = { OP_DTOR, OP_SWAP, OP_RTOD, OP_SWAP };
        compileOrExecute(4, xx);
        return 1;
    }

    return 0;
}

int doFind(const char *name) {         // opcode #65
    // printStringF("-find:[%s]-", name);
    DICT_T* dp = (DICT_T*)LAST;
    while (dp) {
        if (strcmpi(name, dp->name) == 0) {
            push((CELL)dp);
            return 1;
        }
        dp = (DICT_T*)dp->prev;
    }
    return 0;
}

int isDigit(char c, int base) {
    if ((base < 10) && ('0' <= c) && (c < ('0' + base))) return c - '0';
    if (('0' <= c) && (c <= '9')) return c - '0';
    if ((base == 16) && ('a' <= c) && (c <= 'f')) return c - 'a' + 10;
    if ((base == 16) && ('A' <= c) && (c <= 'F')) return c - 'A' + 10;
    return -1;
}

int doNumber(const char *w) {
    if ((*(w) == '\'') && (*(w+2) == '\'') && (*(w+3) == 0)) {
        push(*(w+1));
        return 1;
    }
    CELL num = 0;
    int base = BASE;
    int isNeg = 0;
    int valid = 0;
    if (*w == '#') { base = 10; ++w; }
    if (*w == '$') { base = 16; ++w; }
    if (*w == '%') { base = 2; ++w; }
    if ((base == 10) && (*w == '-')) { isNeg = 1; ++w; }
    while (*w) {
        int n = isDigit(*w, base);
        if (n < 0) { return 0; }
        num = (num * base) + n;
        valid = 1;
        ++w;
    }
    if (valid == 0) { return 0; }
    if (isNeg) { num = -num; }
    push(num);
    return 1;
}

// ( a -- )
void doParseWord() {    // opcode #59
    char *w = (char *)pop();
    int lwc = lastWasCall;
    lastWasCall = 0;
    // printStringF("-pw[%s]-", w);
    if (doFind(w)) {
        DICT_T *dp = (DICT_T *)pop();
        ADDR xt = dp->XT;
        // printStringF("-found:%08lx/%08lx-", dp, xt);
        if (compiling(w, 0)) {
            if (dp->flags == 1) {
                // 1 => IMMEDIATE
                run(xt, 0);
            } else {
                CCOMMA(OP_CALL);
                ACOMMA((ADDR)xt);
                lastWasCall = 1;
            }
        } else {
            run(xt, 0);
        }
        return;
    }

    if (isInlineWord(w)) { return; }

    if (doNumber(w)) {
        if (compiling(w, 0)) {
            if ((0x0000 <= T) && (T < 0x0100)) {
                CCOMMA(OP_CLIT);
                fCCOMMA();
            } else if ((0x0100 <= T) && (T < 0x010000)) {
                CCOMMA(OP_WLIT);
                fWCOMMA();
            } else {
                CCOMMA(OP_LIT);
                fCOMMA();
            }
        }
        return;
    }

    if (strcmp_PF(w, PSTR(";")) == 0) {
        if (! compiling(w, 1)) { return; }
        if (lwc && (*(HERE-5) == OP_CALL)) { *(HERE-5) = OP_JMP; } 
        else { CCOMMA(OP_RET); }
        STATE = 0;
        return;
    }

    if (strcmp_PF(w, PSTR("if")) == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_JMPZ);
        push((CELL)HERE);
        ACOMMA(0);
        return;
    }

    if (strcmp_PF(w, PSTR("if-")) == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_NJMPZ);
        push((CELL)HERE);
        ACOMMA(0);
        return;
    }

    if (strcmp_PF(w, PSTR("else")) == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_JMP);
        push((CELL)HERE);
        fSWAP();
        ACOMMA(0);
        push((CELL)HERE);
        fSWAP();
        fASTORE();
        return;
    }

    if (strcmp_PF(w, PSTR("then")) == 0) {
        if (! compiling(w, 1)) { return; }
        push((CELL)HERE);
        fSWAP();
        fASTORE();
        return;
    }

    if (strcmp_PF(w, PSTR("begin")) == 0) {
        if (! compiling(w, 1)) { return; }
        push((CELL)HERE);
        return;
    }

    if (strcmp_PF(w, PSTR("repeat")) == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_JMP);
        fACOMMA();
        return;
    }

    if (strcmp_PF(w, PSTR("while")) == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_JMPNZ);
        fACOMMA();
        return;
    }

    if (strcmp_PF(w, PSTR("until")) == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_JMPZ);
        fACOMMA();
        return;
    }

    if (strcmp_PF(w, PSTR("while-")) == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_NJMPNZ);
        fACOMMA();
        return;
    }

    if (strcmp_PF(w, PSTR("until-")) == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_NJMPZ);
        fACOMMA();
        return;
    }

    if (strcmp_PF(w, PSTR("do")) == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_DO);
        push((CELL)HERE);
        return;
    }

    if (strcmp_PF(w, PSTR("leave")) == 0) {
        if (! compiling(w, 1)) { return; }
        printString("WARNING: LEAVE not supported!");
        return;
    }

    if (strcmp_PF(w, PSTR("loop")) == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_LOOP);
        CCOMMA(OP_JMPNZ);
        fACOMMA();
        return;
    }

    if (strcmp_PF(w, PSTR("loop+")) == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_LOOPP);
        CCOMMA(OP_JMPNZ);
        fACOMMA();
        return;
    }

    if (strcmp_PF(w, PSTR(":")) == 0) {
        if (! interpreting(w, 1)) { return; }
        if (getNextWord(w, ' ')) {
            doCreate(w);
            STATE = 1;
        }
        return;
    }

    if (strcmp_PF(w, PSTR("variable")) == 0) {
        if (! interpreting(w, 1)) { return; }
        if (getNextWord(w, ' ')) {
            doCreate(w);
            CCOMMA(OP_LIT);
            COMMA((CELL)HERE+CELL_SZ+1);
            CCOMMA(OP_RET);
            COMMA(0);
        }
        return;
    }

    if (strcmp_PF(w, PSTR("constant")) == 0) {
        if (! interpreting(w, 1)) { return; }
        if (getNextWord(w, ' ')) {
            doCreate(w);
            CCOMMA(OP_LIT);
            fCOMMA();
            CCOMMA(OP_RET);
        }
        return;
    }

    BYTE op = getOpcode(w);
    if (op < 0xFF) {
        if (compiling(w, 0)) {
            CCOMMA(op);
        } else {
            ADDR xt = HERE+0x20;
            *(xt) = op;
            *(xt+1) = OP_RET;
            run(xt, 0);
        }
        return;
    }
    STATE = 0;
    printStringF("[%s]??", w);
    throw(123);
}

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void parseLine(char *line) {
    push((CELL)line);
    doParse(' ');
}

void loadUserWords() {
    loadSource(PSTR(": auto-run-last last >body dict a! ;"));
    loadSource(PSTR(": auto-run-off 0 0 a! ;"));
    loadSource(PSTR(": elapsed tick swap - 1000 /mod . . ;"));
    loadSource(PSTR(": bm tick swap begin 1- while- drop elapsed ;"));
    loadSource(PSTR(": low->high over over > if swap then ;"));
    loadSource(PSTR(": high->low over over < if swap then ;"));
    loadSource(PSTR(": dump low->high do i c@ . loop ;"));
    loadSource(PSTR(": led 13 ;"));
    loadSource(PSTR(": led-on 1 led dp! ; : led-off 0 led dp! ;"));
    loadSource(PSTR(": blink led-on dup ms led-off dup ms ;"));
    loadSource(PSTR(": k 1000 * ; : mil k k ;"));
    loadSource(PSTR(": blinks 0 swap do blink loop ;"));
    loadSource(PSTR("variable pot  3 pot ! "));
    loadSource(PSTR("variable but  6 but ! "));
    loadSource(PSTR(": init led output-pin pot @ input-pin but @ input-pin ;"));
    loadSource(PSTR("variable pot-lv variable sens 4 sens !"));
    loadSource(PSTR(": but@ but @ dp@ ;"));
    loadSource(PSTR(": pot@ pot @ ap@ ;"));
    loadSource(PSTR(": bp->led but@ if led-on else led-off then ;"));
    loadSource(PSTR(": .pot? pot@ dup pot-lv @ - abs sens @ > if dup . cr pot-lv ! else drop then ;"));
    loadSource(PSTR(": go bp->led .pot? ;"));
    #ifdef __DEV_BOARD__
    loadSource(PSTR("init // auto-run-last"));
    #endif
    // loadSource(PSTR(""));
}

// ---------------------------------------------------------------------
// main.c
// ---------------------------------------------------------------------

#pragma warning(disable:4996)

void toTIB(int c) {
    if (c == 0) { return; }
    if (c == 8) {
        if (0 < numTIB) {
            numTIB--;
            *(--TIBEnd) = 0;
        }
        return;
    }
    if (c == 13) {
        // printStringF("[%s]\r\n", TIB);
        parseLine(TIB);
        ok();
        numTIB = 0;
        TIBEnd = TIB;
        *TIBEnd = 0;
        return;
    }
    if (TIB_SZ < numTIB) {
        printString("-TIB full-");
        return;
    }
    if (c == 9) { c = 32; }
    if (c == 10) { c = 32; }
    if (32 <= c) {
        *(TIBEnd++) = c;
        *(TIBEnd) = 0;
#ifdef __DEV_BOARD__
        char x[2];
        x[0] = c;
        x[1] = 0;
        printString(x);
#endif
    }
}

#ifdef __DEV_BOARD__
void loop() {
    while (pc.readable()) {
        int c = pc.getc();
        toTIB(c);
    }
}
#else
void doHistory(char* l) {
    FILE* fp = fopen("history.txt", "at");
    if (fp) {
        fprintf(fp, "%s\n", l);
        fclose(fp);
    }
}

void loop() {
    char buf[128];
    numTIB = 0;
    TIBEnd = TIB;
    *TIBEnd = 0;
    fgets(buf, 128, stdin);
    int len = strlen(buf);
    while ((0 < len) && (buf[len - 1] < ' ')) {
        buf[--len] = 0;
    }
    if (strcmp(buf, "bye") == 0) {
        isBYE = 1;
        return;
    }
    doHistory(buf);
    for (int i = 0; i < len; i++) toTIB(buf[i]);
    toTIB(13);
}
#endif

void loadSource(const char* src) {
    parseLine((char*)src);
}

void loadSourceF(const char* fmt, ...) {
    char buf[96];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    parseLine(buf);
}

void loadBaseSystem() {
    loadSourceF(": cell %d ; : addr %d ;", CELL_SZ, ADDR_SZ);
    loadSourceF(": dict $%lx ;", (long)&dict[0]);
    loadSourceF(": (here) $%lx ; : here (here) @ ;", (long)&HERE);
    loadSourceF(": (last) $%lx ; : last (last) @ ;", (long)&LAST);
    loadSourceF(": base $%lx ;", (long)&BASE);
    loadSourceF(": state $%lx ;", (long)&STATE);
    loadSourceF(": tib $%lx ;", (long)&TIB[0]);
    loadSourceF(": (dsp) $%lx ; : (rsp) $%lx ;", (long)&DSP, (long)&RSP);
    loadSourceF(": dstack $%lx ;", (long)&dstk[0]);
    loadSourceF(": rstack $%lx ;", (long)&rstk[0]);

    loadSource(": hex $10 base ! ; : decimal #10 base ! ; : binary %10 base ! ;"
        " : depth (dsp) @ 1- ; : 0sp 0 (dsp) ! ;"
        " : nip swap drop ; : tuck swap over ;"
        " : 2dup over over ; : 2drop drop drop ;"
        " : mod /mod drop ; : / /mod nip ;"
        " : <> = 0= ;"
        " : negate 0 swap - ;"
        " : abs dup 0 < if negate then ;"
        " : bl #32 ; : space bl emit ;"
        " : pad here $40 + ; "
        " : (neg) here $44 + ; "
        " : is-neg? dup 0 < if negate 1 (neg) ! then ; "
        " : hold pad @ 1- dup pad ! c! ; "
        " : <# pad dup ! ; "
        " : # base @ u/mod swap abs '0' + dup '9' > if 7 + then hold ; "
        " : #s begin # while- drop ; "
        " : #> (neg) @ if '-' emit then pad @ pad over - type ; "
        " : (.)  <# 0 (neg) ! base @ #10 = if is-neg? then #s #> ; "
        " : (u.) <# 0 (neg) ! #s #> ; "
        " : . space (.) ;  : u. space (u.) ; "
        " : +! tuck @ + swap ! ;"
        " : ?dup if- dup then ;"
        " : min over over < if drop else nip then ;"
        " : max over over > if drop else nip then ;"
        " : between rot dup >r min max r> = ;"
        " : cr #13 emit #10 emit ;"
        " : allot here + (here) ! ;"
        " : >body addr + a@ ;"
        " : .wordl cr dup . space dup >body . addr 2* + dup c@ . 1+ space count type ;"
        " : wordsl last begin dup .wordl a@ while- drop ;"
        " : .word addr 2* + 1+ count type 9 emit ;"
        " : words last begin dup .word a@ while- drop ;"
    );
}

void ok() {
    printString(" ok. ");
    doDotS();
    printString("\r\n");
}

void startUp() {
    printString("BoardForth v0.0.1 - Chris Curl\r\n");
    printString("Source: https://github.com/CCurl/BoardForth \r\n");
    printStringF("Dictionary size is: %d ($%04x) bytes. \r\n", (int)DICT_SZ, (int)DICT_SZ);
    vmInit();
    printString("Hello.");
}

#ifndef __DEV_BOARD__
int main()
{
    // Initialise the digital pin LED13 as an output

    startUp();
    loadBaseSystem();
    loadUserWords();
    int num = 0;
    int x = 0;
    numTIB = 0;
    ok();

    while (true) {
        loop();
        if (isBYE) { break; }
    }
}
#endif
