// ---------------------------------------------------------------------
// forth.c
// ---------------------------------------------------------------------

#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

const PROGMEM char xxx[] = "yyy";

#ifdef __ESP32__
void printStringF(const char *, ...);
void analogWrite(int pin, int val) { printStringF("-apin!:%d/%d-", pin, val); }
#endif

typedef long CELL;
typedef unsigned long UCELL;
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
#define C ((char *)dstk[DSP])
#define N0 dstk[DSP]
#define N1 dstk[DSP-1]
#define N2 dstk[DSP-2]
#define N3 dstk[DSP-3]
#define N4 dstk[DSP-4]

#define DROP1 --DSP
#define DROP2 DROP1; DROP1
#define DROP3 DROP1; DROP1; DROP1

#define TMP_RUNOP    (VHERE + 0x0002)
#define TMP_WORD     (VHERE + 0x0004)
#define TMP_SQUOTE   (VHERE + 0x0030)
#define TMP_DOTQUOTE (VHERE + 0x0030)
#define TMP_PAD      (LAST  - 0x0004)

#define DBG_ALL   4
#define DBG_TRACE 3
#define DBG_INFO  2
#define DBG_DEBUG 1
#define DBG_OFF   0

#define NAME_LEN      12
#define DICT_ENTRY_SZ 20

typedef struct {
    ADDR XT;
    BYTE flags;
    BYTE len;
    char name[NAME_LEN+1];
    BYTE lex;
} DICT_T;

typedef struct {
    char name[12];
} LEXICON_T;

LEXICON_T lex[LEX_SZ];
int lexCount = 0;
int currentLex = 0;

#define LOOP_STK_SZ 8
typedef struct {
    ADDR loopStart;
    ADDR loopEnd;
    CELL start;
    CELL index;
    CELL stop;
} LOOP_T;

CELL cellAt(ADDR);
CELL wordAt(ADDR);
ADDR addrAt(ADDR);
void cellStore(ADDR addr, CELL val);
void wordStore(ADDR addr, CELL val);
void addrStore(ADDR addr, ADDR val);
void printString(const char*);
void printStringF(const char*, ...);
void ok();
void doParse(char sep);
int isNumber(const char*);
void doParseWord();
void doCreate(const char* name);
void doSlMod();
void doUSlMod();
void doType();
CELL getNextWord(char *to, char sep);
int doFind(char *name);
void doDot(CELL num, int inUnsigned, int space, int width);
void doDotS();
void doFor();
void doNext();
void doBreak();
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
LOOP_T doStack[LOOP_STK_SZ];
int loopSP = -1;
int isBYE = 0, isError = 0;
DICT_T tempWords[10];
CELL debugMode = DBG_OFF;
CELL numWords = 0;
extern const char *bootStrap[];

#ifdef __IS_PC__
#pragma warning(disable:4996)
void pinMode(int pin, int mode) { printStringF("-pinMode(%d,%d)-", pin, mode); }
int digitalRead(int pin) { printStringF("-dread(%d)-", pin); return pin + 1; }
int analogRead(int pin) { printStringF("-aread(%d)-", pin); return pin + 1; }
void digitalWrite(int pin, int val) { printStringF("-dwrite(%d,%d)-", pin, val); }
void analogWrite(int pin, int val) { printStringF("-awrite(%d,%d)-", pin, val); }
void delay(int ms) { Sleep(ms); }
long millis() { return GetTickCount(); }
#endif

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

#define BASE_OPCODES \
    X("NOP", NOP, ) \
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
    X("COUNT", COUNT, push(T); N += 1; T = (*A)) \
    X("TYPE", TYPE, doType() ) \
    X("EMIT", EMIT, buf[0] = (char)T; printString(buf); DROP1) \
    X("(.)", PDOT, doDot(T, 0, 0, 0); DROP1) \
    X(".", DOT,    doDot(T, 0, 1, 0); DROP1) \
    X("U.", UDOT,  doDot(T, 1, 1, 0); DROP1) \
    X(".N", NDOT,  doDot(N, 1, 0, T); DROP2) \
    X(".S", DOTS, doDotS()) \
    X("FOR", FOR, doFor()) \
    X("I", I, if (0 <= loopSP) push(doStack[loopSP].index)) \
    X("J", J, if (1 <= loopSP) push(doStack[loopSP-1].index)) \
    X("K", K, if (2 <= loopSP) push(doStack[loopSP-2].index)) \
    X("NEXT", NEXT, doNext()) \
    X("BREAK", BREAK, doBreak()) \
    X("BEGIN", BEGIN, doBegin()) \
    X("AGAIN", AGAIN, doAgain()) \
    X("WHILE", WHILE, doWhile(1, 0)) \
    X("WHILE-", WHILEN, doWhile(0, 0)) \
    X("UNTIL", UNTIL, doWhile(1, 1)) \
    X("MS", DELAY, delay(pop())) \
    X("TIMER", TIMER, push(millis())) \
    X("C,", CCOMMA, *(HERE++) = (BYTE)T; DROP1) \
    X("W,", WCOMMA, doWComma((WORD)T); DROP1) \
    X(",",  COMMA, doComma(T); DROP1) \
    X("A,", ACOMMA, doAComma(A); DROP1) \
    X("NEXTWORD", NEXTWORD, T = getNextWord(C, ' ')) \
    X("FIND", FIND, push(doFind((char *)pop()))) \
    X("MALLOC", MALLOC, T = (CELL)malloc(T)) \
    X("FREE", MFREE, free((void *)T); DROP1) \
    X("FILL", FILL, memset((void *)N2, N0, N1); DROP3) \
    X("ZCOUNT", ZCOUNT, push(T); T = strlen(C)) \
    X("ZTYPE", ZTYPE, printString(C); DROP1 ) \
    X("DEBUG-MODE", DEBUG_MODE, push((CELL)&debugMode)) \

#ifndef __FILES__
#define FILE_OPCODES
#else
CELL doFileOpen(const char* fn, const char* mode);
void doFileClose(CELL fp);
void doFileRead();
void doFileWrite();
#define FILE_OPCODES \
    X("FOPEN", FOPEN, N = doFileOpen((char *)N+1, C+1); DROP1) \
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
    X("FOPEN", FOPEN, N = doFileOpen((char *)N+1, C+1); DROP1) \
    X("FCLOSE", FCLOSE, doFileClose(T); DROP1) \
    X("FREAD", FREAD, doFileRead()) \
    X("FWRITE", FWRITE, doFileWrite())
#endif

#define ARDUINO_OPCODES
#ifdef __ARDUINO__
#undef ARDUINO_OPCODES
#define ARDUINO_OPCODES \
    X("INPUT",  INPUT_PIN,        pinMode(T, PIN_INPUT);        DROP1) \
    X("PULLUP", INPUT_PIN_PULLUP, pinMode(T, PIN_INPUT_PULLUP); DROP1) \
    X("OUTPUT", OUTPUT_PIN,       pinMode(T, PIN_OUTPUT);       DROP1) \
    X("pin@",   DPIN_FETCH,       T = digitalRead((int)T); ) \
    X("pin!",   DPIN_STORE,       digitalWrite((int)T, (int)N); DROP2) \
    X("apin@",  APIN_FETCH,       T = analogRead((int)T); ) \
    X("apin!",  APIN_STORE,       analogWrite((int)T, (int)N);  DROP2)
#endif

// NB: These are for the HID library from NicoHood
#define GAMEPAD_OPCODES
#ifdef __GAMEPAD_FAKE__
#undef GAMEPAD_OPCODES
#define GAMEPAD_OPCODES \
    X("GP.X",          GP_XA, printStringF("GP.x(%ld)", T); DROP1) \
    X("GP.Y",          GP_YA, printStringF("GP.y(%ld)", T); DROP1) \
    X("GP.PRESS",      GP_BD, printStringF("GP.press(%ld)", T); DROP1) \
    X("GP.RELEASE",    GP_BU, printStringF("GP.release(%ld)", T); DROP1) \
    X("GP.PAD1",       GP_P1, printStringF("GP.p1(%ld)", T); DROP1) \
    X("GP.PAD2",       GP_P2, printStringF("GP.p2(%ld)", T); DROP1) \
    X("GP.RELEASEALL", GP_RA, printStringF("GP.releaseAll()")) \
    X("GP.WRITE",      GP_WR, printStringF("GP.write()"))
#endif

#ifdef __GAMEPAD__
#undef GAMEPAD_OPCODES
#define GAMEPAD_OPCODES \
    X("GP.X",          GP_XA, Gamepad.xAxis(T); DROP1) \
    X("GP.Y",          GP_YA, Gamepad.yAxis(T); DROP1) \
    X("GP.PRESS",      GP_BD, Gamepad.press(T); DROP1) \
    X("GP.RELEASE",    GP_BU, Gamepad.release(T); DROP1) \
    X("GP.PAD1",       GP_P1, Gamepad.dPad1(T); DROP1) \
    X("GP.PAD2",       GP_P2, Gamepad.dPad2(T); DROP1) \
    X("GP.RELEASEALL", GP_RA, Gamepad.releaseAll()) \
    X("GP.WRITE",      GP_WR, Gamepad.write())
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
    GAMEPAD_OPCODES \
    COMPORT_OPCODES \
    X("OPCODES", DUMP_OPCODES, dumpOpcodes()) \
    X("FORTH-SOURCE", BOOT_STRAP, push((CELL)&bootStrap[0])) \
    X("BYE", BYE, printString(" goodbye."); isBYE = 1)

#define X(name, op, code) OP_ ## op,
typedef enum {
    OP_NONE = 31,
    OPCODES
} OPCODE_T;
#undef X

#define X(name, op, code) if (opcode == OP_ ## op) { strcpy(buf, #op); }
void printOpcode(BYTE opcode) {
    char buf[32];
    sprintf(buf, "#%d", opcode);
    OPCODES;
    printf("\r\n-op:%s(PC:%lx,T:%lx,N:%lx)-", buf, (UCELL)PC, T, N);
}
#undef X

#define X(name, op, code) x = OP_ ## op; printStringF("\r\n%3d ($%02x, %c): %s", x, x, (char)x, name);
void dumpOpcodes() {
    int x;
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
        case 0: RSP = 0;  return;
            OPCODES
        default:
            printStringF("-unknown opcode: %d ($%02x) at %04lx-", IR, IR, PC - 1);
            return;
        }
        if (DSP < 1) { DSP = 0; }
        if ((max_cycles) && (--max_cycles < 1)) { return; }
    }
}
#undef X

void doDot(CELL num, int isUnsigned, int space, int width) {
    int len  = 0;
    if ((!isUnsigned) && (BASE == 10) && (num < 0)) {
        printString("-");
        num = -num;
    }
    UCELL n = (UCELL)num;
    char *cp = (char *)TMP_PAD;
    *(cp) = 0;
    do {
        char c = (char)(n % BASE);
        if (9 < c) { c += 7; }
        *(--cp) = c + '0';
        n /= BASE;
        ++len;
    } while (0 < n);
    while (len < width) { *(--cp) = '0'; len++; }
    if (space) { printString(" "); }
    printString(cp);
}

void doDotS() {
    printString("(");
    for (int i = 1; i <= DSP; i++) {
        if (1 < i) { printString(" "); }
        doDot(dstk[i], 0, 0, 0);
    }
    printString(")");
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
        LOOP_T* dp = &doStack[++loopSP];
        dp->loopStart = PC;
        dp->loopEnd = 0;
        dp->start = (N < T) ? N : T;
        dp->stop =  (N < T) ? T : N;
        dp->index = dp->start;
        DROP2;
    }
}

void doNext() {
    if (0 <= loopSP) {
        LOOP_T* dp = &doStack[loopSP];
        dp->loopEnd = PC;
        ++dp->index;
        if (dp->index <= dp->stop) { PC = dp->loopStart; }
        else { --loopSP; }
    }
}

void doBreak() {
    if (0 <= loopSP) {
        LOOP_T* dp = &doStack[loopSP];
        if (dp->loopEnd) {
            --loopSP;
            PC = dp->loopEnd;
        } else {
            printString("-cant-break-1st-");
        }
    }
}

void doBegin() {
    if (loopSP < LOOP_STK_SZ) {
        LOOP_T* dp = &doStack[++loopSP];
        dp->loopStart = PC;
        dp->loopEnd = 0;
    }
}

void doAgain() { 
    if (0 <= loopSP) { 
        LOOP_T* dp = &doStack[loopSP];
        dp->loopEnd = PC;
        PC = dp->loopStart; 
    } 
}

void doWhile(int dropIt, int isUntil) {
    if (loopSP < 0) { return; }
    LOOP_T* dp = &doStack[loopSP];
    dp->loopEnd = PC;
    CELL x = (dropIt) ? pop() : T;
    if (isUntil) x = (x) ? 0 : 1;
    if (x) { 
        PC = dp->loopStart; 
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
//    switch (which) {
//    case 1: Joystick.X(val); break;
//    case 2: Joystick.Y(val); break;
//    case 3: Joystick.Z(val); break;
//    }
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
    if (LAST < (HERE+0x40)) {
        printString("***OUT OF MEMORY***");
        return;
    }
    dp = (DICT_T*)LAST;
    if (DBG_DEBUG <= debugMode) { printStringF("-cw:%s(%ld)-", name, LAST); }
    dp->flags = 0;
    dp->lex = currentLex;
    dp->len = (BYTE)strlen(name);
    if (dp->len > NAME_LEN) {
        printStringF("-name [%s] too long (%d)-", name, NAME_LEN);
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
#ifdef __IS_PC__
    void printString(const char* str) { fputs(str, stdout); }
#else
    #ifdef __SERIAL__
    void printString(const char* str) { theSerial.print(str); }
    #else 
    void printString(const char* str) {}
    #endif
#endif

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
void doComma(CELL v)  { cellStore(HERE, v); HERE += CELL_SZ; }
void doAComma(ADDR v) { (ADDR_SZ == 2) ? doWComma((CELL)v) : doComma((CELL)v); }

int compiling(char* w, int errIfNot) {
    if ((STATE == 0) && (errIfNot)) { printStringF("[%s]: Compile only.", w); }
    return (STATE == 1) ? 1 : 0;
}

int interpreting(char* w, int errIfNot) {
    if ((STATE != 0) && (errIfNot)) { printStringF("[%s]: Interpreting only.", w); }
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

int doFind(char* name) {
    DICT_T* dp = isTempWord(name);
    if (dp) {
        push((CELL)dp);
        return 1;
    }
    dp = (DICT_T*)LAST;
    for (int i = 0; i < numWords; i++) {
        if ((strCmp(name, dp->name) == 0) && (dp->lex == currentLex)) {
            push((CELL)dp);
            return 1;
        }
        dp++;
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

int isNumber(const char* w) {
    if ((*(w) == '\'') && (*(w + 2) == '\'') && (*(w + 3) == 0)) {
        push(*(w + 1));
        return 1;
    }
    CELL num = 0, base = BASE, isNeg = 0, valid = 0;
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
        // 1 => IMMEDIATE, 2 => INLINE
        if (compiling(w, 0)) {
            if (dp->flags & 0x01) {
                run(xt, 0);
            } else {
                if (dp->flags & 0x02) {
                    ADDR xt2 = (dp-1)->XT-1;
                    for (ADDR a = xt; a < xt2; a++) {
                        doCComma(*a);
                    }
                } else {
                    doCComma(OP_CALL);
                    doAComma((ADDR)xt);
                    lastWasCall = 1;
                }
            }
        } else {
            run(xt, 0);
        }
        return;
    }

    if (isInlineWord(w)) { return; }

    if (isNumber(w)) {
        if (compiling(w, 0)) {
            CELL v = pop();
            if ((0x0000 <= v) && (v < 0x0100)) {
                doCComma(OP_BLIT);
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
            getNextWord(C, '"');
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
            char* x = C;
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

    if (strCmp(w, "LEXICON") == 0) {
        if (getNextWord(w, ' ')) {
            if (lexCount < LEX_SZ) { 
                w[12] = 0;
                currentLex = lexCount++;
                doCreate(w);
                doCComma(OP_BLIT);
                doCComma(currentLex);
                doCComma(OP_RET);
            } else {
                printString("-lexicon full-");
            }
        }
        return;
    }

    if (strCmp(w, "DEFINITIONS") == 0) {
        currentLex = pop();
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
#ifdef __DEV_BOARD__
    if (c == 127) {
        if (0 < numTIB) {
            numTIB--;
            *(--TIBEnd) = 0;
            printStringF("%c %c", 8, 8);
        }
        return;
    }
#endif
    if (c == 9) { c = 32; }
    if (c == 10) { c = 32; }
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
    if ((' ' <= c) && (c <= '~')) {
        *(TIBEnd++) = c;
        *(TIBEnd) = 0;
        ++numTIB;
#ifdef __DEV_BOARD__
        char x[2];
        x[0] = c;
        x[1] = 0;
        printString(x);
#endif
    }
}

void vmInit() {
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
    currentLex = 0;
    doComma(0);
}

#ifdef __DEV_BOARD__
void loop() {
#ifdef __SERIAL__
    while (theSerial.available()) {
        int c = theSerial.read();
        toTIB(c);
    }
#endif
    autoRun();
}
#endif
#ifdef __IS_PC__
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
    loadSourceF(": cell %d ; : cells cell * ; : addr %d ; ", CELL_SZ, ADDR_SZ);
    loadSourceF(": dict $%lx ; : dict-sz $%lx ; : entry-sz %d ;", (long)&dict[0], DICT_SZ, DICT_ENTRY_SZ);
    loadSourceF(": vars $%lx ; : vars-sz $%lx ;", (long)&vars[0], VARS_SZ);
    loadSourceF(": (num-words) $%lx ; : num-words (num-words) @ ;", (long)&numWords);
    loadSourceF(": (here) $%lx ; : here (here) @ ;", (long)&HERE);
    loadSourceF(": (last) $%lx ; : last (last) @ ;", (long)&LAST);
    loadSourceF(": (vhere) $%lx ; : vhere (vhere) @ ;", (long)&VHERE);
    loadSourceF(": base $%lx ; : state $%lx ;", (long)&BASE, (long)&STATE);
    loadSourceF(": tib $%lx ;", (long)&TIB[0]);
    loadSourceF(": (dsp) $%lx ; : dstack $%lx ;", (long)&DSP, (long)&dstk[0]);

    for (int i = 0; bootStrap[i]; i++) { parseLine(bootStrap[i]); }
}

#ifdef __FILES__
#define __USER_WORDS__
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
#ifdef __SERIAL__
    theSerial.begin(19200);
    while (!theSerial) {}
    while (theSerial.available()) {}
#endif
#ifdef __LITTLEFS__
    myFS.begin(1024 * 1024);
#endif
#ifdef __GAMEPAD__
    Gamepad.begin();
#endif
    vmInit();
    loadBaseSystem();
    #ifdef __USER_WORDS__
    loadUserWords();
    #endif
    numTIB = 0;
    TIBEnd = TIB;
    *TIBEnd = 0;
    ok();
}

#ifdef __IS_PC__
int main()
{
    setup();
    while (true) {
        loop();
        if (isBYE) { break; }
    }
}
#endif
#define SOURCE_BASE \
    X(1000, "lexicon forth : depth (dsp) @ 1- ; : 0sp 0 (dsp) ! ;") \
    X(1001, ": tuck swap over ; inline") \
    X(1002, ": ?dup if- dup then ;") \
    X(1003, ": mod /mod drop ; inline : / /mod nip ; inline") \
    X(1401, ": bl #32 ; inline : space bl emit ; inline") \
    X(1402, ": cr #13 emit #10 emit ; inline : tab #9 emit ; inline") \
    X(1005, ": rot >r swap r> swap ; : -rot swap >r swap r> ;") \
    X(1006, ": 2dup over over ; inline : 2drop drop drop ; inline") \
    X(1007, ": +! tuck @ + swap ! ; inline") \
    X(1008, ": negate 0 swap - ; inline") \
    X(1009, ": off 0 swap ! ; inline : on 1 swap ! ; inline") \
    X(1010, ": abs dup 0 < if negate then ;") \
    X(1011, ": hex $10 base ! ; : decimal #10 base ! ; : binary %10 base ! ;") \
    X(1012, ": low->high 2dup > if swap then ;") \
    X(1013, ": high->low 2dup < if swap then ;") \
    X(1014, ": min low->high drop ;") \
    X(1015, ": max high->low drop ;") \
    X(1016, ": between rot dup >r min max r> = ;") \
    X(1017, ": allot vhere + (vhere) ! ;") \
    X(1018, ": >body @ ; inline : auto-run dict ! ;") \
    X(1019, ": auto-last last >body auto-run ; : auto-off 0 auto-run ;") \
    X(1020, ": .word addr + 1+ count type tab ;") \
    X(1021, ": words last num-words 1 for dup .word entry-sz + next drop ;") \
    X(1022, ": .wordl cr dup . dup a@ . addr + dup c@ . 1+ dup c@ . space count type ;") \
    X(1023, ": wordsl last num-words 1 for dup .wordl entry-sz + next drop ;") \
    X(1024, "variable (regs) #10 CELLS allot") \
    X(1025, ": reg CELLS (regs) + ;") \
    X(1026, ": >src 0 reg ! ; : >dst 1 reg ! ;") \
    X(1027, ": src 0 reg @ ; : src+ src dup 1+ >src ;") \
    X(1028, ": dst 1 reg @ ; : dst+ dst dup 1+ >dst ;") \
    X(1029, ": .b 3 base @ #16 = if 1- then .n ;") \
    X(1030, ": dump low->high for i c@ space .b next ;") \
    X(1031, ": _t0 cr dup 8 .n ':' emit #16 over + dump ;") \
    X(1032, ": _t1 dup _t0 #16 + ;") \
    X(1033, ": dump-dict dict begin _t1 dup here < while drop ;") \
    X(1034, ": elapsed timer swap - 1000 /mod . '.' emit 3 .n .\"  seconds\" ;") \
    X(1035, "variable (ch) CELL allot variable (cl) CELL allot") \
    X(1036, "variable (nw) CELL allot variable (vh) CELL allot") \
    X(1037, ": marker here (ch) ! last (cl) ! num-words (nw) ! vhere (vh) ! ;") \
    X(1038, ": forget (ch) @ (here) ! (cl) @ (last) ! (nw) @ (num-words) ! (vh) @ (vhere) ! ;") \
    X(1039, ": forget-1 last a@ (here) ! last entry-sz + (last) ! num-words 1- (num-words) ! ;") \
    X(1040, ": pad last #128 - ; : ' pad nextword if- drop pad find then ;") \
    X(1999, "marker")

#define SOURCE_PC X(2000, ": is-pc 0 ;")
#ifdef __IS_PC__
#undef SOURCE_PC
#define SOURCE_PC \
    X(2000, ": is-pc 1 ;") \
    X(2001, ": _t0 27 emit '[' emit ;") \
    X(2002, ": gotoXY ( x y -- )  _t0 (.) ';' emit (.) 'H' emit ;") \
    X(2003, ": cls _t0 .\" 2J\" 0 dup gotoXY ;") \
    X(2004, ": .ch dup #32 < if drop '.' then dup $7e > if drop '.' then emit ;") \
    X(2005, ": dump-ch low->high for i c@ .ch next ;") \
    X(2006, "variable (comport)   CELL allot 0 (comport) !") \
    X(2007, "variable (comhandle) CELL allot 0 (comhandle) !") \
    X(2008, ": comhandle (comhandle) @ ;") \
    X(2009, ": comport (comport) @ ;") \
    X(2010, ": comopen ( n -- ) dup (comport) ! com-open (comhandle) ! ;") \
    X(2011, ": comclose ( -- ) comhandle com-close 0 (comhandle) ! ;") \
    X(2012, ": comread ( -- c ) comhandle com-read ;") \
    X(2013, ": comwrite ( c -- ) comhandle com-write 0= if .\" -err-\" then ;") \
    X(2014, ": comall ( -- ) begin comread dup if emit 1 then while ;") \
    X(2015, ": comstring ( a n -- ) over + for i c@ comwrite next ;") \
    X(2016, ": comcr ( -- ) 13 comwrite ;") \
    X(2017, ": comline ( cs -- )   count comstring comcr comall ;") \
    X(2018, ": comlinez ( zs -- ) zcount comstring comcr comall ;") \
    X(2099, "marker") 
#endif

#define SOURCE_ARDUINO \
    X(4001, "variable (mux) CELL allot") \
    X(4002, ": mux (mux) @ ; : mux! (mux) ! ;") \
    X(4003, ": (s0) mux ;     : s0 (s0) c@ ; ") \
    X(4004, ": (s1) mux 1+ ;  : s1 (s1) c@ ;") \
    X(4005, ": (s2) mux 2 + ; : s2 (s2) c@ ; ") \
    X(4006, ": (s3) mux 3 + ; : s3 (s3) c@ ;") \
    X(4007, ": (z)  mux 4 + ; : z  (z)  c@ ;") \
    X(4008, ": _t1 ( channel s# bit -- ) rot and 0= 0= swap if- pin! else 2drop then ;") \
    X(4009, ": mux-select ( mux channel -- ) mux!") \
    X(4010, "    dup s0 %0001 _t1 ") \
    X(4011, "    dup s1 %0010 _t1 ") \
    X(4012, "    dup s2 %0100 _t1 ") \
    X(4013, "        s3 %1000 _t1 ;") \
    X(4014, ": mux-init ( s0 s1 s2 s3 z mux -- ) mux! (z) c! (s3) c! (s2) c! (s1) c! (s0) c! ") \
    X(4015, "    z pullup s0 output s1 output s2 output s3 if s3 output then ;") \
    X(4016, ": _t0 ( mux -- z-pin ) mux! z ;") \
    X(4017, ": mux-output ( mux -- ) _t0 output ;") \
    X(4018, ": mux@  ( mux -- n ) _t0 pin@ ;") \
    X(4019, ": mux@A ( mux -- n ) _t0 apin@ ;") \
    X(4020, ": mux!  ( n mux -- ) _t0 pin! ;") \
    X(4021, ": mux!A ( n mux -- ) _t0 apin! ;") \
    X(4999, "marker")

#define SOURCE_USER \
    X(5005, "( Note: the pinMode on the Arduino board is set to INPUT_PULLUP )") \
    X(5010, "( Wire the switch to go to ground when closed )") \
    X(5015, "( This way, the pin is HIGH/1 when the switch is open and LOW/0 when closed )") \
    X(5020, "( DCS maps gamepad/joystick button presses to DCS commands )") \
    X(5025, "( e.g. - consider a rocker switch to raise and lower the landing gear )") \
    X(5030, "( The gamer would map GP-button <x> to 'RAISE_LANDING_GEAR' )") \
    X(5035, "( And he would map GP-button <y> to 'LOWER_LANDING_GEAR' )") \
    X(5040, "( When the switch goes LOW/0, we want to report that GP-button <x> was pressed )") \
    X(5045, "( When the switch goes HIGH/1, we want to report that GP-button <y> was pressed )") \
    X(5050, "( So when a switch opens and closes, that can generate potentially 2 DCS commands )") \
    X(5055, "( So we need to be able to associate a 2 GP-Buttons with one mux/channel )") \
    X(5060, "( and trigger the appropriate button press depending when we detect that there was a change )") \
    X(5065, "( For a given mux/channel, we need to know 3 pieces of info: )") \
    X(5070, "( 1 - The last known channel value, 2 bytes to support digital and analog data )") \
    X(5075, "( 2 - Which GP-button to 'press' when the channel goes HIGH -> LOW, 1 byte )") \
    X(5080, "( 3 - Which GP-button to 'press' when the channel goes LOW -> HIGH, 1 byte )") \
    X(5085, "( Note: a GP-button value of ZERO means no GP-button is mapped for that event )") \
    X(5086, "variable mux1 5 allot") \
    X(5087, "variable mux2 5 allot") \
    X(5090, "#16 4 * constant data-sz") \
    X(5095, "variable mux1-data data-sz allot") \
    X(5100, "variable mux2-data data-sz allot") \
    X(5105, "variable (data) CELL allot") \
    X(5110, "mux1-data data-sz 0 fill") \
    X(5115, "mux2-data data-sz 0 fill") \
    X(5120, "( The value read from the mux )") \
    X(5125, "variable (val) 2 allot") \
    X(5130, ": >val ( n -- ) (val) w! ;") \
    X(5135, ": val ( -- n ) (val) w@ ;") \
    X(5140, "( The data context for the current pin/channel )") \
    X(5145, "variable (ctx) CELL allot") \
    X(5150, ": >ctx ( ch -- ) #15 and 4 * (data) @ + (ctx) ! ;") \
    X(5155, ": ctx ( -- a ) (ctx) @ ;") \
    X(5160, "( Set/Retrieve values from the data context )") \
    X(5165, ": ch-config ( ch l h -- ) ROT >ctx ctx 3 + c! ctx 2 + c! 0 ctx w! ;") \
    X(5170, ": lastval   ( -- n )      ctx w@ ;") \
    X(5175, ": >lastval  ( n -- )      ctx w! ;") \
    X(5180, ": btn-lo    ( -- n )      ctx 2 + c@ ;") \
    X(5185, ": btn-hi    ( -- n )      ctx 3 + c@ ;") \
    X(5190, ": press-button ( n -- ) dup gp.press 50 ms gp.release ;") \
    X(5195, ": do-report    ( n -- ) if- press-button else drop then ;") \
    X(5200, ": report-LOW   ( -- )   btn-lo do-report ;") \
    X(5205, ": report-HIGH  ( -- )   btn-hi do-report ;") \
    X(5210, ": ->LOW?  ( -- ) lastval 0 > val 0=  and 0= 0= ;") \
    X(5215, ": ->HIGH? ( -- ) lastval 0=  val 0 > and 0= 0= ;") \
    X(5220, ": process ( val ch -- )") \
    X(5225, "	>ctx >val") \
    X(5230, "	->LOW? if report-LOW then") \
    X(5235, "	->HIGH? if report-HIGH then") \
    X(5240, "	val >lastval ;") \
    X(5245, ": process-mux ( mux data -- )") \
    X(5250, "	(data) ! (mux) !") \
    X(5255, "	0 #15 for mux i over mux-select mux@ i process next ;") \
    X(5260, ": process-muxes ( -- )") \
    X(5265, "	mux1 mux1-data process-mux") \
    X(5270, "	mux2 mux2-data process-mux ;") \
    X(5275, "mux1-data (data) ! ") \
    X(5280, "0 1 2 ch-config") \
    X(5285, "1 3 4 ch-config") \
    X(5290, "2 5 0 ch-config") \
    X(5295, "3 0 6 ch-config") \
    X(5300, "4 7 8 ch-config") \
    X(5305, "mux2-data (data) ! ") \
    X(5310, "0 16 17 ch-config") \
    X(5315, "3 4 5 6  9 mux1 mux-init cr") \
    X(5320, "3 4 5 6 10 mux2 mux-init cr") \
    X(5991, ".\" BoardForth v0.0.1 - by Chris Curl\" cr") \
    X(5992, ".\" Source: https://github.com/CCurl/BoardForth\" cr") \
    X(5993, ".\" Dictionary size:\" dict-sz . .\" , free:\" last here - . cr") \
    X(5998, ".\" Hello.\"") \
    X(5999, "marker")

#define SOURCES SOURCE_BASE SOURCE_PC SOURCE_ARDUINO SOURCE_USER

#undef X
#define X(num, val) const PROGMEM char str ## num[] = val;
SOURCES

#undef X
#define X(num, val) str ## num,
const char *bootStrap[] = {
    SOURCES
    NULL
};
