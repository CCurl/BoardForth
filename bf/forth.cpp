// forth.cpp

#include "defs.h"

#ifdef __DEV_BOARD__
    #define PM_INPUT INPUT
#else
    int analogRead(int p)          { printStringF("-analogRead(%d)-", p);         return 0; }
    int analogWrite(int p, int v)  { printStringF("-analogWrite(%d,%d)-", p, v);  return 0; }
    int digitalRead(int p)         { printStringF("-digitalRead(%d)-", p);        return 0; }
    int digitalWrite(int p, int v) { printStringF("-digitalWrite(%d,%d)-", p, v); return 0; }
    void pinMode(int p, int m)     { printStringF("-pinMode(%d,%d)-", p, m); }
    long millis() { return GetTickCount(); }
    void loadSource(const char* source);
    typedef unsigned int uint;
    typedef unsigned long ulong;
    #define PM_INPUT 1
    #define INPUT_PULLUP 2
    #define INPUT_PULLDOWN 3
    #define OUTPUT 4
    #pragma warning(disable: 4996)
#endif


CELL* dstk;
CELL* rstk;

BYTE dict[DICT_SZ + 1];
DICT_T words[WORDS_SZ];
SYSVARS_T* sys;
CELL reg[26];
int curReg;
CELL PC;
int isS4Mode = 0;

typedef struct {
    char name[16];
    char codes[16];
} s4_word_t;

s4_word_t s4Macros[] = {
     {"CELL", "#4"} ,{"CELLS", "#4*"} ,{"WORD", "#2"} ,{"WORDS", "#2*"}
    ,{"mod", "%"} ,{"negate", "n0-S-"}
    ,{"-", "-"}, {"+", "+"} ,{"*", "*"} ,{"/", "/"}
    ,{"1-", "1-"} ,{"1+", "1+"}
    ,{"<", "<"} ,{"<=", "<="} ,{"=", "="} ,{"<>", "<>"} ,{">=", ">="} ,{">", ">"}
    ,{"and", "&"} ,{"or", "|"} ,{"xor", "^"} ,{"not", "~"}
    ,{"dup", "D"} ,{"drop", "\\"} ,{"swap", "S"} ,{"over", "O"} ,{"nip", "S\\"} ,{"tuck", "SO"}
    ,{"c@", "C@"} ,{"@", "@"}
    ,{"c!", "C!"} ,{"!", "!"}
    ,{"tick", "T"}
    ,{"emit", ","}
    ,{"fopen", "FO"} ,{"fclose", "FC"}
    ,{"leave", ";"}, {"words", "Id"}
    ,{".", ".$20,"}, {"(.)", "."}
    ,{"space", "$20,"} ,{"cr", "$d,$a,"} ,{"tab", "#9,"}
    ,{".si", "IA"} ,{".ic", "IC"} ,{".iw", "ID"} ,{".ir", "IR"} ,{".s", "Is"}
    ,{"mc@", "M@"}, {"mc!", "M!"}
    ,{"pin-input", "POI"} ,{"pin-output", "POO"} ,{"pin-pullup", "POU"} ,{"pin-pulldown", "POD"}
    ,{"dp-read", "PRD"} ,{"dp-write", "PWD"} ,{"ap-read", "PRA"} ,{"ap-write", "PWA"}
    ,{"", ""}
};

void s4CompileString(const char* str) {
    while (*str) { CCOMMA(*(str++)); }
}

void s4RunString(const char* str) {
    CELL xt = sys->HERE + 100;
    CELL t1 = xt;
    while (*str) { dict[t1++] = *(str++); }
    dict[t1] = 0;
    run(xt, 0);
}

s4_word_t* s4FindMacro(char* w) {
    for (int i = 0; ; i++) {
        s4_word_t* p = &s4Macros[i];
        if (p->name[0] == 0) { return 0; }
        if (strcmp(p->name, w) == 0) {
            return p;
        }
    }
    return 0;
}

void s4PutAddress(CELL tgt, CELL val) {
    dict[tgt + 0] = (val & 0xFF);
    dict[tgt + 1] = ((val >> 8) & (0xFF));
}

int s4Parse(char* w) {
    if (isS4Mode) {
        if (strcmp(w, "forth") == 0) {
            isS4Mode = 0;
            return 1;
        }
        (sys->STATE) ? s4CompileString(w) : s4RunString(w);
        return 1;
    }

    if (strcmp(w, "s4") == 0){
        isS4Mode = 1;
        return 1;
    }

    s4_word_t* p = s4FindMacro(w);
    if (p) {
        if (sys->STATE) {
            s4CompileString(p->codes);
        }
        else {
            s4RunString(p->codes);
        }
        return 1;
    }
    return 0;
}

int doQuote(int pc) {
    char buf[2]; buf[1] = 0;
    while ((dict[pc]) && (pc < DICT_SZ) && (dict[pc] != '"')) {
        buf[0] = dict[pc++];
        printString(buf);
    }
    return pc+1;
}

int doPins(int pc) {
    BYTE op = dict[pc++]; 
    BYTE arg = dict[pc++];
    CELL pin = pop();
    if (op == 'O') { 
        if (arg == 'I') { pinMode(pin, PM_INPUT); }
        if (arg == 'U') { pinMode(pin, INPUT_PULLUP); }
        if (arg == 'D') { pinMode(pin, INPUT_PULLDOWN); }
        if (arg == 'O') { pinMode(pin, OUTPUT); }
    } else if (op == 'R') {
        if (arg == 'A') { push(analogRead(pin)); }
        if (arg == 'D') { push(digitalRead(pin)); }
    }
    else if (op == 'W') {
        CELL val = pop();
        if (arg == 'A') { analogWrite(pin, val); }
        if (arg == 'D') { digitalWrite(pin, val); }
    }
    return pc;
}

CELL doReg(CELL pc, BYTE ir) {
    curReg = ir - 'a'; 
    CELL t1 = dict[pc];
    CELL t2 = dict[pc + 1];
    if (t1 == '!') { ++pc; reg[curReg] = pop(); }
    if (t1 == '@') {
        ++pc; push(reg[curReg]);
        if (t2 == '+') { ++pc; ++reg[curReg]; }
        if (t2 == '-') { ++pc; --reg[curReg]; }
    }
    return pc;
}

void run(CELL pc, CELL max_cycles) {
    CELL t1, t2;
    char ir, buf[2];
    buf[1] = 0;
    while (1) {
        if (max_cycles && (--max_cycles < 1)) { return; }
        if ((pc < 0) || (DICT_SZ <= pc)) { return; }
        ir = dict[pc++];
        // printStringF("\r\n-PC-%d/%lx:IR-%d/%x-", PC-1, PC-1, (int)IR, (unsigned int)IR); fDOTS();
        switch (ir) {
        case 0: pc = -1; break;                             // 0
        case ' ': break;                                    // 32
        case '!': t1 = pop(); t2 = pop();                   // 33
            if (inAddrSpace(t1)) { cellStore(t1, t2); }
            break;
        case '"': pc = doQuote(pc);     break;              // 34
        case '#': pc = s4Number(pc, 10); break;             // 35
        case '$': pc = s4Number(pc, 16); break;             // 36
        case '%': t1 = pop(); T %= t1;  break;              // 37
        case '&': t1 = pop(); T &= t1;  break;              // 38
        case '\'': push(dict[pc++]);    break;              // 39
        case '(': if (pop() == 0) { pc = addrAt(pc); }      // 40
                else { pc += ADDR_SZ; }
            break;
        case ')': break; /* *** FREE *** */                 // 41
        case '*': t1 = pop(); T *= t1;  break;              // 42
        case '+': t1 = pop(); T += t1; break;               // 45
        case ',': buf[0] = pop()&0xFF; printString(buf);    // 44
            break;
        case '-': t1 = pop(); T -= t1; break;                 // 45
        case '.': t1 = pop(); doNumOut(t1, sys->BASE); break; // 46
        case '/': t1 = pop(); T = (t1) ? T / t1 : -1; break;  // 47
        //case '0': case '1': case '2': case '3': case '4':   // 48-57
        //case '5': case '6': case '7': case '8': case '9':
        case '1': t1 = dict[pc];                              // 49
            if (t1 == '-') { ++pc; --T; }
            if (t1 == '+') { ++pc; ++T; }
            break;
        case ':': rpush(pc + 4); pc = s4addrAt(pc); break;  // 58
        case ';': pc = rpop(); break;                       // 59
        case '<': t1 = dict[pc];  t2 = pop();               // 60
            if (t1 == '=') { T = (T <= t2) ? -1 : 0; ++pc; }
            else if (t1 == '>') { T = (T != t2) ? -1 : 0; ++pc; }
            else { T = (T < t2) ? -1 : 0; }
            break;
        case '=': t2 = pop(); T = (T == t2) ? -1 : 0; break; // 61
        case '>': t1 = dict[pc]; t2 = pop();                 // 62
            if (t1 == '=') { T = (T >= t2) ? -1 : 0; ++pc; }
            else { T = (T > t2) ? -1 : 0; }
            break;
     // case '?': push(_getch());                   break;  // 63
        case '@': T = cellAt(T);                    break;  // 64
        case 'A': break;   /* *** FREE ***  */
        case 'B': break;   /* *** FREE ***  */
        case 'C': t1 = dict[pc++];
            if (t1 == '@') { T = (inAddrSpace(T)) ? dict[T] : 0; }
            if (t1 == '!') { t1 = pop(); t2 = pop(); if (inAddrSpace(t1)) { dict[t1] = (byte)t2; } }
            break;
        case 'D': push(T); break;
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
        case 'H': T = (T << 8); break;
        case 'I': t1 = dict[pc++];
            if (t1 == 'A') { dumpAll(); }
            if (t1 == 'C') { dumpCode(); }
            if (t1 == 'D') { dumpDict(1); }
            if (t1 == 'd') { dumpDict(0); }
            if (t1 == 'R') { dumpRegs(); }
            if (t1 == 'S') { dumpStack(1); }
            if (t1 == 's') { dumpStack(0); }
            break;
        case 'J': t1 = dict[pc++];
            if (t1 == 'J') { pc = addrAt(pc); }
            if (t1 == 'Z') { if (pop() == 0) { pc = addrAt(pc); } else { pc += ADDR_SZ; } }
            if (t1 == 'N') { if (pop()) { pc = addrAt(pc); } else { pc += ADDR_SZ; } }
            if (t1 == 'z') { if (T == 0) { pc = addrAt(pc); } else { pc += ADDR_SZ; } }
            if (t1 == 'n') { if (T) { pc = addrAt(pc); } else { pc += ADDR_SZ; } }
            break;
        case 'K': T *= 1000; break;
        case 'L': break;   /* *** FREE ***  */
        case 'M': t1 = dict[pc++];
            if (t1 == '@') { T = *((byte*)T); }
            if (t1 == '!') { *((byte*)T) = (N & 0xFF); }
            break;
        case 'N': pc = doNumber(pc); break;
        case 'O': push(N); break;
        case 'P': pc = doPins(pc);
            break;
        case 'Q': break;   /* *** FREE ***  */
        case 'R': break;   /* *** FREE ***  */
        case 'S': t1 = T; T = N; N = t1;   break;
        case 'T': push(millis()); break;
        case 'U': break;   /* *** FREE ***  */
        case 'V': break;   /* *** FREE ***  */
        // case 'W': delay(pop()); break;
        case 'X': t1 = dict[pc++]; if (t1 == 'X') { vmInit(); } break;
        case 'Y': break;   /* *** FREE ***  */
        case 'Z': break;   /* *** FREE ***  */
        case '[': rpush(pc); break;                         // 91
        case '\\': pop(); break;                            // 92
        case ']': if (T) { pc = R; }                        // 93
                else { pop(); rpop(); }
                break;
        case '^': t1 = pop(); T ^= t1;  break;              // 94
        case '_': break;   /* *** FREE ***  */              // 95
        case '`': break;   /* *** FREE ***  */              // 96
        case 'a': case 'b': case 'c': case 'd': case 'e':
        case 'f': case 'g': case 'h': case 'i': case 'j':
        case 'k': case 'l': case 'm': case 'n': case 'o':
        case 'p': case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x': case 'y':
        case 'z': pc = doReg(pc, ir); break;
        case '{': break;   /* *** FREE ***  */              // 123
        case '|': t1 = pop(); T |= t1; break;               // 124
        case '}': break;   /* *** FREE ***  */              // 125
        case '~': T = ~T; break;                            // 126
        }
    }
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
    sys->DSTACK = sys->TIB - (CELL_SZ * STK_SZ) - CELL_SZ;
    sys->RSTACK = sys->DSTACK - (CELL_SZ * STK_SZ) - CELL_SZ;
    dstk = (CELL*)&dict[sys->DSTACK];
    rstk = (CELL*)&dict[sys->RSTACK];
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

int inAddrSpace(CELL loc) {
    return ((0 <= loc) && (loc < DICT_SZ)) ? 1 : 0;
}

CELL cellAt(CELL loc) {
    if (!inAddrSpace(loc)) { return 0; }
#ifdef __NEEDS_ALIGN__
    return (dict[loc + 3] << 24) | (dict[loc + 2] << 16) | (dict[loc + 1] << 8) | dict[loc];
#else
    return *((CELL*)&dict[loc]);
#endif
}

CELL wordAt(CELL loc) {
    if (!inAddrSpace(loc)) { return 0; }
#ifdef __NEEDS_ALIGN__
    return (dict[loc + 1] << 8) | dict[loc];
#else
    return *((WORD*)&dict[loc]);
#endif
}

int s4Digit(BYTE c) {
    if (('0' <= c) && (c <= '9')) return c - '0';
    if (('A' <= c) && (c <= 'F')) return (c + 10) - 'A';
    if (('a' <= c) && (c <= 'f')) return (c + 10) - 'a';
    return -1;
}

void doNumOut(CELL num, int base) {
    if (num == 0) { printString("0"); return; }
    int isNeg = ((base == 10) && (num < 0)) ? 1 : 0;
    if (isNeg) { num = -num; }
    char buf[36], *cp = buf+35;
    *(cp) = 0;
    while (num) {
        int r = num % base;
        if ((10 < base) && (9 < r)) { r += 7; }
        *(--cp) = r + '0';
        num = num / base;
    }
    if (isNeg) { *(--cp); }
    printString(cp);
}

CELL doNumber(CELL pc) {
    BYTE n = dict[pc++];
    push(dict[pc++]);
    if (n > '1') {
        T |= (dict[pc++] << 8);
    }
    if (n > '2') {
        T |= (dict[pc++] << 16);
        T |= (dict[pc++] << 24);
    }
    return pc;
}

CELL hexNumberAt(CELL loc) {
    int d = s4Digit(dict[loc]);
    CELL r = 0;
    while (0 <= d) {
        r = (r << 4) + d;
        ++loc;
        d = s4Digit(dict[loc]);
    }
    push(r);
    return loc;
}

CELL numberAt(CELL loc) {
    int d = isdigit(dict[loc]);
    CELL r = 0;
    while (0 <= d) {
        r = (r << 4) + d;
        ++loc;
        d = s4Digit(dict[loc]);
    }
    push(r);
    return loc;
}

CELL s4addrAt(CELL loc) {
    CELL r = (s4Digit(dict[loc]) << 12);
    r |= (s4Digit(dict[loc + 1]) << 8);
    r |= (s4Digit(dict[loc + 2]) << 4);
    r |= s4Digit(dict[loc + 3]);
    return r;
}

CELL addrAt(CELL loc) {
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
        CELL xt = dp->XT;
        sprintf(s4, ":%04x", (WORD)xt);
        if (compiling(w, 0)) {
            s4CompileString(s4);
        }
        else { s4RunString(s4); }
        return;
    }

    if (isNumber(w)) {
        if (compiling(w, 0)) {
            t1 = pop();
            CCOMMA('N');
            if ((0 <= t1) && (t1 <= 0xFF)) {
                CCOMMA('1');
                CCOMMA((BYTE)t1);
            } else if ((0x0100 <= t1) && (t1 <= 0xFFFF)) {
                CCOMMA('2');
                WCOMMA((WORD)t1);
            } else {
                CCOMMA('4');
                COMMA(t1);
            }
        }
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
        s4CompileString("00");
        return;
    }

    if (strcmp(w, "else") == 0) {
        if (!compiling(w, 1)) { return; }
        t1 = pop();
        s4CompileString("JJ");
        push(sys->HERE);
        s4CompileString("00");
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
            push(wa);
            fCREATE();
            s4CompileString("N2");
            WCOMMA((WORD)sys->HERE+3);
            CCOMMA(';');
            COMMA(0);
        }
        return;
    }

    if (strcmp(w, "constant") == 0) {
        if (!interpreting(w, 1)) { return; }
        push(wa);
        fNEXTWORD();
        if (pop()) {
            push(wa);
            fCREATE();
            s4CompileString("N4");
            COMMA(pop());
            CCOMMA(';');
        }
        return;
    }

    if (strcmp(w, ".\"") == 0) {
        if (!compiling(w, 1)) { return; }
        BYTE c = nextChar();
        int len = 0;
        CCOMMA('"');
        while (c && (c != '"')) {
            CCOMMA(c);
            c = nextChar();
            ++len;
        }
        CCOMMA('"');
        return;
    }

    sys->STATE = 0;
    printStringF("[%s]??", w);
}
void fPARSELINE() {    // opcode #60
    sys->TOIN = pop();
    if (isS4Mode) {
        char* l = (char*)&dict[sys->TOIN];
        while (*l && (*l < 33)) { ++l; }
        if (*l) s4Parse(l);
        return;
    }
    CELL buf = sys->HERE + 24;
    char* w = (char*)&dict[buf];
    push(buf);
    fNEXTWORD();
    while (pop()) {
        // printStringF("-pw:%s-", w);
        if (strcmp(w, "//") == 0) { break; }
        if (strcmp(w, "\\") == 0) { break; }
        push(buf);
        fPARSEWORD();
        buf = sys->HERE + 24;
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
        printStringF("-dictionary overflow-");
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
    CELL cl = sys->LAST - 1;
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
int isDigit(char c, int base) {
    char min = '0', max = min + base - 1;
    max = (base == 2) ? '1' : max;
    if ((min <= c) && (c <= max)) { return c - '0'; }
    if (base == 16) {
        if (('a' <= c) && (c <= 'f')) { return c - 'a' + 10; }
        if (('A' <= c) && (c <= 'F')) { return c - 'A' + 10; }
    }
    return -1;
}
int isNumber(char *w) {
    if ((*w == '\'') && (*(w+2) == '\'') && (*(w+3) == 0)) {
        push(*(w+1));
        return 1;
    }

    int base = sys->BASE;
    CELL num = 0, isNeg = 0;

    if (*w == '#') { w++; base = 10; }
    if (*w == '$') { w++; base = 16; }
    if (*w == '%') { w++; base =  2; }
    if ((*w == '-') && (base == 10)) { w++; isNeg = 1; }

    while (*w) {
        int n = isDigit(*w, base);
        if (n < 0) { return 0; }
        num = (num * base) + n;
        ++w;
    }

    if (isNeg) { num = -num; }
    push(num);
    return 1;
}
CELL s4Number(CELL pc, int base) {
    int c = isDigit(dict[pc], base);
    push(0);
    while (0 <= c) {
        T = (T*base) + c;
        c = isDigit(dict[++pc], base);
    }
    return pc;
}

void CCOMMA(BYTE v) {
    dict[sys->HERE++] = v; 
}
void WCOMMA(WORD v) {
    dict[sys->HERE++] = v & 0xFF; v = v >> 8;
    dict[sys->HERE++] = v & 0xFF;
}
void COMMA(CELL v) {
    dict[sys->HERE++] = v & 0xFF; v = v >> 8;
    dict[sys->HERE++] = v & 0xFF; v = v >> 8;
    dict[sys->HERE++] = v & 0xFF; v = v >> 8;
    dict[sys->HERE++] = v & 0xFF;
}

BYTE nextChar() {
    if (dict[sys->TOIN]) return dict[sys->TOIN++];
    return 0;
}

void parseLine(char* line) {
    CELL x = sys->TIB;
    while (*line) {
        dict[x++] = *(line++);
    }
    dict[x] = 0;
    push(sys->TIB);
    fPARSELINE();
}


void ok() {
    if (isS4Mode) { printString(" s4 "); }
    else { printString(" ok "); }
    dumpStack(0);
    printString("\r\n");
}

void dumpAll() { dumpCode(); dumpDict(1); dumpRegs(); dumpStack(1); }

void dumpCode() {
    char x[32];
    int n = 0;
    int c = 15;

    printStringF("\r\n; CODE: HERE=%04lx (%ld), FREE: %lu", sys->HERE, sys->HERE, (sys->RSTACK - sys->HERE));
    for (int i = 0; i < sys->HERE; i++) {
        if (++c == 16) {
            if (n) { x[n] = 0; printStringF(" ; %s", x); }
            printStringF("\r\n%04x:", i);
            n = 0;
            c = 0;
        }
        BYTE b = dict[i];
        x[n++] = ((31 < b) && (b < 128)) ? b : '.';
        printStringF(" %02x", dict[i]);
    }
    for (int i = c; i < 15; i++) {
        printStringF("   ");
    }
    if (n) { x[n] = 0; printStringF(" ; %s", x); }
}

void dumpDict(int hdr) { 
    if (!hdr) {
        int n = 0;
        for (int i = sys->LAST - 1; 0 <= i; i--) {
            DICT_T* dp = &words[i];
            if (++n == 8) { n = 0; printString("\r\n"); }
            printStringF("%s\t", dp->name);
        }
        return;
    }
    printStringF("\r\n; WORDS: LAST=%ld", sys->LAST);
    printStringF("\r\n  #   XT   d  f  l word");
    printStringF("\r\n---- ---- -- -- -- -----------------");
    for (int i = sys->LAST - 1; 0 <= i; i--) {
        DICT_T* dp = &words[i];
        int fl = dp->flagsLen;
        printStringF("\r\n%4d %04lx %2d %2d %2d %s", i,
            (CELL)dp->XT, (int)dp->dictionaryId, (fl >> 6), (fl & 0x1F), dp->name);
    }
}

void dumpRegs() {
    printStringF("\r\n; REGISTERS:"); 
    for (int i = 0; i < 26; i++) {
        printStringF("%s%c: %ld", ((i%8) == 0) ? "\r\n" : "\t", ('a'+i), reg[i]);
    }
}

void dumpStack(int hdr) {
    if (hdr) { printStringF("\r\nSTACK: "); }
    printString(" (");
    for (int i = 1; i <= sys->DSP; i++) {
        printStringF(" #%ld:$%lx", dstk[i], dstk[i]);
    }
    printString(" )");
}

void loadBaseSystem() {
    char buf[32];
    sprintf(buf, ": code %lu ;", (ulong)&dict[0]);  parseLine(buf);
    sprintf(buf, ": dict %lu ;", (ulong)&words[0]); parseLine(buf);
    sprintf(buf, ": code-sz %lu ;", (UCELL)DICT_SZ); parseLine(buf);
    loadSource(PSTR(": cell 4 ; : addr 2 ;"));
    loadSource(PSTR(": tib    #8 @ ;      : >in   #12 ;"));
    loadSource(PSTR(": (h) #16 ;          : here (h) @ ;"));
    loadSource(PSTR(": (l) #20 ;          : last (l) @ ;"));
    loadSource(PSTR(": base  #24 ;        : state #28 ;"));
    loadSource(PSTR(": sp0   #32 @ ;      // : rp0   #36 @ ;"));
    loadSource(PSTR(": (dsp) #40 ;        : dsp (dsp) @ ;"));
    loadSource(PSTR(": (rsp) #44 ;        : rsp (rsp) @ ;"));
    loadSource(PSTR(": !sp 0 (dsp) ! ;    // : !rsp 0 (rsp) ! ;"));
    loadSource(PSTR("// : cells 4 * ;        // : cell+ 4 + ;"));
    loadSource(PSTR(": +! tuck @ + swap ! ;"));
    loadSource(PSTR(": ?dup dup if dup then ;"));
    loadSource(PSTR(": abs dup 0 < if negate then ;"));
    loadSource(PSTR(": depth dsp 1- ;"));
    loadSource(PSTR(": min over over < if drop else nip then ;"));
    loadSource(PSTR(": max over over > if drop else nip then ;"));
    // loadSource(PSTR(": between rot dup >r min max r> = ;"));
    loadSource(PSTR(": count dup 1+ swap c@ ;"));
    loadSource(PSTR(": type begin swap dup c@ emit 1+ swap 1- while drop ;"));
    loadSource(PSTR(": mtype begin swap dup mc@ emit 1+ swap 1- while drop ;"));
    loadSource(PSTR(": hex $10 base ! ; : decimal #10 base ! ; : binary 2 base ! ;"));
    loadSource(PSTR(": allot here + (h) ! ;"));
}

void loadUserWords() {
    /*
    // dc: dump code
    // : d1 base @ $10 = if space .2 else . then ; \
    // : d16 0 #16 do dup here < if dup c@ d1 then 1+ loop ; \
    // : dc 0 begin cr dup .4 ':' emit d16 dup here < while drop ; \
    */
    // loadSource(PSTR(": m@  dup 1+ 1+ mw@ $10000 * swap mw@ or ;"));
    // loadSource(PSTR(": mw! over   $100 / over 1+ mc! mc! ;"));
    // loadSource(PSTR(": m!  over $10000 / over 1+ 1+ mw! mw! ;"));
    // loadSource(PSTR(": auto-run-last last >body 0 ! ;"));
    // loadSource(PSTR(": auto-run-off 0 0 ! ;"));
    // loadSource(PSTR(": d-code 0 here do i c@ dup .2 space dup 32 < if drop '.' then dup 126 > if drop '.' then emit space loop ;"));

    loadSource(PSTR(": k 1000 * ; : mil k k ;"));
    loadSource(PSTR(": elapsed tick swap - dup 1000 / . 1000 mod . ;"));
    loadSource(PSTR(": bm tick swap begin 1- while elapsed ;"));
    // loadSource(PSTR(": low->high over over > if swap then ;"));
    // : dump+addr over . ':' space begin swap dup c@ space .2 1+ swap 1- while- ;
    // loadSource(PSTR(": dump low->high do i c@ . loop ;"));
    // loadSource(PSTR("variable (led)     : led (led) @ ; "));
    // loadSource(PSTR("variable (pot)     : pot (pot) @ ; "));
    // loadSource(PSTR("variable (button)  : button (button) @ ; "));
    // loadSource(PSTR("variable (pot-lv)  : pot-lv (pot-lv) @ ; "));
    // loadSource(PSTR("variable (pot-cv)  : pot-cv (pot-cv) @ ;"));
    // loadSource(PSTR("variable (sens)    : sens (sens) @ ;"));
    // loadSource(PSTR(": pot-val pot ap@ dup (pot-cv) ! ;"));
    // loadSource(PSTR(": button->led button dp@ led dp! ;"));
    // loadSource(PSTR(": .pot? pot-val pot-lv - abs sens > if pot-cv dup . cr (pot-lv) ! then ;"));
    // loadSource(PSTR(": go button->led .pot? ;"));
    // loadSource(PSTR(" 22 (led) ! 3 (pot) ! 6 (button) ! 4 (sens) !"));
    // loadSource(PSTR("led output pot input button input"));
    // loadSource(PSTR("auto-run-last"));
    char* buf = (char *) &dict[sys->HERE + 10];
    loadSource("s4");
    loadSource("\"BoardForth v0.0.1 - Chris Curl\"");
    loadSource("$d,$a,\"Source: https://github.com/CCurl/BoardForth\"");
    sprintf(buf, "$d,$a,\"Dictionary size is: %d ($%04x) bytes.\"$d,$a,", (int)DICT_SZ, (int)DICT_SZ);
    loadSource(buf);
    loadSource("\"Hello.\"");
    loadSource("forth");
}
