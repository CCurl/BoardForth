#include "board.h"
#ifndef __DEV_BOARD__
#include <windows.h>
int analogRead(int p) { return 0; }
int analogWrite(int p, int v) { return 0; }
int digitalRead(int p) { return 0; }
int digitalWrite(int p, int v) { return 0; }
void pinMode(int p, int m) {}
#endif

#include "defs.h"
#pragma warning(disable: 4996)

CELL *dstk;
CELL *rstk;

BYTE dict[DICT_SZ+1];
DICT_T words[WORDS_SZ];
BYTE lastDictionaryId = 0;
SYSVARS_T* sys;
LOOPINFO_T loopInfo[12];
int loopDepth = 0;
CELL reg[26];
int curReg;
CELL PC;

typedef struct {
    char name[16];
    char codes[8];
} s4_word_t;

s4_word_t s4Words[] = {
    {"dup", "#"},
    {"drop", "\\"},
    {"mod", "%"},
    {"-",  "-"},
    {"1-", "1-"},
    {"+",  "+"},
    {"1+", "1+"},
    {"*",  "*"},
    {"/",  "/"},
    {"<",  "<"},
    {"=",  "="},
    {">",  ">"},
    {"and",  "&"},
    {"or",  "|"},
    {"swap", "S"},
    {"over", "O"},
    {"c@", "c@"},
    {"w@", "w@"},
    {"@",  "@"},
    {"c!", "c!"},
    {"w!", "w!"},
    {"!",  "!"},
    {"emit", ","},
    {"fopen", "FO"},
    {"fclose", "FC"},
    {"leave", ";"},
    {"", ""}
};

int iHERE = 0;
void s4CompileString(const char* str) {
    while (*str) {
        CCOMMA((*str++));
    }
}

void s4RunString(const char* str) {

}

s4_word_t *s4Find(char* w) {
    for (int i = 0; ; i++) {
        s4_word_t* p = &s4Words[i];
        if (p->name[0] == 0) { return 0; }
        if (strcmp(p->name, w) == 0) {
            return p;
        }
    }
    return 0;
}

void s4PutAddress(CELL tgt, CELL val) {

}

int s4Parse(char* w) {
    s4_word_t* p = s4Find(w);
    if (p) { s4CompileString(p->codes); }
    return (p) ? 1 : 0;
}

int doQuote(int pc, int isPush) {
    char buf[2]; buf[1] = 0;
    while ((dict[pc] != '"') && (pc < DICT_SZ)) {
        buf[1] = dict[pc++];
        printString(buf);
    }
    return ++pc;
}

void run(CELL pc, CELL max_cycles) {
    CELL t1, t2;
    BYTE IR; //  , * a1;
    while (1) {
        if (max_cycles) {
            if (--max_cycles < 1) { return; }
        }
        if ((pc < 0) || (DICT_SZ <= pc)) { return; }
        IR = dict[PC++];
        // printStringF("\r\n-PC-%d/%lx:IR-%d/%x-", PC-1, PC-1, (int)IR, (unsigned int)IR); fDOTS();
        switch (IR) {
        case 0: pc = -1; break;                                  // 0
        case ' ': break;                                    // 32
        // case '!': reg[curReg] = pop();  break;              // 33
        case '"': pc = doQuote(pc, 0);  break;              // 34
        case '#': push(T);              break;              // 35
        // case '$': pc = doHexNumber(pc); break;              // 36
        case '%': t1 = pop(); T %= t1;  break;              // 37
        case '&': t1 = pop(); T &= t1;  break;              // 38
        // case '\'': push(dict[pc++]);    break;              // 39
        //case '(': pc = doIf(pc);        break;              // 40
        //case ')': /*maybe ELSE?*/       break;              // 41
        case '*': t1 = pop(); T *= t1;  break;              // 42
        case '+': t1 = dict[pc];                            // 43
            if (t1 == '+') { ++pc;  ++T; }
            else { t1 = pop(); T += t1; }
            break;
        case ',': printStringF("%c", (char)pop());  break;  // 44
        case '-': t1 = dict[pc];                            // 45
            if (t1 == '-') { ++pc; --T; }
            else { t1 = pop(); T -= t1; }
            break;
        case '.': printStringF("%ld", pop());      break;   // 46
        case '/': t1 = pop(); if (t1) { T /= t1; } break;   // 47-57
        //case '0': case '1': case '2': case '3': case '4':
        //case '5': case '6': case '7': case '8': case '9':
        //    pc = doNumber(pc - 1); break;
        case ':': push(pc + ADDR_SZ); pc = addrAt(pc); break;           // 58
        case ';': pc = rpop(); break;                       // 59
        case '<': t1 = pop(); T = T < t1 ? -1 : 0;  break;  // 60
        case '=': t1 = pop(); T = T == t1 ? -1 : 0; break;  // 61
        case '>': t1 = pop(); T = T > t1 ? -1 : 0;  break;  // 62
        // case '?': push(_getch());                   break;  // 63
        case '@': T = dict[T];                break;  // 64
        case 'A': t1 = dict[pc++];
            if (t1 == 'R') { T = analogRead(T); }
            if (t1 == 'W') { t2 = pop(); t1 = pop(); analogWrite(t2, t1); }
            break;
        case 'B': printString(" "); break;
        case 'C': t1 = dict[pc++];
            if (t1 == '@') { if ((0 <= T) && (T < DICT_SZ)) { T = dict[T]; } }
            if (t1 == '!') { t1 = pop(); t2 = pop(); if ((0 <= t1) && (t1 < DICT_SZ)) { dict[t1] = (byte)t2; } }
            break;
        case 'D': t1 = dict[pc++];
            if (t1 == 'R') { T = digitalRead(T); }
            if (t1 == 'W') { t2 = pop(); t1 = pop(); digitalWrite(t2, t1); }
            break;
        case 'E': break;   /* *** FREE ***  */
        case 'F': t1 = dict[pc++];
            //if (t1 == 'O') { pc = doFileOpen(pc, "rb"); }
            //if (t1 == 'N') { pc = doFileOpen(pc, "wb"); }
            //if (t1 == 'C') { pc = doFileClose(pc); }
            //if (t1 == 'R') { pc = doFileRead(pc); }
            //if (t1 == 'W') { pc = doFileWrite(pc); }
            //if (t1 == 'F') { push(0); }
            //if (t1 == 'T') { push(-1); }
            break;
        case 'G': break;   /* *** FREE ***  */
        case 'H': t1 = dict[pc++];
            if (t1 == '@') { push(sys->HERE); }
            if (t1 == '!') { t2 = pop(); if ((0 < t2) && (t2 < DICT_SZ)) { sys->HERE = t2; } }
            break;
        case 'I': t1 = dict[pc++];
            //if (t1 == 'A') { dumpAll(); }
            //if (t1 == 'C') { dumpCode(); }
            //if (t1 == 'D') { dumpDict(); }
            //if (t1 == 'M') { dumpMemory(); }
            //if (t1 == 'R') { dumpRegs(); }
            //if (t1 == 'S') { dumpStack(1); }
            break;
        case 'J':  break;   /* *** FREE ***  */
        case 'K': T *= 1000; break;
        case 'L': break;   /* *** FREE ***  */
        case 'M': t1 = dict[pc++];
            if (t1 == '@') { T = *((byte*)T); }
            if (t1 == '!') { *((byte *)T) = (N & 0xFF); }
            break;
        case 'N': break;
        case 'O': push(N); break;
        case 'P': t1 = dict[pc++]; t2 = pop();
            if (t1 == 'I') { pinMode(t2, A_INPUT); }
            if (t1 == 'U') { pinMode(t2, INPUT_PULLUP); }
            if (t1 == 'D') { pinMode(t2, INPUT_PULLDOWN); }
            if (t1 == 'O') { pinMode(t2, OUTPUT); }
            break;
        case 'Q': break;   /* *** FREE ***  */
        case 'R': { char x[] = { 13,10,0 };  printString(x); }    break;
        case 'S': t1 = pop(); t2 = pop(); push(t1); push(t2);   break;
        // case 'T': push(millis()); break;
        case 'U': break;   /* *** FREE ***  */
        case 'V': break;   /* *** FREE ***  */
        // case 'W': delay(pop()); break;
        case 'X': t1 = dict[pc++]; if (t1 == 'X') { vmInit(); } break;
        case 'Y': break;
        // case 'Z': isBye = (dict[pc++] == 'Z'); break;
        case '[': rpush(pc); break;                   // 91
        case '\\': pop(); break;                            // 92
        case ']': if (T) { pc = R; }                        // 93
                else { pop(); rpop(); }
                break;
        case '^': t1 = pop(); T ^= t1;  break;              // 94
        case '_': T = -T;      break;                       // 95
        case '`': break;  /* *** FREE ***  */               // 96
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': // 97-122
        case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
        case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
        case 's': case 't': case 'u': case 'v': case 'w': case 'x':
        case 'y': case 'z': curReg = IR - 'a'; t1 = dict[pc];
            if (t1 == '+') { ++pc; ++reg[curReg]; }
            if (t1 == '-') { ++pc; --reg[curReg]; }
            break;
        // case '{': pc = doDefineFunction(pc); break;        // 123
        case '|': t1 = pop(); T |= t1; break;               // 124
        // case '}': pc = rpop(); break;                       // 125
        case '~': T = ~T; break;                            // 126
        }
    }
}

void runOpcode(BYTE opcode) {
    CELL xt = sys->HERE + 17;
    dict[xt] = opcode;
    dict[xt+1] = ';';
    run(xt, 0);
}

void autoRun() {
    CELL addr = addrAt(0);
    if (addr) {
        run(addr, 0);
    }
}

void push(CELL v) {
    sys->DSP = (sys->DSP < STK_SZ) ? sys->DSP + 1 : STK_SZ;
    T = v;
}
CELL pop() {
    sys->DSP = (sys->DSP > 0) ? sys->DSP - 1 : 0;
    return dstk[sys->DSP + 1];
}

void rpush(CELL v) {
    sys->RSP = (sys->RSP < STK_SZ) ? sys->RSP + 1 : STK_SZ;
    R = v;
}
CELL rpop() {
    sys->RSP = (sys->RSP > 0) ? sys->RSP - 1 : 0;
    return rstk[sys->RSP + 1];
}

void vmInit() {
    sys = (SYSVARS_T*)dict;
    sys->HERE = ADDR_HERE_BASE;
    sys->LAST = 0;
    sys->BASE = 10;
    sys->STATE = 0;
    sys->DSP = 0;
    sys->RSP = 0;
    sys->currentDictId = 0;
    sys->TIB = DICT_SZ - TIB_SZ - CELL_SZ;
    sys->DSTACK = sys->TIB -    (CELL_SZ * STK_SZ) - CELL_SZ;
    sys->RSTACK = sys->DSTACK - (CELL_SZ * STK_SZ) - CELL_SZ;
    dstk = (CELL*)&dict[sys->DSTACK];
    rstk = (CELL*)&dict[sys->RSTACK];
    loopDepth = 0;
    genOpcodeWords();
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
    char buf[500];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    printString(buf);
}

CELL cellAt(CELL loc) {
#ifdef __NEEDS_ALIGN__
    return (dict[loc + 3] << 24) | (dict[loc + 2] << 16) | (dict[loc + 1] << 8) | dict[loc];
#else
    return *((CELL *)&dict[loc]);
#endif
}

CELL wordAt(CELL loc) {
#ifdef __NEEDS_ALIGN__
    return (dict[loc + 1] << 8) | dict[loc];
#else
    return *((WORD *)&dict[loc]);
#endif
}

CELL addrAt(CELL loc) {         // opcode #16
    return (ADDR_SZ == 2) ? wordAt(loc) : cellAt(loc);
}

void wordStore(CELL addr, CELL val) {
    dict[addr] = (val & 0xFF);
    dict[addr + 1] = (val >> 8) & 0xFF;
}

void cellStore(CELL addr, CELL val) {
    dict[addr++] = ((val) & 0xFF);
    dict[addr++] = ((val >> 8) & 0xFF);
    dict[addr++] = ((val >> 16) & 0xFF);
    dict[addr++] = ((val >> 24) & 0xFF);
}

void addrStore(CELL addr, CELL val) {
    (ADDR_SZ == 2) ? wordStore(addr, val) : cellStore(addr, val);
}

void fDUMPCODE() {
    char x[32];
    int n = 0;
    FILE* to = (FILE*)pop();
    to = to ? to : stdout;

    fprintf(to, "; WORDS: LAST=%d", sys->LAST);
    for (int i = sys->LAST - 1; 0 <= i; i--) {
        DICT_T* dp = &words[i];
        int fl = dp->flagsLen;
        fprintf(to, "\r\n%4d %04lx %3d %d %d, %s", i, 
            (CELL)dp->XT, (int)dp->dictionaryId, (fl >> 6), (fl & 0x1F), dp->name);
    }

    fprintf(to, "\r\n\r\n; CODE: HERE=%04lx (%ld)", sys->HERE, sys->HERE);
    for (int i = 0; i < sys->HERE; i++) {
        if (i % 16 == 0) {
            if (n) { x[n] = 0; fprintf(to, " ; %s", x); }
            fprintf(to, "\r\n%04x:", i);
            n = 0;
        }
        BYTE b = dict[i];
        x[n++] = ((31 < b) && (b < 128)) ? b : '.';
        fprintf(to, " %02x", dict[i]);
    }
    if (n) { x[n] = 0; fprintf(to, " ; %s", x); }

    //fprintf(to, "\r\n\r\n; OPCODES:");
    //for (int i = 0; opcodes[i].opcode != 255; i++) {
    //    OPCODE_T* op = &opcodes[i];
    //    int c = op->opcode;
    //    int c1 = ((31 < c) && (c < 127)) ? c : '.';
    //    fprintf(to, "\r\n%02x (%3d, %c) %d %s", c, c, c1, (int)op->makeWord, op->name);
    //}
}

int compiling(char* w, int errIfNot) {
    if ((sys->STATE == 0) && (errIfNot)) {
        printStringF("[%s]: Compile only.", w);
    }
    return (sys->STATE == 0) ? 0 : 1;
}

int interpreting(char* w, int errIfNot) {
    if ((sys->STATE != 0) && (errIfNot)) {
        printStringF("[%s]: Interpreting only.", w);
    }
    return (sys->STATE == 0) ? 1 : 0;
}

void fWSTORE() {       // opcode #9
    CELL addr = pop();
    CELL val = pop();
    wordStore(addr, val);
}
void fASTORE() {       // opcode #10
    (ADDR_SZ == 2) ? fWSTORE() : fSTORE();
}
void fSTORE() {        // opcode #11
    CELL addr = pop();
    CELL val = pop();
    if ((0 <= addr) && ((addr + 4) < DICT_SZ)) {
        cellStore(addr, val);
        return;
    }
    printStringF("Invalid address: %ld ($%04lX)", addr);
}
void fCCOMMA() {       // opcode #12
    dict[sys->HERE++] = (BYTE)pop();
}
void fWCOMMA() {       // opcode #13
    WORD x = (WORD)pop();
    wordStore(sys->HERE, x);
    sys->HERE += WORD_SZ;
}
void fCOMMA() {        // opcode #14
    CELL x = pop();
    cellStore(sys->HERE, x);
    sys->HERE += CELL_SZ;
}
void fACOMMA() {       // opcode #15
    (ADDR_SZ == 2) ? fWCOMMA() : fCOMMA();
}
void fSWAP() {         // opcode #24
    CELL t = T; T = N; N = t;
}
void fEMIT() {
    char buf[2];
    buf[0] = (BYTE)pop();
    buf[1] = 0;
    printString(buf);
}
void fTYPE() {
    CELL n = pop();
    CELL a = pop();
    // printStringF("-t:%d:%d-", n, a);
    char x[2];
    x[1] = 0;
    for (int i = 0; i < n; i++) {
        x[0] = dict[a++];
        printString(x);
    }
}
void fDOTS() {
    if (sys->DSP) {
        printString("(");
        for (int i = 1; i <= sys->DSP; i++) {
            push(' '); fEMIT();
            push(dstk[i]);
            push(0);
            fNUM2STR();
            fTYPE();
        }
        printString(" )");
    }
    else {
        printString("()");
    }
}
BYTE getOpcode(char *word) { return 0xFF; }
void fPARSEWORD() {    // opcode #59
    char s4[16];
    CELL wa = pop(), t1;
    char* w = (char*)&dict[wa];
    if (s4Parse(w)) { return; }

    // printStringF("-pw[%s]-", w);
    push(wa); fFIND();
    if (pop()) {
        int dpi = pop();
        DICT_T* dp = &words[dpi];

        // runOpcode(OP_GETXT);
        CELL xt = dp->XT;
        sprintf(s4, ":%04x", xt);
        if (compiling(w, 0)) {
            s4CompileString(s4);
        }
        else { s4RunString(s4); }
        return;
    }

    push(wa); fISNUMBER();
    if (pop()) {
        t1 = pop();
        sprintf(s4, "$%lx", t1);
        if (compiling(w, 0)) {
            s4CompileString(s4);
        }
        else { s4RunString(s4); }
        return;
    }

    if (strcmp(w, ";") == 0) {
        if (!compiling(w, 1)) { return; }
        s4CompileString(";");
        sys->STATE = 0;
        return;
    }

    if (strcmp(w, "if") == 0) {
        if (!compiling(w, 1)) { return; }
        s4CompileString("(");
        push(sys->HERE);
        s4CompileString("0000");
        return;
    }

    if (strcmp(w, "else") == 0) {
        if (!compiling(w, 1)) { return; }
        t1 = pop();
        s4CompileString("JJ");
        push(sys->HERE);
        s4CompileString("0000)E(");
        s4PutAddress(t1, sys->HERE);
        return;
    }

    if (strcmp(w, "then") == 0) {
        if (!compiling(w, 1)) { return; }
        s4CompileString(")");
        t1 = pop();
        s4PutAddress(t1, sys->HERE);
        return;
    }

    if (strcmp(w, "begin") == 0) {
        if (!compiling(w, 1)) { return; }
        s4CompileString("[");
        return;
    }

    if (strcmp(w, "while") == 0) {
        if (!compiling(w, 1)) { return; }
        s4CompileString("]");
        return;
    }

    if (strcmp(w, ":") == 0) {
        if (!interpreting(w, 1)) { return; }
        push(wa);
        fNEXTWORD();
        if (pop()) {
            push(wa);
            fCREATE();
            sys->STATE = 1;
        }
        return;
    }

    if (strcmp(w, "variable") == 0) {
        if (!interpreting(w, 1)) { return; }
        push(wa);
        fNEXTWORD();
        if (pop()) {
        }
        return;
    }

    if (strcmp(w, "constant") == 0) {
        if (!interpreting(w, 1)) { return; }
        push(wa);
        fNEXTWORD();
        if (pop()) {
        }
        return;
    }

    if (strcmp(w, ".\"") == 0) {
        if (! compiling(w, 1)) { return; }
        BYTE c = nextChar();
        int len = 0;
        CCOMMA('"');
        while (c && (c != '"')) {
            CCOMMA(c);
            c = nextChar();
            ++len;
        }
        return;
    }

    sys->STATE = 0;
    printStringF("[%s]??", w);
}
void fPARSELINE() {    // opcode #60
    sys->TOIN = pop();
    CELL buf = sys->HERE+24;
    char* w = (char*)&dict[buf];
    push(buf);
    fNEXTWORD();
    while (pop()) {
        // printStringF("-pw:%s-", w);
        if (strcmp(w, "//") == 0) { break; }
        if (strcmp(w, "\\") == 0) { break; }
        push(buf);
        fPARSEWORD();
        buf = sys->HERE+24;
        w = (char*)&dict[buf];
        push(buf);
        fNEXTWORD();
    }
}
void fCREATE() {       // opcode #64
    CELL wa = pop();
    char* name = (char*)&dict[wa];
    // printStringF("-define [%s] at %d (%lx)", name, sys->HERE, sys->HERE);

    if (WORDS_SZ <= sys->LAST) {
        printStringF("-dict space overflow-");
        return;
    }
    DICT_T* dp = &words[sys->LAST++];
    dp->XT = (ADDR)sys->HERE;
    dp->dictionaryId = sys->currentDictId;
    dp->flagsLen = (BYTE)strlen(name);
    strcpy((char*)dp->name, name);
    // printStringF(",XT:%d (%lx)-", sys->HERE, sys->HERE);
}
//// (a1 -- [a2 1] | 0)
void fFIND() {         // opcode #65
    char* name = (char*)&dict[pop()];
    // printStringF("-lf:[%s]-", name);
    CELL cl = sys->LAST-1;
    while (0 <= cl) {
        DICT_T* dp = &words[cl];
        if (strcmp(name, (char*)dp->name) == 0) {
            // printStringF("-FOUND! (%lx)-", cl);
            push(cl);
            push(1);
            return;
        }
        cl--;
    }
    push(0);
}
void fNEXTWORD() {     // opcode #66
    CELL to = pop();
    char c = nextChar();
    int len = 0;
    while (c && (c < 33)) { c = nextChar(); }
    while (c && (32 < c)) {
        dict[to++] = c;
        c = nextChar();
        len++;
    }
    dict[to] = 0;
    push(len);
}
void fISNUMBER() {     // opcode #67
    CELL wa = pop();
    char* w = (char*)&dict[wa];

    if ((*w == '\'') && (*(w + 2) == '\'') && (*(w + 3) == 0)) {
        push(*(w + 1));
        push(1);
        return;
    }

    if (*w == '#') { is_decimal(w + 1); return; }
    if (*w == '$') { is_hex(w + 1);     return; }
    if (*w == '%') { is_binary(w + 1);  return; }

    if (sys->BASE == 10) { is_decimal(w); return; }
    if (sys->BASE == 16) { is_hex(w);     return; }
    if (sys->BASE == 2) { is_binary(w);  return; }
    push(0);
}
// OP_NUM2STR (#85)    : num>str ( n l -- a ) ... ;
void fNUM2STR() {
    BYTE reqLen = (BYTE)pop();
    CELL val = pop();
    int isNeg = ((val < 0) && (sys->BASE == 10));
    ulong num = (isNeg) ? -val : val;
    BYTE len = 0;
    CELL cp = sys->HERE + 50;
    dict[cp--] = (BYTE)0;
    reqLen = (reqLen < 49) ? reqLen : 48;

    do {
        CELL r = (num % sys->BASE) + '0';
        if ('9' < r) { r += 7; }
        dict[cp--] = (BYTE)r;
        len++;
        num /= sys->BASE;
    } while (num > 0);
    if (isNeg) {
      dict[cp--] = '-';
      ++len;
    }

    while (len < reqLen) {
        dict[cp--] = '0';
        ++len;
    }
    dict[cp] = len;
    push(cp + 1);
    push(len);
}

void genOpcodeWords() { }
//    for (int i = 0; ; i++) {
//        OPCODE_T* op = &opcodes[i];
//        if (op->opcode == 255) { return; }
//        if (op->makeWord) {
//            CELL to = sys->HERE + 100;
//            stringToDict(op->name, to);
//            push(to);
//            fCREATE();
//            CCOMMA(op->opcode);
//            CCOMMA(OP_RET);
//            DICT_T* dp = &words[sys->LAST - 1];
//            dp->flagsLen |= 0x40;
//        }
//    }
//}

void CCOMMA(BYTE v) { push(v); fCCOMMA(); }
void WCOMMA(WORD v) { push(v); fWCOMMA(); }
void COMMA(CELL v) { push(v); fCOMMA(); }
void ACOMMA(ADDR v) { push(v); fACOMMA(); }

BYTE nextChar() {
    if (dict[sys->TOIN]) return dict[sys->TOIN++];
    return 0;
}

void is_hex(char* word) {
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
            num += ((c + 10) - 'A');
            continue;
        }
        if ((c >= 'a') && (c <= 'f')) {
            num *= 0x10;
            num += ((c + 10) - 'a');
            continue;
        }
        push(0);
        return;
    }
    push((CELL)num);
    push(1);
}

void is_decimal(char* word) {
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
        }
        else {
            push(0);
            return;
        }
    }

    num = is_neg ? -num : num;
    push((CELL)num);
    push(1);
}

void is_binary(char* word) {
    CELL num = 0;
    if (*word == (char)0) { push(0); return; }
    while (*word) {
        char c = *(word++);
        if ((c >= '0') && (c <= '1')) {
            num *= 2;
            num += (c - '0');
        }
        else {
            push(0);
            return;
        }
    }

    push((CELL)num);
    push(1);
}

CELL stringToDict(char* s, CELL to) {
    // printStringF("-sd.%d", (int)to);
    if (to == 0) { to = sys->HERE + 64; }
    // printStringF(":%d-\r\n", (int)to);
    CELL x = to;
    while (*s) {
        dict[x++] = *(s++);
    }
    dict[x] = 0;
    return to;
}

void parseLine(char* line) {
    stringToDict(line, sys->TIB);
    push(sys->TIB);
    fPARSELINE();
}

void loadBaseSystem() {
    char buf[32];
    sprintf(buf, ": dict #%lu ;", (ulong)&words[0]);
    parseLine(buf);
    loadSource(PSTR(": cell 4 ; : addr 2 ;"));
    loadSource(PSTR("// : tib    #8 @ ;      : >in   #12 ;"));
    loadSource(PSTR(": (h) #16 ;          : here (h) @ ;"));
    loadSource(PSTR(": (l) #20 ;          : last (l) @ ;"));
    loadSource(PSTR(": base  #24 ;        : state #28 ;"));
    loadSource(PSTR(": sp0   #32 @ ;      // : rp0   #36 @ ;"));
    loadSource(PSTR(": (dsp) #40 ;        : dsp (dsp) @ ;"));
    loadSource(PSTR("// : (rsp) #44 ;        : rsp (rsp) @ ;"));
    loadSource(PSTR(": !sp 0 (dsp) ! ;    // : !rsp 0 (rsp) ! ;"));
    loadSource(PSTR("// : cells 4 * ;        // : cell+ 4 + ;"));
    loadSource(PSTR(": inline     ;"));
    loadSource(PSTR("// : immediate 1 last addr + c! ;"));
    loadSource(PSTR(": nip swap drop   ; inline"));
    loadSource(PSTR(": tuck swap over  ; inline"));
    loadSource(PSTR(": +! tuck @ + swap ! ;"));
    loadSource(PSTR(": <> = 0 = ; inline"));
    loadSource(PSTR(": ?dup dup if dup then ;"));
    loadSource(PSTR(": abs dup 0 < if 0 swap - then ;"));
    loadSource(PSTR(": depth dsp 1- ;"));
    loadSource(PSTR("// : pick depth swap - 1- cells sp0 + @ ;"));
    // loadSource(PSTR(": rot  swap r> swap ;"));
    loadSource(PSTR(": min over over < if drop else nip then ;"));
    loadSource(PSTR(": max over over > if drop else nip then ;"));
    // loadSource(PSTR(": between rot dup >r min max r> = ;"));
    loadSource(PSTR(": cr #13 emit #10 emit ;"));
    loadSource(PSTR(": space $20 emit ; inline"));
    loadSource(PSTR(": tab #9 emit ; inline"));
    loadSource(PSTR(": count dup 1+ swap c@ ;"));
    loadSource(PSTR(": mw@ @ ;"));
    loadSource(PSTR(": mc@ mw@ $ff and ;"));
    loadSource(PSTR(": type dup 0 = if drop drop leave then begin c@  emit 1- while ;"));
    loadSource(PSTR(": mtype dup 0 = if drop drop leave then begin mc@  emit 1- while ;"));
    loadSource(PSTR(": . 0 num>str space type ;"));
    loadSource(PSTR(": .2 2 num>str type ; : .4 4 num>str type ;"));
    loadSource(PSTR(": hex $10 base ! ; : decimal #10 base ! ; : binary 2 base ! ;"));
    loadSource(PSTR(": allot here + (h) ! ;"));
    loadSource(PSTR(": -fl- last  ! last a@ (l) ! ;"));
    loadSource(PSTR("// : [ 0 state ! ; immediate"));
    loadSource(PSTR("// : ] 1 state ! ;"));
    //loadSource(PSTR(": words 0 >r last begin"
    //    " 1- dup 24 * dict + 3 + dup mc@ #31 and swap 1+ swap mtype"
    //    " r> 1 + >r r@ 10 mod 0 = if cr else #9 emit then"
    //    " while- drop r> drop ;"));
}

void loadUserWords() {
    char* buf = (char*)&dict[sys->HERE + 256];
    sprintf(buf, ": code $%lx ;", (ulong)&dict[0]);
    parseLine(buf);
    sprintf(buf, ": code-sz #%lu ;", (ulong)DICT_SZ);
    parseLine(buf);
    // sprintf(buf, ": dpin-base #%ld ; : apin-base #%ld ;", (long)0, (long)A0);
    // parseLine(buf);

// 10 general purpose registers
// variable regs 10 allot \
// variable $r 0 $r ! \
// : cur$r dup 0 10 between if $r ! else drop then ; \
// : $r! $r @ regs + ! ; \
// : $r@ $r @ regs + @ ; \
// : $r@+ $r@ $r@ 1+ $r! ; \

// dc: dump code
// : d1 base @ $10 = if space .2 else . then ; \
// : d16 0 #16 do dup here < if dup c@ d1 then 1+ loop ; \
// : dc 0 begin cr dup .4 ':' emit d16 dup here < while drop ; \

    loadSource(PSTR(": m@  dup 1+ 1+ mw@ $10000 * swap mw@ or ;"));
    loadSource(PSTR(": mw! over   $100 / over 1+ mc! mc! ;"));
    loadSource(PSTR(": m!  over $10000 / over 1+ 1+ mw! mw! ;"));
    loadSource(PSTR(": auto-run-last last >body 0 a! ;"));
    loadSource(PSTR(": auto-run-off 0 0 a! ;"));
    loadSource(PSTR(": d-code 0 here do i c@ dup .2 space dup 32 < if drop '.' then dup 126 > if drop '.' then emit space loop ;"));
    
    loadSource(PSTR(": k 1000 * ; : mil k k ;"));
    loadSource(PSTR(": elapsed tick swap - 1000 /mod . . ;"));
    loadSource(PSTR(": bm tick swap begin 1- while- drop elapsed ;"));
    loadSource(PSTR(": low->high over over > if swap then ;"));
    // : dump+addr over . ':' space begin swap dup c@ space .2 1+ swap 1- while- ;
    loadSource(PSTR(": dump low->high do i c@ . loop ;"));
    loadSource(PSTR("variable (led)     : led (led) @ ; "));
    loadSource(PSTR("variable (pot)     : pot (pot) @ ; "));
    loadSource(PSTR("variable (button)  : button (button) @ ; "));
    loadSource(PSTR("variable (pot-lv)  : pot-lv (pot-lv) @ ; "));
    loadSource(PSTR("variable (pot-cv)  : pot-cv (pot-cv) @ ;"));
    loadSource(PSTR("variable (sens)    : sens (sens) @ ;"));
    loadSource(PSTR(": pot-val pot ap@ dup (pot-cv) ! ;"));
    loadSource(PSTR(": button->led button dp@ led dp! ;"));
    loadSource(PSTR(": .pot? pot-val pot-lv - abs sens > if pot-cv dup . cr (pot-lv) ! then ;"));
    loadSource(PSTR(": go button->led .pot? ;"));
    loadSource(PSTR(" 22 (led) ! 3 (pot) ! 6 (button) ! 4 (sens) !"));
    // loadSource(PSTR("led output pot input button input"));
    // loadSource(PSTR("auto-run-last"));
}

void ok() {
    printString(" ok. ");
    fDOTS();
    printString("\r\n");
}
