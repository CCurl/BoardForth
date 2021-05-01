// ---------------------------------------------------------------------
// forth.c
// ---------------------------------------------------------------------

#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef void (*FP)();
typedef long  CELL;
typedef unsigned long  ulong;
typedef unsigned short WORD;
typedef unsigned char  BYTE;
typedef BYTE* ADDR;

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

void dumpDict();
void startUp();
void vmInit();
void push(CELL);
CELL pop();
void rpush(CELL);
CELL rpop();
void run(ADDR, CELL);
ADDR allocSpace(WORD);
void allocFree(ADDR);
BYTE getOpcode(char*);
ADDR align4(ADDR);
BYTE nextChar();
void is_hex(char*);
void is_decimal(char*);
void is_binary(char*);
void CCOMMA(BYTE v);
void WCOMMA(WORD v);
void COMMA(CELL);
void ACOMMA(ADDR);
void parseLine(char*);
void loadBaseSystem();
BYTE getOpcode(char*);
void allocFreeAll();
void printString(const char*);
void printStringF(const char*, ...);
void loadUserWords();
void ok();
void autoRun();
CELL cellAt(ADDR);
CELL wordAt(ADDR);
ADDR addrAt(ADDR);
void fDUMPDICT();
void doNextWord();
void doParse(char sep);
int doNumber(const char *);

#pragma warning(disable:4996)

int digitalRead(int pin) {return 0;}
int analogRead(int pin) {return 0;}
void digitalWrite(int pin, int val) {}
void analogWrite(int pin, int val) {}
void delay(int ms) {}
long millis() { return 0; }
void cellStore(ADDR addr, CELL val);
void wordStore(ADDR addr, CELL val);
void doPARSEWORD();
void doCreate(const char* name);

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
int isBYE = 0;

#define Y(op, code) X("", op, code)

#define OPCODES \
    X("NOOP", NOOP, ) \
    X("test", TEST, push(*(PC++))) \
    X("cliteral", CLIT, push(*(PC++))) \
    X("wliteral", WLIT, push(wordAt(PC)); PC += WORD_SZ) \
    X("literal", LIT, push(cellAt(PC)); PC += CELL_SZ) \
    X("c@", CFETCH, ADDR a = (ADDR)T; T = (CELL)*a) \
    X("w@", WFETCH, T = wordAt((ADDR)T)) \
    X("@",  FETCH,  T = cellAt((ADDR)T)) \
    X("a@", AFETCH, (ADDR_SZ == 2) ? fWFETCH() : fFETCH()) \
    X("c!", CSTORE, ADDR a = (ADDR)pop(); CELL v = pop(); *(a) = (v & 0xFF)) \
    X("!",  STORE,  ADDR a = (ADDR)pop(); CELL v = pop(); cellStore(a, v)) \
    X("w!", WSTORE, ADDR a = (ADDR)pop(); CELL v = pop(); wordStore(a, v)) \
    X("a!", ASTORE, ADDR a = (ADDR)pop(); CELL v = pop(); (ADDR_SZ == 2) ? wordStore(a,v) : cellStore(a,v)) \
    X("RSHIFT", RSHIFT, T = T>>1) \
    X("AND", AND, N &= T; pop()) \
    X("OR", OR, N |= T; pop()) \
    X("XOR", XOR, N ^= T; pop()) \
    X("NOT", NOT, T = (T) ? 0 : -1) \
    X(">r", DTOR, rpush(pop())) \
    X("r@", RFETCH, push(R)) \
    X("r>", RTOD, push(rpop())) \
    X("(", PAREN, ) \
    X("TYPE", TYPE, ) \
    X("DOTS", DOTS, ) \
    X("+", ADD, N += T; pop()) \
    X("EMIT", EMIT, ) \
    X("WDTFEED", WDTFEED, ) \
    X("BREAK", BREAK, ) \
    X("FILL", FILL, ) \
    X("OPENBLOCK", OPENBLOCK, ) \
    X("FILECLOSE", FILECLOSE, ) \
    X("FILEREAD", FILEREAD, ) \
    X("LOAD", LOAD, ) \
    X("THRU", THRU, ) \
    X("DO", DO, ) \
    X("LOOP", LOOP, ) \
    X("LOOPP", LOOPP, ) \
    X("DEBUGGER", DEBUGGER, ) \
    X("PARSEWORD", PARSEWORD, doPARSEWORD()) \
    X("<", LESS, N = (N < T) ? 1 : 0; pop()) \
    X("GETXT", GETXT, ) \
    X("ALIGN2", ALIGN2, ) \
    X("ALIGN4", ALIGN4, ) \
    X("CREATE", CREATE, ) \
    X("FIND", FIND, ) \
    X("NEXTWORD", NEXTWORD, doNextWord()) \
    X("ISNUMBER", ISNUMBER, doNumber((char *)pop());) \
    X("NJMPZ", NJMPZ, PC = (T == 0)? addrAt(PC) : PC + ADDR_SZ) \
    X("NJMPNZ", NJMPNZ, PC = (T != 0)? addrAt(PC) : PC + ADDR_SZ) \
    X("PARSELINE", PARSELINE, doParse(' ')) \
    X("=", EQUALS, N = (N == T) ? 1 : 0; pop()) \
    X(">", GREATER, N = (N > T) ? 1 : 0; pop()) \
    X("I", I, ) \
    X("J", J, ) \
    X("INPUTPIN", INPUTPIN, ) \
    X("OUTPUTPIN", OUTPUTPIN, ) \
    X("DELAY", DELAY, ) \
    X("TICK", TICK, ) \
    X("APINSTORE", APINSTORE, ) \
    X("DPINSTORE", DPINSTORE, ) \
    X("APINFETCH", APINFETCH, ) \
    X("DPINFETCH", DPINFETCH, ) \
    X("MWFETCH", MWFETCH, ) \
    X("MCSTORE", MCSTORE, ) \
    X("NUM2STR", NUM2STR, ) \
    X("COM", COM, ) \
    X("SQUOTE", SQUOTE, ) \
    X("-", SUB, N -= T; pop()) \
    X("*", MULT, N *= T; pop()) \
    X("SLMOD", SLMOD, ) \
    X("LSHIFT", LSHIFT, T = T << 1) \
    X("CCOMMA", CCOMMA, *(HERE++) = (BYTE)pop()) \
    X("WCOMMA", WCOMMA, ) \
    X("COMMA", COMMA, ) \
    X("ACOMMA", ACOMMA, ) \
    X("CALL", CALL, ) \
    X("RET", RET, PC = (ADDR)rpop()) \
    X("JMP", JMP, PC = addrAt(PC)) \
    X("JMPZ", JMPZ, PC = (T == 0)? addrAt(PC) : PC + ADDR_SZ; pop()) \
    X("JMPNZ", JMPNZ, PC = (T != 0)? addrAt(PC) : PC + ADDR_SZ; pop()) \
    X("ONEMINUS", ONEMINUS, --T) \
    X("ONEPLUS", ONEPLUS, ++T) \
    X("DUP", DUP, push(T)) \
    X("SWAP", SWAP, CELL x = T; T = N; N = x) \
    X("DROP", DROP, pop()) \
    X("OVER", OVER, push(N)) \
    X("BYE", BYE, isBYE = 1) \

#define X(name, op, code) OP_ ## op,
enum {
    OPCODES
};

#undef X
#define X(name, op, code) void f ## op();
OPCODES

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
#define X(name, op, code) if (strcmp(w, name) == 0) { return OP_ ## op; }
BYTE getOpcode(char* w) {
    OPCODES
        return 0xFF;
}

#undef X
#define X(name, op, code) doCreate(name);
void defineWords() {
    OPCODES
}

void doCreate(const char *name) {
    HERE = align4(HERE);
    // printStringF("-define [%s] at %d (%lx)", name, HERE, HERE);

    DICT_T* dp = (DICT_T*)HERE;
    dp->prev = (ADDR)LAST;
    dp->flags = 0;
    dp->len = (BYTE)strlen(name);
    strcpy(dp->name, name);
    LAST = HERE;
    HERE += ADDR_SZ + dp->len + 3;
    // printStringF(",XT:%d (%lx)-", HERE, HERE);
}

void doNextWord() {
    char* l = (char*)pop();
}

int matches(char ch, char sep) {
    if (ch == sep) { return 1; }
    if ((sep == ' ') && (ch < sep)) { return 1; }
    return 0;
}

void doParse(char sep) {
    toIN = (char*)pop();
    TIBEnd = toIN + strlen(toIN);
    while (1) {
        char* w = (char*)HERE + 0x40;
        char* wp = w;
        CELL len = 0;
        while ((toIN < TIBEnd) && matches(*toIN, sep)) {
            ++toIN;
        }
        while ((toIN < TIBEnd) && !matches(*toIN, sep)) {
            ++len;
            *(wp++) = *(toIN++);
        }
        *wp = 0;
        if (len == 0) { return; }
        push((CELL)w);
        doPARSEWORD();
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

void run(ADDR start, CELL max_cycles) {
    PC = start;
    // printStringF("\r\nrun: %d (%04lx), %d cycles ... ", PC, PC, max_cycles);
    while (1) {
        BYTE IR = *(PC++);
        if (IR == OP_BYE) { return; }
        if (IR == OP_RET) {
            if (RSP < 1) { return; }
            PC = (ADDR)rpop();
        } else if (prims[IR]) {
            prims[IR]();
        } else {
            printStringF("-unknown opcode: %d ($%02x) at %04lx-", IR, IR, PC-1);
        }
        if (max_cycles) {
            if (--max_cycles < 1) { return; }
        }
    }
}

void autoRun() {
    ADDR addr = addrAt(0);
    if (addr) {
        run(addr, 0);
    }
}

void push(CELL v) {
    DSP = (DSP < STK_SZ) ? DSP+1 : STK_SZ;
    T = v;
}
CELL pop() {
    DSP = (DSP > 0) ? DSP-1 : 0;
    return dstk[DSP+1];
}

void rpush(CELL v) {
    RSP = (RSP < STK_SZ) ? RSP+1 : STK_SZ;
    R = v; 
}
CELL rpop() {
    RSP = (RSP > 0) ? RSP-1 : 0;
    return rstk[RSP+1];
}

BYTE nextChar() {
    if (*toIN) return *(toIN++);
    return 0;
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


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------


void CCOMMA(BYTE v) { push(v); fCCOMMA(); }
void WCOMMA(WORD v) { push(v); fWCOMMA(); }
void COMMA(CELL v)  { push(v); fCOMMA();  }
void ACOMMA(ADDR v) { push((CELL)v); fACOMMA(); }

CELL getXT(CELL addr) {
    push(addr);
    fGETXT();
    return pop();
}

ADDR align4(ADDR val) {
    push((CELL)val);
    fALIGN4();
    return (ADDR)pop();
}

CELL align2(CELL val) {
    push(val);
    fALIGN2();
    return pop();
}

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

    if (strcmp_PF(w, PSTR("nip")) == 0) {
        BYTE xx[] = {OP_SWAP, OP_DROP};
        compileOrExecute(2, xx);
        return 1;
    }

    if (strcmp_PF(w, PSTR("tuck")) == 0) {
        BYTE xx[] = {OP_SWAP, OP_OVER};
        compileOrExecute(2, xx);
        return 1;
    }

    if (strcmp_PF(w, PSTR("2dup")) == 0) {
        BYTE xx[] = {OP_OVER, OP_OVER};
        compileOrExecute(2, xx);
        return 1;
    }

    if (strcmp_PF(w, PSTR("2drop")) == 0) {
        BYTE xx[] = {OP_DROP, OP_DROP};
        compileOrExecute(2, xx);
        return 1;
    }

    if (strcmp_PF(w, PSTR("cell")) == 0) {
        BYTE xx[] = {OP_CLIT, CELL_SZ};
        compileOrExecute(2, xx);
        return 1;
    }

    if (strcmp_PF(w, PSTR("addr")) == 0) {
        BYTE xx[] = {OP_CLIT, ADDR_SZ};
        compileOrExecute(2, xx);
        return 1;
    }

    if (strcmp_PF(w, PSTR("/")) == 0) {
        BYTE xx[] = {OP_SLMOD, OP_SWAP, OP_DROP};
        compileOrExecute(3, xx);
        return 1;
    }

    if (strcmp_PF(w, PSTR("mod")) == 0) {
        BYTE xx[] = {OP_SLMOD, OP_DROP};
        compileOrExecute(2, xx);
        return 1;
    }

    if (strcmp_PF(w, PSTR("<>")) == 0) {
        BYTE xx[] = {OP_EQUALS, OP_NOT};
        compileOrExecute(2, xx);
        return 1;
    }

    if (strcmp_PF(w, PSTR("space")) == 0) {
        BYTE xx[] = {OP_CLIT, 32, OP_EMIT };
        compileOrExecute(3, xx);
        return 1;
    }

    if (strcmp_PF(w, PSTR("count")) == 0) {
        BYTE xx[] = {OP_DUP, OP_ONEPLUS, OP_SWAP, OP_CFETCH };
        compileOrExecute(4, xx);
        return 1;
    }

    if (strcmp_PF(w, PSTR("rot")) == 0) {
        BYTE xx[] = {OP_DTOR, OP_SWAP, OP_RTOD, OP_SWAP };
        compileOrExecute(4, xx);
        return 1;
    }

    return 0;
}

static int match(CELL sep, CELL ch) {
    if (sep == ch) { return 1; }
    if (sep == ' ') { return (ch <= ' ') ? 1 : 0; }
    return 0;
}

static CELL parse(CELL sep, CELL* ret) {
    while ((toIN < TIBEnd) && match(sep, *toIN)) {
        ++toIN;
    }
    CELL len = 0;
    while ((toIN < TIBEnd) && !match(sep, *toIN)) {
        ++len;
        ++toIN;
    }
    *ret = (CELL)toIN;
    return len;
}

int doFind(const char *name) {         // opcode #65
    printStringF("-find:[%s]-", name);
    DICT_T* dp = (DICT_T*)LAST;
    while (dp) {
        if (strcmp(name, dp->name) == 0) {
            push((CELL)dp);
            return 1;
        }
        dp = (DICT_T*)dp->prev;
    }
    return 0;
}

int doNumber(const char *w) {
    return 0;
}

// ( a -- )
void doPARSEWORD() {    // opcode #59
    char *w = (char *)pop();
    printStringF("-pw[%s]-", w);
    if (doFind(w)) {
        DICT_T *dp = (DICT_T *)T;
        fGETXT();
        ADDR xt = (ADDR)pop();
        // printStringF("-found:%08lx/%08lx-", dp, xt);
        if (compiling(w, 0)) {
            if (dp->flags == 1) {
                // 1 => IMMEDIATE
                run(xt, 0);
            } else {
                CCOMMA(OP_CALL);
                ACOMMA((ADDR)xt);
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
        CCOMMA(OP_RET);
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
        push((CELL)w);
        doNextWord();
        if (pop()) {
            push((CELL)w);
            fCREATE();
            STATE = 1;
        }
        return;
    }

    if (strcmp_PF(w, PSTR("variable")) == 0) {
        if (! interpreting(w, 1)) { return; }
        push((CELL)w);
        fNEXTWORD();
        if (pop()) {
            push((CELL)w);
            fCREATE();
            CCOMMA(OP_LIT);
            COMMA((CELL)HERE+CELL_SZ+1);
            CCOMMA(OP_RET);
            COMMA(0);
        }
        return;
    }

    if (strcmp_PF(w, PSTR("constant")) == 0) {
        if (! interpreting(w, 1)) { return; }
        push((CELL)w);
        fNEXTWORD();
        if (pop()) {
            push((CELL)w);
            fCREATE();
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
}

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------














void is_hex(char *word) {
    CELL num = 0;
    if (*word == (char)0) { push(0); return; }
    while (*word) {
        char c = *(word++);
        if ((c >= '0') && (c <= '9')) {
            num *= 0x10;
            num += (c - '0');
            continue;
        }
        if ((c >= 'A') && (c <= 'F')) {
            num *= 0x10;
            num += ((c+10) - 'A');
            continue;
        }
        if ((c >= 'a') && (c <= 'f')) {
            num *= 0x10;
            num += ((c+10) - 'a');
            continue;
        }
        push(0);
        return;
    }
    push((CELL)num);
    push(1);
}

void is_decimal(char *word) {
    long num = 0;
    int is_neg = 0;
    if (*word == '-') {
        word++;
        is_neg = 1;
    }
    if (*word == (char)0) { push(0); return; }
    while (*word) {
        char c = *(word++);
        if ((c >= '0') && (c <= '9')) {
            num *= 10;
            num += (c - '0');
        } else {
            push(0);
            return;
        }
    }

    num = is_neg ? -num : num;
    push((CELL)num);
    push(1);
}

void is_binary(char *word) {
    CELL num = 0;
    if (*word == (char)0) { push(0); return; }
    while (*word) {
        char c = *(word++);
        if ((c >= '0') && (c <= '1')) {
            num *= 2;
            num += (c - '0');
        } else {
            push(0);
            return;
        }
    }

    push((CELL)num);
    push(1);
}

void parseLine(char *line) {
    TIB = TIBBuf;
    strcpy(TIB, line);
    push((CELL)TIB);
    doParse(' ');
}

void loadUserWords() {
    // sprintf(buf, ": dpin-base #%ld ; : apifln-base #%ld ;", (long)0, (long)A0);
    // parseLine(buf);
    loadSource(PSTR(": auto-run-last last >body 0 a! ;"));
    loadSource(PSTR(": auto-run-off 0 0 a! ;"));

    loadSource(": ALT0  $100 ;");
    loadSource(": ALT1  $200 ;");
    loadSource(": ALT2  $300 ;");
    loadSource(": ALT3  $400 ;");
    loadSource(": PA_0  $00 ;");
    loadSource(": PA_1  $01 ;");
    loadSource(": PA_2  $02 ;");
    loadSource(": PA_3  $03 ;");
    loadSource(": PA_4  $04 ;");
    loadSource(": PA_5  $05 ;");
    loadSource(": PA_6  $06 ;");
    loadSource(": PA_7  $07 ;");
    loadSource(": PA_8  $08 ;");
    loadSource(": PA_9  $09 ;");
    loadSource(": PA_10 $0A ;");
    loadSource(": PA_11 $0B ;");
    loadSource(": PA_12 $0C ;");
    loadSource(": PA_13 $0D ;");
    loadSource(": PA_14 $0E ;");
    loadSource(": PA_15 $0F ;");
    loadSource(": PA_1_ALT0  PA_1  ALT0 or ;");
    loadSource(": PA_4_ALT0  PA_4  ALT0 or ;");
    loadSource(": PA_7_ALT0  PA_7  ALT0 or ;");
    loadSource(": PA_7_ALT1  PA_7  ALT1 or ;");
    loadSource(": PA_7_ALT2  PA_7  ALT2 or ;");
    loadSource(": PA_15_ALT0 PA_15 ALT0 or ;");

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
    loadSource(PSTR(": init led output pot @ input but @ input ;"));
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

void dumpDict() {
    ADDR addr = &dict[0];
    printStringF("%04x %04x (%ld %ld)", HERE, LAST, HERE, LAST);
    for (int i = 0; i < 9999; i++) {
        if (addr > HERE) { return; }
        if (i % 16 == 0) printStringF("\r\n %08lx:", addr);
        printStringF(" %02x", *(addr++));
    }
}

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// main.c
// ---------------------------------------------------------------------

#pragma warning(disable:4996)

void parseLine(char*);

#ifdef __DEV_BOARD__
Serial pc(PA_9, PA_10, "pc", 19200);

void printSerial(const char* str) {
    pc.printf("%s", str);
}
#endif

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
        char x[2];
        x[0] = c;
        x[1] = 0;
        *(TIBEnd++) = c;
        *(TIBEnd) = 0;
        printString(x);
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
        fprintf(fp, "%s\r\n", l);
        fclose(fp);
    }
}

void loop() {
    char buf[128];
    ok();
    fgets(buf, 128, stdin);
    int len = strlen(buf);
    while ((0 < len) && (buf[len - 1] <= ' ')) {
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
    // ***MAX 96***  ...
    loadSourceF(": ds $%lx ;", (long)&dict[0]);
    loadSourceF(": (h) $%lx ; : here (h) @ ;", (long)&HERE);
    loadSourceF(": (l) $%lx ; : last (l) @ ;", (long)&LAST);
    loadSourceF(": base $%lx ;", (long)&BASE);
    loadSourceF(": state $%lx ;", (long)&STATE);
    loadSourceF(": tib $%lx ;", (long)&TIB[0]);
    loadSourceF(": dsp $%lx ; : rsp $%lx ;", (long)&DSP, (long)&RSP);
    loadSourceF(": dstack $%lx ;", (long)&dstk[0]);
    loadSourceF(": rstack $%lx ;", (long)&rstk[0]);

    loadSource(": hex $10 base ! ;"
        "\n: decimal #10 base ! ;"
        "\n: binary %10 base ! ;"
        "\n: +! tuck @ + swap ! ;"
        "\n: ?dup if- dup then ;"
        "\n: abs dup 0 < if 0 swap - then ;"
        "\n: min over over < if drop else nip then ;"
        "\n: max over over > if drop else nip then ;"
        "\n: between rot dup >r min max r> = ;"
        "\n: cr #13 emit #10 emit ;"
        "\n: . 0 num>str space type ;"
        "\n: allot here + (h) ! ;"
        "\n: .word cr dup . space dup >body . addr +"
        "\n   dup c@ . 1+ space count type ;"
        "\n: words last begin dup .word a@ while- drop ;");
    // ***MAX 96***  ...
}

void ok() {
    printString(" ok. ");
    fDOTS();
    printString("\r\n");
}

void startUp() {
    printString("BoardForth v0.0.1 - Chris Curl\r\n");
    printString("Source: https://github.com/CCurl/BoardForth \r\n");
    printStringF("Dictionary size is: %d ($%04x) bytes. \r\n", (int)DICT_SZ, (int)DICT_SZ);
    vmInit();
    printString("Hello.");
}

int main()
{
    // Initialise the digital pin LED13 as an output
#ifdef __DEV_BOARD__
    DigitalOut led(PC_13);
#endif

    startUp();
    loadBaseSystem();
    loadUserWords();
    // push(0);
    // fDUMPDICT();
    int num = 0;
    int x = 0;
    numTIB = 0;

    while (true) {
        loop();
#ifdef __DEV_BOARD__
        num += 1;
        if (num > 499999) {
            num = 0;
            x = (x) ? 0 : 1;
            led = x;
        }
#else
        if (isBYE) { break; }
#endif
    }
}
