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
void analogWrite(int pin, int val) { printStringF("-ap!:%d/%d-", pin, val); }
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
#define M dstk[DSP-2]
#define R rstk[RSP]
#define A ((ADDR)dstk[DSP])
#define DROP1 --DSP
#define DROP2 DROP1; DROP1
#define DROP3 DROP1; DROP1; DROP1

#define TMP_RUNOP    (HERE + 0x0020)
#define TMP_PAD      (HERE + 0x0040)
#define TMP_WORD     (HERE + 0x0080)
#define TMP_SQUOTE   (HERE + 0x0100)
#define TMP_DOTQUOTE (HERE + 0x0200)

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
int doNumber(const char*);
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
void dumpOpcodes();
CELL doFileOpen(const char *fn, const char *mode);
void doFileClose(CELL fp);
void doFileRead();
void doFileWrite();
int strCmp(const char*, const char*);

BYTE IR;
ADDR PC;
ADDR HERE;
ADDR LAST;
BYTE dict[DICT_SZ];
char TIBBuf[TIB_SZ];
char* TIB = TIBBuf;
char* toIN, * TIBEnd;
CELL BASE, STATE, DSP, RSP;
CELL dstk[STK_SZ + 1];
CELL rstk[STK_SZ + 1];
CELL loopDepth, t1;
int numTIB = 0;
int lastWasCall = 0;
DO_LOOP_T doStack[LOOP_STK_SZ];
int loopSP = -1;
int isBYE = 0, isError = 0;
DICT_T tempWords[10];
int debugOn = 0;
extern char screen1[], screen2[], screen3[];

#ifndef __DEV_BOARD__
#pragma warning(disable:4996)
void pinMode(int pin, int mode) {}
int digitalRead(int pin) { return pin + 1; }
int analogRead(int pin) { return pin + 1; }
void digitalWrite(int pin, int val) { printStringF("-dp!:%d/%d-", pin, val); }
void analogWrite(int pin, int val) { printStringF("-ap!:%d/%d-", pin, val); }
void delay(int ms) { Sleep(ms); }
long millis() { return GetTickCount(); }
#endif

#define BASE_OPCODES \
    X("NOOP", NOOP, ) \
    X("CALL", CALL, rpush((CELL)PC + ADDR_SZ); PC = addrAt(PC)) \
    X("RET", RET, PC = (ADDR)rpop()) \
    X("JMP",    JMP,    PC = addrAt(PC)) \
    X("JMPZ",   JMPZ,   PC = (T == 0) ? addrAt(PC) : PC + ADDR_SZ; DROP1) \
    X("NJMPZ",  NJMPZ,  PC = (T == 0) ? addrAt(PC) : PC + ADDR_SZ) \
    X("BLIT", BLIT, push(*(PC++))) \
    X("WLIT", WLIT, push(wordAt(PC)); PC += WORD_SZ) \
    X("LIT",  LIT, push(cellAt(PC)); PC += CELL_SZ) \
    X("DUP", DUP, push(T)) \
    X("SWAP", SWAP, t1 = T; T = N; N = t1) \
    X("DROP", DROP, DROP1) \
    X("OVER", OVER, push(N)) \
    X("NIP", NIP, N = T; DROP1) \
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
    X("AND", AND, N &= T; DROP1) \
    X("NAND", NAND, N = (N & T) ? 0 : 1; DROP1) \
    X("OR", OR, N |= T; DROP1) \
    X("XOR", XOR, N ^= T; DROP1) \
    X("COM", COM, T = ~T) \
    X("<", LESS, N = (N < T) ? 1 : 0; DROP1) \
    X("=", EQUALS, N = (N == T) ? 1 : 0; DROP1) \
    X("<>", NEQUALS, N = (N != T) ? 1 : 0; DROP1) \
    X("0=", ZEQUALS, T = (T == 0) ? 1 : 0) \
    X(">", GREATER, N = (N > T) ? 1 : 0; DROP1) \
    X(">r", DTOR, rpush(pop())) \
    X("r@", RFETCH, push(R)) \
    X("r>", RTOD, push(rpop())) \
    X("TYPE", TYPE, doType() ) \
    X("EMIT", EMIT, printStringF("%c", (char)pop())) \
    X(".S", DOTS, doDotS()) \
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
    X("MS", DELAY, delay(pop())) \
    X("TICK", TICK, push(millis())) \
    X("S\"", SQUOTE, doSQuote()) \
    X(".\"", DOTQUOTE, doDotQuote()) \
    X("screen-1", SCREEN1, push((CELL)&screen1[0])) \
    X("screen-2", SCREEN2, push((CELL)&screen2[0])) \
    X("screen-3", SCREEN3, push((CELL)&screen3[0])) \
    X("C,", CCOMMA, *(HERE++) = (BYTE)T; DROP1) \
    X("W,", WCOMMA, doWComma((WORD)T); DROP1) \
    X(",",  COMMA, doComma(T); DROP1) \
    X("A,", ACOMMA, doAComma(A); DROP1) \
    X("MALLOC", MALLOC, T = (CELL)malloc(T)) \
    X("FILL", FILL, memset((void *)T, N, M); DROP3) \
    X("ZCOUNT", ZCOUNT, push(T); T = strlen((char *)T)) \

#ifndef __FILES__
#define FILE_OPCODES
#else
#define FILE_OPCODES \
    X("FOPEN", FOPEN, N = doFileOpen((char *)N+1, (char *)T+1); DROP1) \
    X("FCLOSE", FCLOSE, doFileClose(T); DROP1) \
    X("FREAD", FREAD, doFileRead()) \
    X("FWRITE", FWRITE, doFileWrite())
#endif

#ifdef __LITTLEFS__
#undef FILE_OPCODES
#define FILE_OPCODES \
    X("FOPEN", FOPEN, N = doFileOpen((char *)N+1, (char *)T+1); DROP1) \
    X("FCLOSE", FCLOSE, doFileClose(T); DROP1) \
    X("FREAD", FREAD, doFileRead()) \
    X("FWRITE", FWRITE, doFileWrite())
#endif

#ifndef __ARDUINO__
#define ARDUINO_OPCODES
#else
#define ARDUINO_OPCODES \
    X("INPUT-PIN", INPUT_PIN, pinMode(T, PIN_INPUT);        DROP1) \
    X("INPUT-PULLUP", INPUT_PIN_PULLUP, pinMode(T, PIN_INPUT_PULLUP); DROP1) \
    X("OUTPUT-PIN", OUTPUT_PIN, pinMode(T, PIN_OUTPUT);       DROP1) \
    X("ap!", APIN_STORE, analogWrite((int)T, (int)N);  DROP2) \
    X("dp!", DPIN_STORE, digitalWrite((int)T, (int)N); DROP2) \
    X("ap@", APIN_FETCH, T = analogRead((int)T); ) \
    X("dp@", DPIN_FETCH, T = digitalRead((int)T); )
#endif

// NB: This is for the Joystick library in the Teensy
#ifndef __JOYSTICK__
#define JOYSTICK_OPCODES
#else
void doJoyXYZ(int which, CELL val);
#define JOYSTICK_OPCODES \
    X("JOY-X", JOY_X, doJoyXYZ(1, T); DROP1) \
    X("JOY-Y", JOY_Y, doJoyXYZ(2, T); DROP1) \
    X("JOY-Z", JOY_Z, doJoyXYZ(3, T); DROP1) \
    X("JOY-ZROTATE", JOY_ZROTATE, Joystick.Zrotate(T); DROP1) \
    X("JOY-SLIDERLEFT", JOY_SLIDERLEFT, Joystick.sliderLeft(T); DROP1) \
    X("JOY-SLIDERRIGHT", JOY_SLIDERRIGHT, Joystick.sliderRight(T); DROP1) \
    X("JOY-BUTTON", JOY_BUTTON, Joystick.button(T, N); DROP2) \
    X("JOY-USEMANUAL", JOY_USEMANUAL, Joystick.useManualSend(T); DROP1) \
    X("JOY-SENDNOW", JOY_SENDNOW, Joystick.send_now();)
#endif

#define OPCODES \
    BASE_OPCODES \
    FILE_OPCODES \
    ARDUINO_OPCODES \
    JOYSTICK_OPCODES \
    X("DUMP-OPCODES", DUMP_OPCODES, dumpOpcodes()) \
    X("BYE", BYE, printString(" bye."); isBYE = 1)

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

#define X(name, op, code) printStringF("\n%3d ($%02x): %s", OP_ ## op, OP_ ## op, name);
void dumpOpcodes() {
    OPCODES
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
        }
        else {
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
    }
    else {
        if (!dropIt) pop();
        --loopSP;
    }
}

#ifdef __FILES__
CELL doFileOpen(const char *fn, const char *mode) {
    FILE* fp = fopen(fn, mode);
    return (CELL)fp;
}

void doFileClose(CELL fp) {
    fclose((FILE*)fp);
}

void doFileRead() {
    FILE* fp = (FILE *)pop();
    CELL sz = pop();
    BYTE* buf = (BYTE*)pop();
    CELL num = fread(buf, 1, sz, fp);
    push(num);
}

void doFileWrite() {
    FILE* fp = (FILE*)pop();
    CELL sz = pop();
    BYTE* buf = (BYTE*)pop();
    CELL num = fwrite(buf, 1, sz, fp);
    push(num);
}
#endif

#ifdef __LITTLEFS__
CELL doFileOpen(const char* fn, const char* mode) {
    FILE* fp = fopen(fn, mode);
    return (CELL)fp;
}

void doFileClose(CELL fp) {
    fclose(fp);
}

void doFileRead() {
    FILE* fp = (FILE*)pop();
    BYTE* buf = (BYTE*)pop();
    CELL sz = pop();
    CELL num = fread(buf, 1, sz, fp);
    push(num);
}

void doFileWrite() {
    FILE* fp = (FILE*)pop();
    BYTE* buf = (BYTE*)pop();
    CELL sz = pop();
    CELL num = fwrite(buf, 1, sz, fp);
    push(num);
}
#endif

#ifdef __JOYSTICK__
void doJoyXYZ(int which, CELL val) {
    switch (which) {
    case 1: Joystick.X(val); break;
    case 2: Joystick.Y(val); break;
    case 3: Joystick.Z(val); break;
    }
}
#endif

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

void doCreate(const char* name) {
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
    HERE += (ADDR_SZ * 2) + dp->len + 3;
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

CELL getNextWord(char* to, char sep) {
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
    isError = 0;
    toIN = (char*)pop();
    TIBEnd = toIN + strlen(toIN);
    // try {
    while (1) {
        char* w = (char*)TMP_WORD;
        CELL len = getNextWord(w, sep);
        if (len == 0) { return; }
        push((CELL)w);
        doParseWord();
        if (isError) { return; }
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
void printString(const char* str) {
#ifdef __DEV_BOARD__
    printSerial(str);
#else
    printf("%s", str);
#endif
}

// ---------------------------------------------------------------------
void printStringF(const char* fmt, ...) {
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
#ifdef __NEEDS_ALIGN__
    return (*(loc + 3) << 24) + (*(loc + 2) << 16) + (*(loc + 1) << 8) + *(loc);
#else
    return *(CELL*)loc;
#endif
}
CELL wordAt(ADDR loc) {
#ifdef __NEEDS_ALIGN__
    return (*(loc + 1) << 8) + *(loc);
#else
    return *(WORD*)loc;
#endif
}
ADDR addrAt(ADDR loc) {
    return (ADDR_SZ == 2) ? (ADDR)wordAt(loc) : (ADDR)cellAt(loc);
}

void wordStore(ADDR addr, CELL val) {
#ifdef __NEEDS_ALIGN__
    * (addr) = (val & 0xFF);
    *(addr + 1) = (val >> 8) & 0xFF;
#else
    * (WORD*)addr = (WORD)val;
#endif
}
void cellStore(ADDR addr, CELL val) {
#ifdef __NEEDS_ALIGN__
    * (addr++) = (val & 0xFF);
    *(addr++) = (val >> 8) & 0xFF;
    *(addr++) = (val >> 16) & 0xFF;
    *(addr) = (val >> 24) & 0xFF;
#else
    * (CELL*)addr = val;
#endif
}
void addrStore(ADDR addr, ADDR v) {
    (ADDR_SZ == 2) ? wordStore(addr, (CELL)v) : cellStore(addr, (CELL)v);
}

inline void doCComma(BYTE v) { *(HERE++) = v; }
void doWComma(CELL v) { wordStore(HERE, v); HERE += WORD_SZ; }
void doComma(CELL v) { cellStore(HERE, v); HERE += CELL_SZ; }
void doAComma(ADDR v) {
    (ADDR_SZ == 2) ? doWComma((CELL)v) : doComma((CELL)v);
}

int compiling(char* w, int errIfNot) {
    if ((STATE == 0) && (errIfNot)) {
        printStringF("[%s]: Compile only.", w);
    }
    return (STATE == 1) ? 1 : 0;
}

int interpreting(char* w, int errIfNot) {
    if ((STATE != 0) && (errIfNot)) {
        printStringF("[%s]: Interpreting only.", w);
    }
    return (STATE == 0) ? 1 : 0;
}

int isInlineWord(char* w) {

    if (strCmp(w, "inline") == 0) {
        DICT_T* dp = (DICT_T*)LAST;
        dp->flags = 2;
        return 1;
    }

    if (strCmp(w, "immediate") == 0) {
        DICT_T* dp = (DICT_T*)LAST;
        dp->flags = 1;
        return 1;
    }

    return 0;
}

int doFind(const char* name) {         // opcode #65
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

int doNumber(const char* w) {
    if ((*(w) == '\'') && (*(w + 2) == '\'') && (*(w + 3) == 0)) {
        push(*(w + 1));
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
void doParseWord() {
    char* w = (char*)pop();
    //printf("-%s-", w);
    //if (strCmp(w, "dump-dict") == 0) {
    //    int x = 1;
    //}
    int lwc = lastWasCall;
    lastWasCall = 0;
    if (doFind(w)) {
        DICT_T* dp = (DICT_T*)pop();
        ADDR xt = dp->XT;
        // 1 => IMMEDIATE?
        if (compiling(w, 0)) {
            if (dp->flags == 1) {
                run(xt, 0);
            }
            else {
                doCComma(OP_CALL);
                doAComma((ADDR)xt);
                lastWasCall = 1;
            }
        }
        else {
            run(xt, 0);
        }
        return;
    }

    if (isInlineWord(w)) { return; }

    if (doNumber(w)) {
        if (compiling(w, 0)) {
            CELL v = pop();
            if ((0x0000 <= v) && (v < 0x0100)) {
                doCComma(OP_BLIT);
                doCComma((BYTE)v);
            }
            else if ((0x0100 <= v) && (v < 0x010000)) {
                doCComma(OP_WLIT);
                doWComma((WORD)v);
            }
            else {
                doCComma(OP_LIT);
                doComma(v);
            }
        }
        return;
    }

    if (strCmp(w, "if") == 0) {
        if (!compiling(w, 1)) { return; }
        doCComma(OP_JMPZ);
        push((CELL)HERE);
        doAComma(0);
        return;
    }

    if (strCmp(w, "if-") == 0) {
        if (!compiling(w, 1)) { return; }
        doCComma(OP_NJMPZ);
        push((CELL)HERE);
        doAComma(0);
        return;
    }

    if (strCmp(w, "else") == 0) {
        if (!compiling(w, 1)) { return; }
        ADDR tgt = (ADDR)pop();
        doCComma(OP_JMP);
        push((CELL)HERE);
        doAComma(0);
        addrStore(tgt, HERE);
        return;
    }

    if (strCmp(w, "then") == 0) {
        if (!compiling(w, 1)) { return; }
        ADDR tgt = (ADDR)pop();
        addrStore(tgt, HERE);
        return;
    }

    if (strCmp(w, ":") == 0) {
        if (!interpreting(w, 1)) { return; }
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
        if (!interpreting(w, 1)) { return; }
        if (getNextWord(w, ' ')) {
            doCreate(w);
            doCComma(OP_LIT);
            doComma((CELL)HERE + CELL_SZ + 1);
            doCComma(OP_RET);
            doComma(0);
        }
        return;
    }

    if (strCmp(w, "constant") == 0) {
        if (!interpreting(w, 1)) { return; }
        if (getNextWord(w, ' ')) {
            doCreate(w);
            doCComma(OP_LIT);
            doComma(pop());
            doCComma(OP_RET);
        }
        return;
    }

    if (strCmp(w, "s\"") == 0) {
        if (STATE == 0) {
            getNextChar();
            push((CELL)TMP_SQUOTE);
            char* x = (char*)T;
            CELL len = getNextWord(x + 1, '"');
            getNextChar();
            *x = (BYTE)len;
            return;
        }
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
        if (STATE == 0) {
            getNextChar();
            push((CELL)TMP_DOTQUOTE);
            char* x = (char *)T;
            CELL len = getNextWord(x, '"');
            getNextChar();
            push(len);
            doType();
            return;
        }
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
        }
        else {
            ADDR xt = TMP_RUNOP;
            *(xt) = op;
            *(xt + 1) = OP_RET;
            run(xt, 0);
        }
        return;
    }
    STATE = 0;
    printStringF("[%s]??", w);
    isError = 1;
    // throw(123);
}

// ---------------------------------------------------------------------
void parseLine(const char* line) {
    push((CELL)line);
    doParse(' ');
}

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

    parseLine(screen1);
    parseLine(screen2);
    parseLine(screen3);
}

void loadUserWords() {
    CELL fp = doFileOpen("user-words.4th", "rb");
    if (fp) {
        char* fc = (char *)malloc(MAX_FILE_SZ);
        if (fc) { memset(fc, 0, MAX_FILE_SZ); }
        push((CELL)fc);
        push(MAX_FILE_SZ);
        push(fp);
        doFileRead();
        doFileClose(fp);
        if (pop()) { parseLine(fc); }
        free(fc);
    }
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

char screen1[] = ": depth (dsp) @ 1- ; : 0sp 0 (dsp) ! ;"
"\n: tuck swap over ;"
"\n: ?dup if- dup then ;"
"\n: rot >r swap r> swap ; : -rot swap >r swap r> ;"
"\n: 2dup over over ; : 2drop drop drop ;"
"\n: mod /mod drop ; : / /mod nip ;"
"\n: +! tuck @ + swap ! ;"
"\n: negate 0 swap - ;"
"\n: off 0 swap ! ; : on 1 swap ! ;"
"\n: abs dup 0 < if negate then ;"
"\n: hex $10 base ! ; : decimal #10 base ! ; : binary %10 base ! ;"
"\n: hex? base @ #16 = ; : decimal? base @ #10 = ;"
"\n: bl #32 ; : space bl emit ; : cr #13 emit #10 emit ; : tab #9 emit ;"
"\n: pad here $40 + ; : (neg) here $44 + ; "
"\n: hold pad @ 1- dup pad ! c! ; "
"\n: <# pad dup ! ; "
"\n: # base @ u/mod swap abs '0' + dup '9' > if 7 + then hold ; "
"\n: #s begin # while- ; "
"\n: #> (neg) @ if '-' emit then pad @ pad over - type ; "
"\n: is-neg? (neg) off base @ #10 = if dup 0 < if (neg) on negate then then ;"
"\n: (.) is-neg? <# #s #> ; "
"\n: (u.) (neg) off <# #s #> ; "
"\n: . space (.) ; : u. space (u.) ; "
"\n: .n >r is-neg? r> <# 0 for # next #> drop ;"
"\n: .c decimal? if 3 .n else hex? if 2 .n else (.) then then ;";

char screen2[] = ": ?dup if- dup then ;"
"\n: rot >r swap r> swap ; : -rot swap >r swap r> ;"
"\n: 2dup over over ; : 2drop drop drop ;"
"\n: mod /mod drop ; : / /mod nip ;"
"\n: +! tuck @ + swap ! ;"
"\n: negate 0 swap - ;"
"\n: off 0 swap ! ; : on 1 swap ! ;"
"\n: abs dup 0 < if negate then ;"
"\n: hex $10 base ! ; : decimal #10 base ! ; : binary %10 base ! ;"
"\n: hex? base @ #16 = ; : decimal? base @ #10 = ;"
"\n: bl #32 ; : space bl emit ; : cr #13 emit #10 emit ; : tab #9 emit ;"
"\n: pad here $40 + ; : (neg) here $44 + ; "
"\n: hold pad @ 1- dup pad ! c! ; "
"\n: <# pad dup ! ; "
"\n: # base @ u/mod swap abs '0' + dup '9' > if 7 + then hold ; "
"\n: #s begin # while- ; "
"\n: #> (neg) @ if '-' emit then pad @ pad over - type ; "
"\n: is-neg? (neg) off base @ #10 = if dup 0 < if (neg) on negate then then ;"
"\n: (.) is-neg? <# #s #> ; "
"\n: (u.) (neg) off <# #s #> ; "
"\n: . space (.) ; : u. space (u.) ; "
"\n: .n >r is-neg? r> <# 0 for # next #> drop ;"
"\n: .c decimal? if 3 .n else hex? if 2 .n else (.) then then ;";

char screen3[] = ": low->high 2dup > if swap then ;"
"\n: high->low 2dup < if swap then ;"
"\n: min low->high drop ;"
"\n: max high->low drop ;"
"\n: between rot dup >r min max r> = ;"
"\n: allot here + (here) ! ;"
"\n: >body addr + a@ ;"
"\n: count dup 1+ swap c@ ;"
"\n: .wordl cr dup . space dup >body . addr 2* + dup c@ . 1+ space count type ;"
"\n: wordsl last begin dup .wordl a@ while- ;"
"\n: .word addr 2* + 1+ count type tab ;"
"\n: words last begin dup .word a@ while- ;"
"\nvariable (regs) 9 cells allot"
"\n: reg cells (regs) + ;"
"\n: >src 0 reg ! ; : >dst 1 reg ! ;"
"\n: src 0 reg @ ; : src+ src dup 1+ >src ;"
"\n: dst 1 reg @ ; : dst+ dst dup 1+ >dst ;"
"\n: dump low->high for i c@ space .c next ;"
"\n: _t0 cr dup 8 .n ':' emit #16 over + dump ;"
"\n: _t1 dup _t0 #16 + ;"
"\n: dump-dict dict begin _t1 dup here < while drop ;"
"\n: elapsed tick swap - 1000 /mod . '.' emit 3 .n .\" seconds.\" ;"
"\nvariable (ch)  variable (cl)"
"\n: marker here (ch) ! last (cl) ! ;"
"\n: forget (ch) @ (here) ! (cl) @ (last) ! ;"
"\n: forget-1 last (here) ! last a@ (last) ! ;"
"\nmarker";
