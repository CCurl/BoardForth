// ---------------------------------------------------------------------
// forth.c
// ---------------------------------------------------------------------

#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef __DEV_BOARD__
void printSerial(const char* str) {
    mySerial.print(str);
}
#endif

#ifdef __ESP32__
void printStringF(const char *, ...);
void analogWrite(int pin, int val) {
  printStringF("-ap!:%d/%d-", pin, val);
}
#endif

//typedef void (*FP)();
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
#define DROP1 --DSP
#define DROP2 DROP1; DROP1

typedef struct {
    ADDR prev;
    ADDR XT;
    BYTE flags;
    BYTE len;
    char name[32]; // not really 32 ... but we need a number
} DICT_T;

#define LOOP_STK_SZ 8
typedef struct {
    ADDR startAddr;
    CELL start;
    CELL index;
    CELL stop;
} DO_LOOP_T;

void push(CELL);
CELL pop();
void rpush(CELL);
CELL rpop();
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
void addrStore(ADDR addr, ADDR val);
void doParseWord();
void doCreate(const char* name);
void doSlMod();
void doUSlMod();
void doType();
void doDotS();
void doFor();
void doNext();
void doI();
void doJ();
void doBegin();
void doAgain();
void doWhile(int, int);
void doSQuote();
void doDotQuote();
void doWComma(CELL);
void doComma(CELL);
void doAComma(ADDR);
int strCmp(const char*, const char*);

BYTE IR;
ADDR PC;
ADDR HERE;
ADDR LAST;
BYTE dict[DICT_SZ];
char TIBBuf[TIB_SZ];
char *TIB = TIBBuf;
char *toIN, *TIBEnd;
CELL BASE, STATE, DSP, RSP;
CELL dstk[STK_SZ+1];
CELL rstk[STK_SZ+1];
CELL loopDepth, t1;
int numTIB = 0;
int lastWasCall = 0;
DO_LOOP_T doStack[LOOP_STK_SZ];
int loopSP = -1;
int isBYE = 0;
DICT_T tempWords[10];
int debugOn = 0;

#ifndef __DEV_BOARD__
#pragma warning(disable:4996)
void pinMode(int pin, int mode) {}
int digitalRead(int pin) { return pin+1; }
int analogRead(int pin) { return pin+1; }
void digitalWrite(int pin, int val) { printString("-dp!-"); }
void analogWrite(int pin, int val) { printString("-ap!-"); }
void delay(int ms) { Sleep(ms); }
long millis() { return GetTickCount(); }
#endif

// #define Y(op, code) X(#op, op, code)

#define OPCODES \
    X("NOOP", NOOP, ) \
    X("CALL", CALL, rpush((CELL)PC + ADDR_SZ); PC = addrAt(PC)) \
    X("EXIT", RET, PC = (ADDR)rpop()) \
    X("JMP",    JMP,    PC = addrAt(PC)) \
    X("JMPZ",   JMPZ,   PC = (T == 0) ? addrAt(PC) : PC + ADDR_SZ; DROP1) \
    X("NJMPZ",  NJMPZ,  PC = (T == 0) ? addrAt(PC) : PC + ADDR_SZ) \
    X("CLIT", CLIT, push(*(PC++))) \
    X("WLIT", WLIT, push(wordAt(PC)); PC += WORD_SZ) \
    X("LIT",  LIT, push(cellAt(PC)); PC += CELL_SZ) \
    X("DUP", DUP, push(T)) \
    X("SWAP", SWAP, t1 = T; T = N; N = t1) \
    X("DROP", DROP, DROP1) \
    X("OVER", OVER, push(N)) \
    X("c@", CFETCH, T = (BYTE)*A) \
    X("w@", WFETCH, T = wordAt(A)) \
    X("@",  FETCH,  T = cellAt(A)) \
    X("a@", AFETCH, (ADDR_SZ == 2) ? T = wordAt(A) : T = cellAt(A)) \
    X("c!", CSTORE, *A = (N & 0xFF); DROP2) \
    X("!",  STORE,  cellStore(A, N); DROP2) \
    X("w!", WSTORE, wordStore(A, N); DROP2) \
    X("a!", ASTORE, (ADDR_SZ == 2) ? wordStore(A, N) : cellStore(A, N); DROP2) \
    X("+", ADD, N += T; DROP1) \
    X("-", SUB, N -= T; DROP1) \
    X("*", MULT, N *= T; DROP1) \
    X("/MOD", SLMOD, doSlMod()) \
    X("U/MOD", USLMOD, doUSlMod()) \
    X("2/", RSHIFT, T = T >> 1) \
    X("2*", LSHIFT, T = T << 1) \
    X("1-", ONEMINUS, --T) \
    X("1+", ONEPLUS, ++T) \
    X(".S", DOTS, doDotS()) \
    X("AND", AND, N &= T; DROP1) \
    X("NAND", NAND, N = (N & T) ? 0 : 1; DROP1) \
    X("OR", OR, N |= T; DROP1) \
    X("XOR", XOR, N ^= T; DROP1) \
    X("<", LESS, N = (N < T) ? 1 : 0; DROP1) \
    X("=", EQUALS, N = (N == T) ? 1 : 0; DROP1) \
    X("0=", LNOT, T = (T == 0) ? -1 : 0) \
    X(">", GREATER, N = (N > T) ? 1 : 0; DROP1) \
    X(">r", DTOR, rpush(pop())) \
    X("r@", RFETCH, push(R)) \
    X("r>", RTOD, push(rpop())) \
    X("TYPE", TYPE, doType() ) \
    X("EMIT", EMIT, printStringF("%c", (char)pop())) \
    X("WDTFEED", WDTFEED, ) \
    X("OPENBLOCK", OPENBLOCK, ) \
    X("FILECLOSE", FILECLOSE, ) \
    X("FILEREAD", FILEREAD, ) \
    X("LOAD", LOAD, ) \
    X("THRU", THRU, ) \
    X("FOR", FOR, doFor()) \
    X("I", I, doI()) \
    X("J", J, doJ()) \
    X("NEXT", NEXT, doNext()) \
    X("BEGIN", BEGIN, doBegin()) \
    X("AGAIN", AGAIN, doAgain()) \
    X("WHILE", WHILE, doWhile(1, 0)) \
    X("UNTIL", UNTIL, doWhile(1, 1)) \
    X("WHILE-", WHILEN, doWhile(0, 0)) \
    X("UNTIL-", UNTILN, doWhile(0, 1)) \
    X("INPUT-PIN",    INPUT_PIN,        pinMode(T, PIN_INPUT);        DROP1) \
    X("INPUT-PULLUP", INPUT_PIN_PULLUP, pinMode(T, PIN_INPUT_PULLUP); DROP1) \
    X("OUTPUT-PIN",   OUTPUT_PIN,       pinMode(T, PIN_OUTPUT);       DROP1) \
    X("MS", DELAY, delay(pop())) \
    X("TICK", TICK, push(millis())) \
    X("ap!", APIN_STORE, analogWrite(T, N);  DROP2) \
    X("dp!", DPIN_STORE, digitalWrite(T, N); DROP2) \
    X("ap@", APIN_FETCH, T = analogRead(T); ) \
    X("dp@", DPIN_FETCH, T = digitalRead(T); ) \
    X("COM", COM, T = ~T) \
    X("S\"", SQUOTE, doSQuote()) \
    X(".\"", DOTQUOTE, doDotQuote()) \
    X("C,", CCOMMA, *(HERE++) = (BYTE)T; DROP1) \
    X("W,", WCOMMA, doWComma((WORD)T); DROP1) \
    X(",",  COMMA, doComma(T); DROP1) \
    X("A,", ACOMMA, doAComma(A); DROP1) \
    X("BYE", BYE, printString(" bye."); isBYE = 1) \

#define X(name, op, code) OP_ ## op,
typedef enum {
    OPCODES
} OPCODE_T;
#undef X

#define X(name, op, code) if (opcode == OP_ ## op) { strcpy(buf, #op); }
void printOpcode(BYTE opcode) {
    char buf[32];
    sprintf(buf, "%d", opcode);
    buf[0] = 0;
    OPCODES;
    printf("\n-op:%s(PC:%lx,T:%lx,N:%lx)-", buf, (UCELL)PC, T, N);
}
#undef X

#define X(name, op, code) if (strCmp(w, name) == 0) { return OP_ ## op; }
BYTE getOpcode(char* w) {
    OPCODES
        return 0xFF;
}

#undef X
#define X(name, op, code) case OP_ ## op: code; break;

void run(ADDR start, CELL max_cycles) {
    PC = start;
    while (1) {
        IR = *(PC++);
        if (debugOn) printOpcode(IR);
        if ((IR == OP_RET) && (RSP < 1)) { return; }
        switch (IR) {
            OPCODES
        default: 
            printStringF("-unknown opcode: %d ($%02x) at %04lx-", IR, IR, PC - 1);
            return;
        }
        if ((max_cycles) && (--max_cycles < 1)) { return; }
    }
}
#undef X

void push(CELL v) {
    if (DSP < STK_SZ) ++DSP;
    T = v;
}
CELL pop() {
    if (0 < DSP) --DSP;
    return dstk[DSP + 1];
}

void rpush(CELL v) {
    if (RSP < STK_SZ) ++RSP;
    R = v;
}
CELL rpop() {
    if (0 < RSP) --RSP;
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
        // throw(3);
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
        // throw(3);
    }
}

void doSQuote() {
    BYTE len = *(PC);
    push((CELL)PC);
    PC += (len + 2);
}

void doDotQuote() {
    BYTE len = *(PC);
    push((CELL)(PC + 1));
    push(len);
    PC += (len + 2);
    doType();
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

void doFor() {
    if (loopSP < LOOP_STK_SZ) {
        DO_LOOP_T* dp = &doStack[++loopSP];
        dp->startAddr = PC;
        dp->stop = T;
        dp->start = N;
        if (dp->stop < dp->start) {
            dp->stop = N;
            dp->start = T;
        }
        dp->index = dp->start;
        DROP2;
    }
}

void doI() { if (0 <= loopSP) { push(doStack[loopSP].index); } }
void doJ() { if (1 <= loopSP) { push(doStack[loopSP - 1].index); } }

void doNext() {
    if (0 <= loopSP) {
        DO_LOOP_T* dp = &doStack[loopSP];
        ++dp->index;
        if (dp->index < dp->stop) {
            PC = dp->startAddr;
        } else {
            --loopSP;
        }
    }
}

void doBegin() {
    if (loopSP < LOOP_STK_SZ) {
        DO_LOOP_T* dp = &doStack[++loopSP];
        dp->startAddr = PC;
    }
}

void doAgain() { if (0 <= loopSP) { PC = doStack[loopSP].startAddr; } }

void doWhile(int dropIt, int isUntil) {
    if (loopSP < 0) { return; }
    CELL x = (dropIt) ? pop() : T;
    if (isUntil) x = (x) ? 0 : 1;
    if (x) {
        PC = doStack[loopSP].startAddr;
    } else {
        if (!dropIt) pop();
        --loopSP;
    }
}

ADDR align4(ADDR x) {
    CELL y = (CELL)x;
    while (y % 4) { ++y; }
    return (ADDR)y;
}

DICT_T* isTempWord(const char* name) {
    if (strlen(name) != 3) return 0;
    if (name[0] != '_') return 0;
    if (name[1] != 't') return 0;
    if (name[2] < '0') return 0;
    if (name[2] > '9') return 0;
    return &tempWords[name[2] - '0'];
}

void doCreate(const char *name) {
    // printf("-cw:%s(%lx)-", name, HERE);
    DICT_T* dp = isTempWord(name);
    if (dp) {
        dp->XT = HERE;
        return;
    }

    HERE = align4(HERE);
    dp = (DICT_T*)HERE;
    dp->prev = (ADDR)LAST;
    dp->flags = 0;
    dp->len = (BYTE)strlen(name);
    strcpy(dp->name, name);
    LAST = HERE;
    HERE += (ADDR_SZ*2) + dp->len + 3;
    dp->XT = HERE;
}

int matches(char ch, char sep) {
    if (ch == sep) { return 1; }
    if ((sep == ' ') && (ch < sep)) { return 1; }
    return 0;
}

BYTE getNextChar() {
    return (*toIN) ? *(toIN++) : 0;
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
    // try {
        while (1) {
            char* w = (char*)HERE + 0x100;
            CELL len = getNextWord(w, sep);
            if (len == 0) { return; }
            push((CELL)w);
            doParseWord();
        }
    // }
    // catch (...) {
    //    printString("error caught");
    // }
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

int strCmp(const char* l, const char* r) {
    if (strlen(l) != strlen(r)) { return 1; }
    while (*l) {
        char x = *(l++), y = *(r++);
        // if (x != y) { return 1; }
        if (tolower(x) != tolower(y)) { return 1; }
    }
    return 0;
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
    *(addr++) = (val & 0xFF);
    *(addr++) = (val >>  8) & 0xFF;
    *(addr++) = (val >> 16) & 0xFF;
    *(addr)   = (val >> 24) & 0xFF;
}
void addrStore(ADDR addr, ADDR v) {
    (ADDR_SZ == 2) ? wordStore(addr, (CELL)v) : cellStore(addr, (CELL)v);
}

inline void doCComma(BYTE v) { *(HERE++) = v; }
void doWComma(CELL v) { wordStore(HERE, v); HERE += ADDR_SZ; }
void doComma(CELL v) { cellStore(HERE, v); HERE += CELL_SZ; }
void doAComma(ADDR v) {
    (ADDR_SZ == 2) ? doWComma((CELL)v) : doComma((CELL)v); 
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
            doCComma(bytes[i]);
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

    if (strCmp(w, "inline") == 0) {
        DICT_T *dp = (DICT_T *)LAST;
        dp->flags = 2;
        return 1;
    }

    if (strCmp(w, "immediate") == 0) {
        DICT_T *dp = (DICT_T *)LAST;
        dp->flags = 1;
        return 1;
    }

    return 0;
}

int doFind(const char *name) {         // opcode #65
    DICT_T* dp = isTempWord(name);
    if (dp) {
        push((CELL)dp);
        return 1; 
    }
    dp = (DICT_T*)LAST;
    while (dp) {
        if (strCmp(name, dp->name) == 0) {
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
    //printf("-%s-", w);
    //if (strCmp(w, "dump-dict") == 0) {
    //    int x = 1;
    //}
    int lwc = lastWasCall;
    lastWasCall = 0;
    if (doFind(w)) {
        DICT_T *dp = (DICT_T *)pop();
        ADDR xt = dp->XT;
        // 1 => IMMEDIATE?
        if (compiling(w, 0)) {
            if (dp->flags == 1) {
                run(xt, 0);
            } else {
                doCComma(OP_CALL);
                doAComma((ADDR)xt);
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
            CELL v = pop();
            if ((0x0000 <= v) && (v < 0x0100)) {
                doCComma(OP_CLIT);
                doCComma((BYTE)v);
            } else if ((0x0100 <= v) && (v < 0x010000)) {
                doCComma(OP_WLIT);
                doWComma((WORD)v);
            } else {
                doCComma(OP_LIT);
                doComma(v);
            }
        }
        return;
    }

    if (strCmp(w, "if") == 0) {
        if (! compiling(w, 1)) { return; }
        doCComma(OP_JMPZ);
        push((CELL)HERE);
        doAComma(0);
        return;
    }

    if (strCmp(w, "if-") == 0) {
        if (! compiling(w, 1)) { return; }
        doCComma(OP_NJMPZ);
        push((CELL)HERE);
        doAComma(0);
        return;
    }

    if (strCmp(w, "else") == 0) {
        if (! compiling(w, 1)) { return; }
        ADDR tgt = (ADDR)pop();
        doCComma(OP_JMP);
        push((CELL)HERE);
        doAComma(0);
        addrStore(tgt, HERE);
        return;
    }

    if (strCmp(w, "then") == 0) {
        if (! compiling(w, 1)) { return; }
        ADDR tgt = (ADDR)pop();
        addrStore(tgt, HERE);
        return;
    }

    if (strCmp(w, ":") == 0) {
        if (! interpreting(w, 1)) { return; }
        if (getNextWord(w, ' ')) {
            doCreate(w);
            STATE = 1;
        }
        return;
    }

    if (strCmp(w, ";") == 0) {
        if (!compiling(w, 1)) { return; }
        if (lwc && (*(HERE - ADDR_SZ - 1) == OP_CALL)) { *(HERE - ADDR_SZ - 1) = OP_JMP; }
        else { doCComma(OP_RET); }
        STATE = 0;
        return;
    }

    if (strCmp(w, "variable") == 0) {
        if (! interpreting(w, 1)) { return; }
        if (getNextWord(w, ' ')) {
            doCreate(w);
            doCComma(OP_LIT);
            doComma((CELL)HERE+CELL_SZ+1);
            doCComma(OP_RET);
            doComma(0);
        }
        return;
    }

    if (strCmp(w, "constant") == 0) {
        if (! interpreting(w, 1)) { return; }
        if (getNextWord(w, ' ')) {
            doCreate(w);
            doCComma(OP_LIT);
            doComma(pop());
            doCComma(OP_RET);
        }
        return;
    }

    if (strCmp(w, "s\"") == 0) {
        doCComma(OP_SQUOTE);
        ADDR la = HERE;
        BYTE len = 0;
        doCComma(0);
        char c = getNextChar();
        c = getNextChar();
        while (c && (c != '"')) {
            doCComma(c);
            ++len;
            c = getNextChar();
        }
        *(la) = len;
        doCComma(0);
        return;
    }

    if (strCmp(w, ".\"") == 0) {
        doCComma(OP_DOTQUOTE);
        ADDR la = HERE;
        BYTE len = 0;
        doCComma(0);
        char c = getNextChar();
        c = getNextChar();
        while (c && (c != '"')) {
            doCComma(c);
            ++len;
            c = getNextChar();
        }
        *(la) = len;
        doCComma(0);
        return;
    }

    BYTE op = getOpcode(w);
    if (op < 0xFF) {
        if (compiling(w, 0)) {
            doCComma(op);
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
    // throw(123);
}

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void parseLine(const char *line) {
    push((CELL)line);
    doParse(' ');
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

void vmInit() {
    // init_handlers();
    HERE = &dict[0];
    LAST = 0;
    BASE = 10;
    STATE = 0;
    DSP = 0;
    RSP = 0;
    TIB = TIBBuf;
    TIBEnd = TIBBuf;
    loopDepth = 0;
    doComma(0);
}

#ifdef __DEV_BOARD__
void loop() {
    while (mySerial.available()) {
        int c = mySerial.read();
        toTIB(c);
    }
    autoRun();
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
    doHistory(buf);
    for (int i = 0; i < len; i++) toTIB(buf[i]);
    toTIB(13);
}
#endif

void loadSourceF(const char* fmt, ...) {
    char buf[96];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    parseLine(buf);
}

void loadBaseSystem() {
    loadSourceF(": cell %d ; : cells cell * ; : addr %d ;", CELL_SZ, ADDR_SZ);
    loadSourceF(": dict $%lx ; : dict-sz $%lx ;", (long)&dict[0], DICT_SZ);
    loadSourceF(": (here) $%lx ; : here (here) @ ;", (long)&HERE);
    loadSourceF(": (last) $%lx ; : last (last) @ ;", (long)&LAST);
    loadSourceF(": base $%lx ;", (long)&BASE);
    loadSourceF(": state $%lx ;", (long)&STATE);
    loadSourceF(": tib $%lx ;", (long)&TIB[0]);
    loadSourceF(": (dsp) $%lx ; : (rsp) $%lx ;", (long)&DSP, (long)&RSP);
    loadSourceF(": dstack $%lx ;", (long)&dstk[0]);
    loadSourceF(": rstack $%lx ;", (long)&rstk[0]);

    parseLine(
        " : depth (dsp) @ 1- ; : 0sp 0 (dsp) ! ;"
        " : nip swap drop ; : tuck swap over ;"
        " : ?dup if- dup then ;"
        " : rot >r swap r> swap ; : -rot swap >r swap r> ;"
        " : 2dup over over ; : 2drop drop drop ;"
        " : mod /mod drop ; : / /mod nip ;"
        " : +! tuck @ + swap ! ;"
        " : <> = 0= ;"
        " : negate 0 swap - ;"
        " : off 0 swap ! ; : on 1 swap ! ;"
        " : abs dup 0 < if negate then ;"
        " : hex $10 base ! ; : decimal #10 base ! ; : binary %10 base ! ;"
        " : hex? base @ #16 = ; : decimal? base @ #10 = ;"
        " : bl #32 ; : space bl emit ; : cr #13 emit #10 emit ; : tab #9 emit ;"
        " : pad here $40 + ; : (neg) here $44 + ; "
        " : hold pad @ 1- dup pad ! c! ; "
        " : <# pad dup ! ; "
        " : # base @ u/mod swap abs '0' + dup '9' > if 7 + then hold ; "
        " : #s begin # while- ; "
        " : #> (neg) @ if '-' emit then pad @ pad over - type ; "
        " : is-neg? (neg) off base @ #10 = if dup 0 < if (neg) on negate then then ;"
        " : (.) is-neg? <# #s #> ; "
        " : (u.) (neg) off <# #s #> ; "
        " : . space (.) ; : u. space (u.) ; "
        " : .n >r is-neg? r> <# 0 for # next #> drop ;"
        " : .c decimal? if 3 .n else hex? if 2 .n else (.) then then ;");
    parseLine(
        " : low->high 2dup > if swap then ;"
        " : high->low 2dup < if swap then ;"
        " : min low->high drop ;"
        " : max high->low drop ;"
        " : between rot dup >r min max r> = ;"
        " : allot here + (here) ! ;"
        " : >body addr + a@ ;"
        " : count dup 1+ swap c@ ;"
        " : .wordl cr dup . space dup >body . addr 2* + dup c@ . 1+ space count type ;"
        " : wordsl last begin dup .wordl a@ while- ;"
        " : .word addr 2* + 1+ count type tab ;"
        " : words last begin dup .word a@ while- ;"
        " variable (regs) 9 cells allot"
        " : reg cells (regs) + ;"
        " : >src 0 reg ! ; : >dst 1 reg ! ;"
        " : src 0 reg @ ; : src+ src dup 1+ >src ;"
        " : dst 1 reg @ ; : dst+ dst dup 1+ >dst ;"
        " : dump low->high for i c@ space .c next ;"
        " : _t0 cr dup 8 .n ':' emit #16 over + dump ;"
        " : _t1 dup _t0 #16 + ;"
        " : dump-dict dict begin _t1 dup here < while drop ;"
        " : elapsed tick swap - 1000 /mod . '.' emit 3 .n .\"  seconds.\" ;"
        " variable (ch)  variable (cl)"
        " : marker here (ch) ! last (cl) ! ;"
        " : forget (ch) @ (here) ! (cl) @ (last) ! ;"
        " : forget-1 last (here) ! last a@ (last) ! ;"
        " marker");
}

void loadUserWords() {
    parseLine(
        " : auto-run-last last >body dict a! ;"
        " : auto-run-off 0 dict a! ;"
        " : k 1000 * ; : mil k k ;"
        " : bm tick swap begin 1- while- elapsed ;"
        " : bm2 >r tick 0 r> for next elapsed ;"
        " variable (led) 13 (led) ! : led (led) @ ;"
        " : led-on 1 led dp! ; : led-off 0 led dp! ;"
        " : blink led-on dup ms led-off dup ms ;"
        " : blinks 0 swap for blink next ;"
        " variable (button)  6 (button) ! : button (button) @ ;  : button-val button dp@ ;"
        " : button->led button-val if led-on else led-off then ;"
        " variable (pot)  3 (pot) !  : pot (pot) @ ; : pot-val pot ap@ ;"
        " variable pot-lastVal  variable sensitivity  4 sensitivity !"
        " : pot-changed? pot-lastVal @ - abs sensitivity @ > ;"
        " : .pot dup pot-lastVal ! . cr ;"
        " : .pot? pot-val dup pot-changed? if .pot else drop then ;"
        " : init led output-pin pot input-pin button input-pin ;"
        " : go button->led .pot? ;"
    );
    #ifdef __DEV_BOARD__
    // parseLine("init auto-run-last");
    #endif
}

void ok() {
    printString(" ok. ");
    doDotS();
    printString("\r\n");
}

void setup() {
#ifdef __DEV_BOARD__
    mySerial.begin(19200);
    while (!mySerial) {}
    while (mySerial.available()) {}
#else
    printString("BoardForth v0.0.1 - Chris Curl\n");
    printString("Source: https://github.com/CCurl/BoardForth \n");
    printStringF("Dictionary size is %ld bytes.", DICT_SZ);
#endif
    vmInit();
    loadBaseSystem();
    loadUserWords();
    parseLine(": x cr .\" Hello.\" ; x forget-1");
    numTIB = 0;
    TIBEnd = TIB;
    *TIBEnd = 0;
    ok();
}

#ifndef __DEV_BOARD__
int main()
{
    setup();
    while (true) {
        loop();
        if (isBYE) { break; }
    }
}
#endif
