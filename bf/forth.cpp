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

#define DBG_ALL   4
#define DBG_TRACE 3
#define DBG_INFO  2
#define DBG_DEBUG 1
#define DBG_OFF   0

#define NAME_LEN      25
#define DICT_ENTRY_SZ 32
typedef struct {
    ADDR XT;
    BYTE flags;
    BYTE len;
    char name[NAME_LEN+1];
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
void doWComma(CELL);
void doComma(CELL);
void doAComma(ADDR);
void dumpOpcodes();
int strCmp(const char*, const char*);

BYTE IR;
ADDR PC;
ADDR HERE, VHERE;
ADDR LAST;
BYTE dict[DICT_SZ];
BYTE vars[VARS_SZ];
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
CELL debugMode = DBG_OFF;
CELL numWords = 0;
extern char bootStrap[];

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
    X("JMP", JMP, PC = addrAt(PC)) \
    X("BRANCH", BRANCH, PC += *(PC)) \
    X("ZBRANCH",  ZBRANCH,  PC += (T == 0) ? *(PC) : 1; DROP1) \
    X("NZBRANCH", NZBRANCH, PC += (T == 0) ? *(PC) : 1) \
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
    X("EMIT", EMIT, buf[0] = (char)T; printString(buf); DROP1) \
    X(".S", DOTS, doDotS()) \
    X("FOR", FOR, doFor()) \
    X("I", I, doI()) \
    X("J", J, doJ()) \
    X("NEXT", NEXT, doNext()) \
    X("BEGIN", BEGIN, doBegin()) \
    X("AGAIN", AGAIN, doAgain()) \
    X("WHILE", WHILE, doWhile(1, 0)) \
    X("WHILE-", WHILEN, doWhile(0, 0)) \
    X("UNTIL", UNTIL, doWhile(1, 1)) \
    X("MS", DELAY, delay(pop())) \
    X("TICK", TICK, push(millis())) \
    X("C,", CCOMMA, *(HERE++) = (BYTE)T; DROP1) \
    X("W,", WCOMMA, doWComma((WORD)T); DROP1) \
    X(",",  COMMA, doComma(T); DROP1) \
    X("A,", ACOMMA, doAComma(A); DROP1) \
    X("(NUM-WORDS)", NUM_WORDS, push((CELL)&numWords)) \
    X("MALLOC", MALLOC, T = (CELL)malloc(T)) \
    X("FREE", MFREE, free((void *)T); DROP1) \
    X("FILL", FILL, memset((void *)M, T, N); DROP3) \
    X("ZCOUNT", ZCOUNT, push(T); T = strlen((char *)T)) \
    X("ZTYPE", ZTYPE, printString((char *)T); DROP1 ) \
    X("DEBUG-MODE", DEBUG_MODE, push((CELL)&debugMode)) \

#ifndef __FILES__
#define FILE_OPCODES
#else
CELL doFileOpen(const char* fn, const char* mode);
void doFileClose(CELL fp);
void doFileRead();
void doFileWrite();
#define FILE_OPCODES \
    X("FOPEN", FOPEN, N = doFileOpen((char *)N+1, (char *)T+1); DROP1) \
    X("FCLOSE", FCLOSE, doFileClose(T); DROP1) \
    X("FREAD", FREAD, doFileRead()) \
    X("FWRITE", FWRITE, doFileWrite())
#endif

#ifndef __LITTLEFS__
#define LITTLEFS_OPCODES
#else
CELL doFileOpen(const char* fn, const char* mode);
void doFileClose(CELL fp);
void doFileRead();
void doFileWrite();
#define LITTLEFS_OPCODES \
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

#ifndef __COM_PORT__
#define COMPORT_OPCODES
#else
CELL doComOpen(CELL portNum, CELL baud);
CELL doComRead(CELL handle);
CELL doComWrite(CELL handle, CELL ch);
#define COMPORT_OPCODES \
    X("COM-OPEN", COM_OPEN, N = doComOpen(T, N); DROP1) \
    X("COM-READ", COM_READ, T = doComRead(T)) \
    X("COM-WRITE", COM_WRITE, N = doComWrite(T, N); DROP1) \
    X("COM-CLOSE", COM_CLOSE, CloseHandle((HANDLE)T); DROP1)
#endif

#define OPCODES \
    BASE_OPCODES \
    FILE_OPCODES \
    LITTLEFS_OPCODES \
    ARDUINO_OPCODES \
    JOYSTICK_OPCODES \
    COMPORT_OPCODES \
    X("DUMP-OPCODES", DUMP_OPCODES, dumpOpcodes()) \
    X("BOOT-STRAP", BOOT_STRAP, push((CELL)&bootStrap[0])) \
    X("BYE", BYE, printString(" bye."); isBYE = 1)

#define X(name, op, code) OP_ ## op,
typedef enum {
    OP_NONE = 31,
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

#define X(name, op, code) printStringF("\n%3d ($%02x, %c): %s", OP_ ## op, OP_ ## op, OP_ ## op, name);
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
    char buf[2];
    buf[1] = 0;
    PC = start;
    while (1) {
        IR = *(PC++);
        if (DBG_TRACE <= debugMode) { printOpcode(IR); }
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
        if (dp->index < dp->stop) { PC = dp->startAddr; }
        else { --loopSP; }
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
    if (x) { PC = doStack[loopSP].startAddr; }
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
LittleFS_Program myFS;
File fp;
CELL doFileOpen(const char* fn, const char *mode) {
    int openMode = FILE_READ;
    fp = myFS.open(fn, openMode);
    return (CELL)&fp;
}

void doFileClose(CELL p) {
    File *fp = (File *)p;
    fp->close();
}

void doFileRead() {
    File* fp = (File*)pop();
    BYTE* buf = (BYTE*)pop();
    CELL sz = pop();
    CELL num = fp->read(buf, sz);
    push(num);
}

void doFileWrite() {
    File* fp = (File*)pop();
    BYTE* buf = (BYTE*)pop();
    CELL sz = pop();
    CELL num = fp->write(buf, sz);
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

#ifdef __COM_PORT__
CELL doComOpen(CELL portNum, CELL baud) {
    char fn[32];
    sprintf(fn, "\\\\.\\COM%d", (int)portNum);
    DWORD access = (GENERIC_READ | GENERIC_WRITE);
    HANDLE h = CreateFile(fn, access, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) return 0;
    DCB dcb;
    memset(&dcb, 0, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);
    GetCommState(h, &dcb);
    dcb.BaudRate = baud;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    SetCommState(h, &dcb);
    COMMTIMEOUTS to = { 0 };
    to.WriteTotalTimeoutConstant = 50;
    to.ReadTotalTimeoutConstant = 50;
    SetCommTimeouts(h, &to);
    return (CELL)h;
}

CELL doComRead(CELL handle) {
    char buf[2];
    DWORD num = 0;
    ReadFile((HANDLE)handle, buf, 1, &num, NULL);
    return (CELL)(num > 0 ? buf[0] : 0);
}

CELL doComWrite(CELL handle, CELL ch) {
    char buf[2];
    DWORD num;
    buf[0] = (char)ch;
    BOOL x = WriteFile((HANDLE)handle, buf, 1, &num, NULL);
    return num;
}

#endif

DICT_T* isTempWord(const char* name) {
    if (strlen(name) != 3) return 0;
    if (name[0] != '_') return 0;
    if (name[1] != 't') return 0;
    if (name[2] < '0') return 0;
    if (name[2] > '9') return 0;
    return &tempWords[name[2] - '0'];
}

void doCreate(const char* name) {
    DICT_T* dp = isTempWord(name);
    if (dp) {
        dp->XT = HERE;
        return;
    }

    ++numWords;
    LAST -= DICT_ENTRY_SZ;
    dp = (DICT_T*)LAST;
    if (DBG_DEBUG <= debugMode) { printStringF("-cw:%s(%ld)-", name, LAST); }
    dp->flags = 0;
    dp->len = (BYTE)strlen(name);
    if (dp->len > NAME_LEN) {
        printStringF("-name too long: %d-", dp->len);
        dp->len = NAME_LEN;
    }
    strncpy(dp->name, name, NAME_LEN);
    dp->name[NAME_LEN] = 0;
    dp->XT = HERE;
}

int matches(char ch, char sep) {
    if (ch == sep) { return 1; }
    if ((sep == ' ') && (ch < sep)) { return 1; }
    return 0;
}

inline BYTE getNextChar() {
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
    if (addr) { run(addr, 0); }
}

// ---------------------------------------------------------------------
void printString(const char* str) {
#ifdef __DEV_BOARD__
    printSerial(str);
#else
    fputs(str, stdout);
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

inline void doVComma(BYTE v) { *(VHERE++) = v; }
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
    for (int i = 0; i < numWords; i++) {
        if (strCmp(name, dp->name) == 0) {
            push((CELL)dp);
            return 1;
        }
        dp++;
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
    if (DBG_INFO <= debugMode) { printStringF("-%s-", w); }
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
        doCComma(OP_ZBRANCH);
        push((CELL)HERE);
        doCComma(0);
        return;
    }

    if (strCmp(w, "if-") == 0) {
        if (!compiling(w, 1)) { return; }
        doCComma(OP_NZBRANCH);
        push((CELL)HERE);
        doCComma(0);
        return;
    }

    if (strCmp(w, "else") == 0) {
        if (!compiling(w, 1)) { return; }
        ADDR tgt = (ADDR)pop();
        doCComma(OP_BRANCH);
        push((CELL)HERE);
        doCComma(0);
        BYTE offset = HERE - tgt;
        *(tgt) = offset;
        return;
    }

    if (strCmp(w, "then") == 0) {
        if (!compiling(w, 1)) { return; }
        ADDR tgt = (ADDR)pop();
        BYTE offset = HERE - tgt;
        *(tgt) = offset;
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
            doComma((CELL)VHERE);
            doCComma(OP_RET);
            VHERE += CELL_SZ;
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
        doCComma(OP_LIT);
        doComma((CELL)VHERE);
        ADDR la = VHERE;
        BYTE len = 0;
        doVComma(0);
        char c = getNextChar();
        c = getNextChar();
        while (c && (c != '"')) {
            doVComma(c);
            ++len;
            c = getNextChar();
        }
        *(la) = len;
        doVComma(0);
        return;
    }

    if (strCmp(w, "(") == 0) {
        char c = getNextChar();
        while (c && (c != ')')) {
            c = getNextChar();
        }
        return;
    }

    if (strCmp(w, "z\"") == 0) {
        if (STATE == 0) {
            getNextChar();
            push((CELL)TMP_SQUOTE);
            getNextWord((char *)T, '"');
            getNextChar();
            return;
        }
        doCComma(OP_LIT);
        doComma((CELL)VHERE);
        char c = getNextChar();
        c = getNextChar();
        while (c && (c != '"')) {
            doVComma(c);
            c = getNextChar();
        }
        doVComma(0);
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
        doCComma(OP_LIT);
        doComma((CELL)VHERE);
        doCComma(OP_ZTYPE);
        char c = getNextChar();
        c = getNextChar();
        while (c && (c != '"')) {
            doVComma(c);
            c = getNextChar();
        }
        doVComma(0);
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
    VHERE = &vars[0];
    LAST = HERE + (DICT_SZ-4);
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
    loadSourceF(": vars $%lx ; : vars-sz $%lx ;", (long)&vars[0], VARS_SZ);
    loadSourceF(": (here) $%lx ; : here (here) @ ;", (long)&HERE);
    loadSourceF(": (last) $%lx ; : last (last) @ ;", (long)&LAST);
    loadSourceF(": (vhere) $%lx ; : vhere (vhere) @ ;", (long)&VHERE);
    loadSourceF(": base $%lx ;", (long)&BASE);
    loadSourceF(": state $%lx ;", (long)&STATE);
    loadSourceF(": tib $%lx ;", (long)&TIB[0]);
    loadSourceF(": (dsp) $%lx ; : (rsp) $%lx ;", (long)&DSP, (long)&RSP);
    loadSourceF(": dstack $%lx ;", (long)&dstk[0]);
    loadSourceF(": rstack $%lx ;", (long)&rstk[0]);

    parseLine(bootStrap);
}

#ifdef __FILES__
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
#elif __LITTLEFS__
void loadUserWords() {}
#else
void loadUserWords() {}
#endif

void ok() {
    #ifdef __DEV_BOARD__
    printString(" OK ");
    #else 
    printString(" ok ");
    #endif
    doDotS();
    printString("\r\n");
}

void setup() {
#ifdef __DEV_BOARD__
    mySerial.begin(19200);
    while (!mySerial) {}
    while (mySerial.available()) {}
#ifdef __LITTLEFS__
    myFS.begin(1024*1024);
#endif
#endif
    vmInit();
    loadBaseSystem();
    #ifndef __DEV_BOARD__
    loadUserWords();
    #endif
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

char bootStrap[] = ": depth (dsp) @ 1- ; : 0sp 0 (dsp) ! ;"
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
"\n: bl #32 ; : space #32 emit ; : cr #13 emit #10 emit ; : tab #9 emit ;"
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
"\n: .c decimal? if 3 .n else hex? if 2 .n else (.) then then ;"
"\n: low->high 2dup > if swap then ;"
"\n: high->low 2dup < if swap then ;"
"\n: min low->high drop ;"
"\n: max high->low drop ;"
"\n: between rot dup >r min max r> = ;"
"\n: allot vhere + (vhere) ! ;"
"\n: >body addr + a@ ;"
"\n: count dup 1+ swap c@ ;"
"\n: num-words (num-words) @ ;"
"\n: .wordl cr dup . dup a@ . addr + dup c@ . 1+ dup c@ . space count type ;"
"\n: wordsl last num-words 1 for dup .wordl #32 + next drop ;"
"\n: .word addr + 1+ count type tab ;"
"\n: words last num-words 1 for dup .word #32 + next drop ;"
"\nvariable (regs) 9 cells allot"
"\n: reg cells (regs) + ;"
"\n: >src 0 reg ! ; : >dst 1 reg ! ;"
"\n: src 0 reg @ ; : src+ src dup 1+ >src ;"
"\n: dst 1 reg @ ; : dst+ dst dup 1+ >dst ;"
"\n: dump low->high for i c@ space .c next ;"
"\n: _t0 cr dup 8 .n ':' emit #16 over + dump ;"
"\n: _t1 dup _t0 #16 + ;"
"\n: dump-dict dict begin _t1 dup here < while drop ;"
"\n: elapsed tick swap - 1000 /mod . '.' emit 3 .n .\"  seconds.\" ;"
"\nvariable (ch)  variable (cl) variable (nw) variable (vh)"
"\n: marker here (ch) ! last (cl) ! num-words (nw) ! vhere (vh) ! ;"
"\n: forget (ch) @ (here) ! (cl) @ (last) ! (nw) @ (num-words) ! (vh) @ (vhere) ! ;"
"\n: forget-1 last a@ (here) ! last 32 + (last) ! (num-words) @ 1- (num-words) ! ;"
"\nmarker";
