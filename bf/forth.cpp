#include "board.h"
#ifndef __DEV_BOARD__
#include <windows.h>
#endif

#include "defs.h"
#pragma warning(disable: 4996)

CELL *dstk;
CELL *rstk;

BYTE dict[DICT_SZ];
DICT_T words[WORDS_SZ];
BYTE lastDictionaryId = 0;
SYSVARS_T* sys;
CELL loopSTK[12];
CELL loopDepth;

OPCODE_T opcodes[] = {
    { "nop",  OP_NOOP, 1 },
    { "cliteral",  OP_CLIT, 1 },
    { "wliteral",  OP_WLIT, 1 },
    { "literal",  OP_LIT, 1 },
    { "c@",  OP_CFETCH, 1 },
    { "w@",  OP_WFETCH, 1 },
    { "a@",  OP_AFETCH, 1 },
    { "@",  OP_FETCH, 1 },
    { "c!",  OP_CSTORE, 1 },
    { "w!",  OP_WSTORE, 1 },
    { "a!",  OP_ASTORE, 1 },
    { "!",  OP_STORE, 1 },
    { "c,",  OP_CCOMMA, 1 },
    { "w,",  OP_WCOMMA, 1 },
    { ",",  OP_COMMA, 1 },
    { "a,",  OP_ACOMMA, 1 },
    { "call",  OP_CALL, 1 },
    { "exit",  OP_RET, 1 },
    { "jmp",  OP_JMP, 1 },
    { "jmpz",  OP_JMPZ, 1 },
    { "jmpnz",  OP_JMPNZ, 1 },
    { "1-",  OP_ONEMINUS, 1 },
    { "1+",  OP_ONEPLUS, 1 },
    { "dup",  OP_DUP, 1 },
    { "swap",  OP_SWAP, 1 },
    { "drop",  OP_DROP, 1 },
    { "over",  OP_OVER, 1 },
    { "+",  OP_ADD, 1 },
    { "-",  OP_SUB, 1 },
    { "*",  OP_MULT, 1 },
    { "/mod",  OP_SLMOD, 1 },
    { "2*",  OP_LSHIFT, 1 },
    { "2/",  OP_RSHIFT, 1 },
    { "and",  OP_AND, 1 },
    { "or",  OP_OR, 1 },
    { "xor",  OP_XOR, 1 },
    { "not",  OP_NOT, 1 },
    { ">r",  OP_DTOR, 1 },
    { "r@",  OP_RFETCH, 1 },
    { "r>",  OP_RTOD, 1 },
    { "emit",  OP_EMIT, 1 },
    { "type",  OP_TYPE, 1 },
    { ".s",  OP_DOTS, 1 },
    { "s\\",  OP_SQUOTE, 1 },
    { "(",  OP_PAREN, 1 },
    { "wdtfeed",  OP_WDTFEED, 1 },
    { "brk",  OP_BREAK, 1 },
    { "cmove",  OP_CMOVE, 1 },
    { "cmove>",  OP_CMOVE2, 1 },
    { "fill",  OP_FILL, 1 },
    { "open-block",  OP_OPENBLOCK, 1 },
    { "file-close",  OP_FILECLOSE, 1 },
    { "file-read",  OP_FILEREAD, 1 },
    { "load",  OP_LOAD, 1 },
    { "thru",  OP_THRU, 1 },
    { "do",  OP_DO, 1 },
    { "loop",  OP_LOOP, 1 },
    { "loop+",  OP_LOOPP, 1 },
    { "debugger",  OP_DEBUGGER, 1 },
    { "parse-word",  OP_PARSEWORD, 1 },
    { "parse-line",  OP_PARSELINE, 1 },
    { ">body",  OP_GETXT, 1 },
    { "align2",  OP_ALIGN2, 1 },
    { "align4",  OP_ALIGN4, 1 },
    { "create",  OP_CREATE, 1 },
    { "find",  OP_FIND, 1 },
    { "next-word",  OP_NEXTWORD, 1 },
    { "number?",  OP_ISNUMBER, 1 },
    { "jmpz-",  OP_NJMPZ, 1 },
    { "jmpnz-",  OP_NJMPNZ, 1 },
    { "<",  OP_LESS, 1 },
    { "=",  OP_EQUALS, 1 },
    { ">",  OP_GREATER, 1 },
    { "i",  OP_I, 1 },
    { "input",  OP_INPUTPIN, 1 },
    { "output",  OP_OUTPUTPIN, 1 },
    { "ms",  OP_DELAY, 1 },
    { "tick",  OP_TICK, 1 },
    { "ap!",  OP_APINSTORE, 1 },
    { "dp!",  OP_DPINSTORE, 1 },
    { "ap@",  OP_APINFETCH, 1 },
    { "dp@",  OP_DPINFETCH, 1 },
    { "mw@",  OP_MWFETCH, 1 },
    { "mc!",  OP_MCSTORE, 1 },
    { "num>str",  OP_NUM2STR, 1 },
    { "bye",  OP_BYE, 1 },
    { "",  255, 0 }
};


void run(CELL PC, CELL max_cycles) {
    CELL t1, t2;
    BYTE IR, *a1;
    while (1) {
        if (max_cycles) {
            if (--max_cycles < 1) { return; }
        }
        IR = dict[PC++];
        // printStringF("\r\n-PC-%d/%lx:IR-%d/%x-", PC-1, PC-1, (int)IR, (unsigned int)IR); fDOTS();
        switch (IR) {
        case OP_NOOP:     // noop (#0)
            break;
        case OP_CLIT:     // cliteral (#1)
            push(dict[PC++]);
            break;
        case OP_WLIT:     // wliteral (#2)
            push(wordAt(PC));
            PC += WORD_SZ;
            break;
        case OP_LIT:     // literal (#3)
            push(cellAt(PC));
            PC += CELL_SZ;
            break;
        case OP_CFETCH:     // c@ (#4)
            T = dict[T];
            break;
        case OP_WFETCH:     // w@ (#5)
            T = wordAt(T);
            break;
        case OP_AFETCH:     // a@ (#6)
            T = addrAt(T);
            break;
        case OP_FETCH:     // @ (#7)
            T = cellAt(T);
            break;
        case OP_CSTORE:     // c! (#8)
            t2 = pop();
            t1 = pop();
            dict[t2] = (BYTE)t1;
            break;
        case OP_WSTORE:     // w! (#9)
            t2 = pop();
            t1 = pop();
            wordStore(t2, t1);
            break;
        case OP_ASTORE:     // a! (#10)
            t2 = pop();
            t1 = pop();
            addrStore(t2, t1);
            break;
        case OP_STORE:     // ! (#11)
            t2 = pop();
            t1 = pop();
            cellStore(t2, t1);
            break;
        case OP_CCOMMA:     // c, (#12)
            t1 = pop();
            dict[sys->HERE] = (t1 % 0xFF);
            sys->HERE += 1;
            break;
        case OP_WCOMMA:     // w, (#13)
            t1 = pop();
            wordStore(sys->HERE, t1);
            sys->HERE += WORD_SZ;
            break;
        case OP_COMMA:     // , (#14)
            t1 = pop();
            cellStore(sys->HERE, t1);
            sys->HERE += CELL_SZ;
            break;
        case OP_ACOMMA:     // a, (#15)
            t1 = pop();
            cellStore(sys->HERE, t1);
            sys->HERE += ADDR_SZ;
            break;
        case OP_CALL:     // call (#16)
            rpush(PC + ADDR_SZ);
            PC = addrAt(PC);
            break;
        case OP_RET:     // exit (#17)
            if (sys->RSP < 1) { return; }
            PC = rpop();
            break;
        case OP_JMP:     // -n- (#18)
            PC = addrAt(PC);
            break;
        case OP_JMPZ:     // -n- (#19)
            if (pop() == 0) { PC = addrAt(PC); }
            else { PC += ADDR_SZ; }
            break;
        case OP_JMPNZ:     // -n- (#20)
            if (pop() != 0) { PC = addrAt(PC); }
            else { PC += ADDR_SZ; }
            break;
        case OP_ONEMINUS:     // 1- (#21)
            T--;
            break;
        case OP_ONEPLUS:     // 1+ (#22)
            T++;
            break;
        case OP_DUP:     // dup (#23)
            push(T);
            break;
        case OP_SWAP:     // swap (#24)
            t1 = T;
            T = N;
            N = t1;
            break;
        case OP_DROP:     // drop (#25)
            pop();
            break;
        case OP_OVER:     // over (#26)
            push(N);
            break;
        case OP_ADD:     // + (#27)
            N += T; pop();
            break;
        case OP_SUB:     // - (#28)
            N -= T; pop();
            break;
        case OP_MULT:     // * (#29)
            N *= T; pop();
            break;
        case OP_SLMOD:     // /mod (#30)
            t2 = pop();
            t1 = pop();
            push(t1 % t2);
            push(t1 / t2);
            break;
        case OP_LSHIFT:     // << (#31)
            T *= 2;
            break;
        case OP_RSHIFT:     // >> (#32)
            T /= 2;
            break;
        case OP_AND:     // and (#33)
            N &= T; pop();
            break;
        case OP_OR:     // or (#34)
            N |= T; pop();
            break;
        case OP_XOR:     // xor (#35)
            N ^= T; pop();
            break;
        case OP_NOT:     // not (#36)
            T = (T == 0) ? 1 : 0;
            break;
        case OP_DTOR:     // >r (#37)
            rpush(pop());
            break;
        case OP_RFETCH:     // r@ (#38)
            push(R);
            break;
        case OP_RTOD:     // r> (#39)
            push(rpop());
            break;
        case OP_EMIT:     // emit (#40)
            fEMIT();
            break;
        case OP_TYPE:  // type (#41)
            fTYPE();
            break;
        case OP_DOTS:     // .s (#42)
            fDOTS();
            break;
        case OP_SQUOTE:     // s" (#43)
            push(PC);
            PC += (dict[PC] + 2);
            break;
        case OP_PAREN:     // ( (#44)
            // N += T; pop();
            break;
        case OP_WDTFEED:     // wdtfeed (#45)
            // N += T; pop();
            break;
        case OP_BREAK:     // brk (#46)
            // N += T; pop();
            break;
        case OP_CMOVE:     // cmove (#47)
            break;
        case OP_CMOVE2:     // cmove> (#48)
            break;
        case OP_FILL:     // fill (#49)
            break;
        case OP_OPENBLOCK:     // open-block (#50)
            // N += T; pop();
            break;
        case OP_FILECLOSE:     // file-close (#51)
            // N += T; pop();
            break;
        case OP_FILEREAD:     // file-read (#52)
            // N += T; pop();
            break;
        case OP_LOAD:     // load (#53)
            // N += T; pop();
            break;
        case OP_THRU:     // thru (#54)
            // N += T; pop();
            break;
        case OP_DO:     // do (#55)
            if (loopDepth < 4) {
                CELL t = pop();
                CELL f = pop();
                int x = loopDepth * 3;
                // printStringF("-DO(%ld,%ld,%d)-", f, t, f);
                loopSTK[x] = f;
                loopSTK[x + 1] = t;
                loopSTK[x + 2] = f;
                ++loopDepth;
            }
            else {
                printString("-DO:too deep-");
            }
            break;
        case OP_LOOP:     // loop (#56)
            if (loopDepth > 0) {
                int x = (loopDepth - 1) * 3;
                CELL f = loopSTK[x];
                CELL t = loopSTK[x + 1];
                loopSTK[x + 2] += 1;
                CELL i = loopSTK[x + 2];
                // printString("-LOOP(%ld,%ld,%d)-", f, t, i);
                if ((f < i) && (i < t)) {
                    push(1);
                }
                else {
                    loopDepth -= 1;
                    push(0);
                }
            }
            else {
                printString("-LOOP:depthErr-");
                push(0);
            }
            break;
        case OP_LOOPP:     // loop+ (#57)
            break;
        case OP_DEBUGGER:     // -n- (#58)
            printString("-debugger-");
            break;
        case OP_PARSEWORD:     // parse-word (#59)
            fPARSEWORD();
            break;
        case OP_PARSELINE:     // parse-line (#60)
            fPARSELINE();
            break;
        case OP_GETXT:     // >body (#61)
            {
                DICT_T* dp = &words[T];
                T = dp->XT;
            }
            break;
        case OP_ALIGN2:     // align2 (#62)
            t1 = T;
            if (t1 & 0x01) { ++t1; }
            T = t1;
            break;
        case OP_ALIGN4:     // align4 (#63)
            t1 = T;
            while (t1 & 0x03) { ++t1; }
            T = t1;
            break;
        case OP_CREATE:     // create (#64)
            fCREATE();
            break;
        case OP_FIND:     // find (#65)
            fFIND();
            break;
        case OP_NEXTWORD:     // next-word (#66)
            fNEXTWORD();
            break;
        case OP_ISNUMBER:     // number? (#67)
            fISNUMBER();
            break;
        case OP_NJMPZ:     // -n- (#68)
            if (T == 0) { PC = addrAt(PC); }
            else { PC += ADDR_SZ; }
            break;
        case OP_NJMPNZ:     // -n- (#69)
            if (T != 0) { PC = addrAt(PC); }
            else { PC += ADDR_SZ; }
            break;
        case OP_LESS:     // < (#70)
            N = (N < T) ? 1 : 0;
            pop();
            break;
        case OP_EQUALS:     // = (#71)
            N = (N == T) ? 1 : 0;
            pop();
            break;
        case OP_GREATER:     // > (#72)
            N = (N > T) ? 1 : 0;
            pop();
            break;
        case OP_I:     // i (#73)
            if (loopDepth > 0) {
                t1 = (loopDepth - 1) * 3;
                push(loopSTK[t1 + 2]);
            }
            else {
                printString("-I:depthErr-");
                push(0);
            }
            break;
        case OP_FREE_74:     // j (#74)
            break;
        case OP_INPUTPIN:     // input (#75)
            t1 = pop();
            #ifdef __DEV_BOARD__
                // printStringF("-pinMode(%d, INPUT)-", pin);
                pinMode(t1, INPUT);
            #else
                printStringF("-pinMode(%d, INPUT)-", t1);
            #endif
            break;
        case OP_OUTPUTPIN:     // output (#76)
            t1 = pop();
            #ifdef __DEV_BOARD__
                // printStringF("-pinMode(%d, OUTPUT)-", t1);
                pinMode(t1, OUTPUT);
            #else
                printStringF("-pinMode(%d, OUTPUT)-", t1);
            #endif
            break;
        case OP_DELAY:     // ms (#77)
             t1 = pop();
            #ifdef __DEV_BOARD__
                delay(t1);
            #else
                Sleep(t1);
            #endif
            break;
        case OP_TICK:     // ms (#78)
            #ifdef __DEV_BOARD__
                push(millis());
            #else
                push(GetTickCount());
            #endif
            break;
        case OP_APINSTORE:     //  (#79)
            t2 = pop();
            t1 = pop();
            #ifdef __DEV_BOARD__
                // printStringF("-analogWrite(%d, OUTPUT)-", t2);
                analogWrite((int)t2, (int)t1);
            #else
                printStringF("-analogWrite(%ld, %ld)-", t2, t1);
            #endif
            break;
        case OP_DPINSTORE:     // dp! (#80)
            t2 = pop();
            t1 = pop();
            #ifdef __DEV_BOARD__
                // printStringF("-digitalWrite(%d, %d)-", (int)t2, (int)t1);
                digitalWrite((int)t2, (int)t1);
            #else
                printStringF("-digitalWrite(%ld, %ld)-", t2, t1);
            #endif
            break;
        case OP_APINFETCH:     // ap@ (#81)
            #ifdef __DEV_BOARD__
                // printStringF("-analogRead(%d, A0=%d)-", T, A0);
                T = analogRead((int)T);
            #else
                printStringF("-analogRead(%ld)-", T);
            #endif
            break;
        case OP_DPINFETCH:     // dp@ (#82)
            #ifdef __DEV_BOARD__
            // printStringF("-digitalRead(%d)-", T);
                T = digitalRead((int)T);
            #else
                printStringF("-digitalRead(%ld)-", T);
            #endif
            break;
        case OP_MWFETCH:     // mw@ (#83)
            a1 = (BYTE*)T;
            T = (*(a1 + 1) << 8) | (*a1);
            break;
        case OP_MCSTORE:     // mc! (#84)
            a1 = (BYTE*)pop();
            t1 = pop();
            *a1 = (BYTE)t1;
            break;
        case OP_NUM2STR:     // num>str (#85)
            fNUM2STR();
            break;
        case OP_BYE:     // bye (#87)
            // N += T; pop();
            break;

        }
    }
}

void runOpcode(BYTE opcode) {
    CELL xt = sys->HERE + 17;
    dict[xt] = opcode;
    dict[xt+1] = OP_RET;
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
        fprintf(to, "\r\n%4d %04lx %3d %02x, %s", i, (CELL)dp->XT, (int)dp->dictionaryId, (int)dp->flagsLen, dp->name);
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

    fprintf(to, "\r\n\r\n; OPCODES:");
    for (int i = 0; opcodes[i].opcode != 255; i++) {
        OPCODE_T* op = &opcodes[i];
        int c = op->opcode;
        int c1 = ((31 < c) && (c < 127)) ? c : '.';
        fprintf(to, "\r\n%02x (%3d, %c) %d %s", c, c, c1, (int)op->makeWord, op->name);
    }
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
void fPARSEWORD() {    // opcode #59
    CELL wa = pop();
    char* w = (char*)&dict[wa];
    // printStringF("-pw[%s]-", w);
    push(wa); fFIND();
    if (pop()) {
        DICT_T* dp = &words[pop()];

        // runOpcode(OP_GETXT);
        CELL xt = dp->XT;
        if (compiling(w, 0)) {
            if ((dp->flagsLen & 0x20) == 1) {
                // 1 => IMMEDIATE
                run(xt, 0);
            }
            else if (dp->flagsLen & 0x40) {
                // 2 => INLINE
                BYTE x = dict[xt];
                while (x != OP_RET) {
                    CCOMMA(x);
                    x = dict[++xt];
                }
            }
            else {
                CCOMMA(OP_CALL);
                ACOMMA((ADDR)xt);
            }
        }
        else {
            run(xt, 0);
        }
        return;
    }

    push(wa); fISNUMBER();
    if (pop()) {
        if (compiling(w, 0)) {
            if ((0x0000 <= T) && (T < 0x0100)) {
                CCOMMA(OP_CLIT);
                fCCOMMA();
            }
            else if ((0x0100 <= T) && (T < 0x010000)) {
                CCOMMA(OP_WLIT);
                fWCOMMA();
            }
            else {
                CCOMMA(OP_LIT);
                fCOMMA();
            }
        }
        return;
    }

    if (strcmp(w, ";") == 0) {
        if (!compiling(w, 1)) { return; }
        CCOMMA(OP_RET);
        sys->STATE = 0;
        return;
    }

    if (strcmp(w, "if") == 0) {
        if (!compiling(w, 1)) { return; }
        CCOMMA(OP_JMPZ);
        push(sys->HERE);
        ACOMMA(0);
        return;
    }

    if (strcmp(w, "if-") == 0) {
        if (!compiling(w, 1)) { return; }
        CCOMMA(OP_NJMPZ);
        push(sys->HERE);
        ACOMMA(0);
        return;
    }

    if (strcmp(w, "else") == 0) {
        if (!compiling(w, 1)) { return; }
        CCOMMA(OP_JMP);
        push(sys->HERE);
        fSWAP();
        ACOMMA(0);
        push(sys->HERE);
        fSWAP();
        fASTORE();
        return;
    }

    if (strcmp(w, "then") == 0) {
        if (!compiling(w, 1)) { return; }
        push(sys->HERE);
        fSWAP();
        fASTORE();
        return;
    }

    if (strcmp(w, "begin") == 0) {
        if (!compiling(w, 1)) { return; }
        push(sys->HERE);
        return;
    }

    if (strcmp(w, "repeat") == 0) {
        if (!compiling(w, 1)) { return; }
        CCOMMA(OP_JMP);
        fACOMMA();
        return;
    }

    if (strcmp(w, "while") == 0) {
        if (!compiling(w, 1)) { return; }
        CCOMMA(OP_JMPNZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "until") == 0) {
        if (!compiling(w, 1)) { return; }
        CCOMMA(OP_JMPZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "while-") == 0) {
        if (!compiling(w, 1)) { return; }
        CCOMMA(OP_NJMPNZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "until-") == 0) {
        if (!compiling(w, 1)) { return; }
        CCOMMA(OP_NJMPZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "do") == 0) {
        if (!compiling(w, 1)) { return; }
        CCOMMA(OP_DO);
        push(sys->HERE);
        return;
    }

    if (strcmp(w, "leave") == 0) {
        if (!compiling(w, 1)) { return; }
        printString("WARNING: LEAVE not supported!");
        return;
    }

    if (strcmp(w, "loop") == 0) {
        if (!compiling(w, 1)) { return; }
        CCOMMA(OP_LOOP);
        CCOMMA(OP_JMPNZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "loop+") == 0) {
        if (!compiling(w, 1)) { return; }
        CCOMMA(OP_LOOPP);
        CCOMMA(OP_JMPNZ);
        fACOMMA();
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

    if (strcmp(w, "dictionary") == 0) {
        if (!interpreting(w, 1)) { return; }
        push(wa);
        fNEXTWORD();
        if (pop()) {
            sys->currentDictId = ++lastDictionaryId;
            push(wa);
            fCREATE();
            CCOMMA(OP_CLIT);
            CCOMMA(sys->currentDictId);
            CCOMMA(OP_RET);
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
            CCOMMA(OP_LIT);
            COMMA(sys->HERE + CELL_SZ + 1);
            CCOMMA(OP_RET);
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
            CCOMMA(OP_LIT);
            fCOMMA();
            CCOMMA(OP_RET);
        }
        return;
    }

    if (strcmp(w, "s\"") == 0) {
        if (! compiling(w, 1)) { return; }
        BYTE c = nextChar();
        int len = 0;
        CCOMMA(OP_SQUOTE);
        CELL h = sys->HERE;
        CCOMMA(0);
        while (c && (c != '"')) {
            CCOMMA(c);
            c = nextChar();
            ++len;
        }
        CCOMMA(0);
        dict[h] = len;
        return;
    }

    BYTE op = getOpcode(w);
    if (op < 0xFF) {
        if (compiling(w, 0)) {
            CCOMMA(op);
        }
        else {
            CELL xt = sys->HERE + 0x20;
            dict[xt] = op;
            dict[xt + 1] = OP_RET;
            run(xt, 0);
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

void genOpcodeWords() {
    for (int i = 0; ; i++) {
        OPCODE_T* op = &opcodes[i];
        if (op->opcode == 255) { return; }
        if (op->makeWord) {
            CELL to = sys->HERE + 100;
            stringToDict(op->name, to);
            push(to);
            fCREATE();
            CCOMMA(op->opcode);
            CCOMMA(OP_RET);
        }
    }
}

BYTE getOpcode(char* w) {
    for (int i = 0; ; i++) {
        OPCODE_T* op = &opcodes[i];
        if (op->opcode == 255) { return 0xFF; }
        if ((strcmp(w, op->name) == 0) && (op->makeWord)) {
            return op->opcode;
        }
    }
}

void CCOMMA(BYTE v) { push(v); fCCOMMA(); }
void WCOMMA(WORD v) { push(v); fWCOMMA(); }
void COMMA(CELL v) { push(v); fCOMMA(); }
void ACOMMA(ADDR v) { push(v); fACOMMA(); }

CELL getXT(CELL addr) {
    push(addr);
    runOpcode(OP_GETXT);
    return pop();
}

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
    loadSource(PSTR(": forth 0 ; : cell 4 ; : addr 2 ;"));
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
    loadSource(PSTR(": / /mod nip      ; inline"));
    loadSource(PSTR(": mod /mod drop   ; inline"));
    loadSource(PSTR(": +! tuck @ + swap ! ;"));
    loadSource(PSTR(": <> = not ; inline"));
    loadSource(PSTR(": ?dup if- dup then ;"));
    loadSource(PSTR(": abs dup 0 < if 0 swap - then ;"));
    loadSource(PSTR(": execute >r ;"));
    loadSource(PSTR(": depth dsp 1- ;"));
    loadSource(PSTR("// : pick depth swap - 1- cells sp0 + @ ;"));
    loadSource(PSTR(": rot >r swap r> swap ;"));
    loadSource(PSTR(": min over over < if drop else nip then ;"));
    loadSource(PSTR(": max over over > if drop else nip then ;"));
    loadSource(PSTR(": between rot dup >r min max r> = ;"));
    loadSource(PSTR(": cr #13 emit #10 emit ;"));
    loadSource(PSTR(": space $20 emit ; inline"));
    loadSource(PSTR(": tab #9 emit ; inline"));
    loadSource(PSTR(": count dup 1+ swap c@ ;"));
    loadSource(PSTR(": mc@ mw@ $ff and ;"));
    loadSource(PSTR(": type  if- over + do i c@  emit loop else drop drop then ;"));
    loadSource(PSTR(": mtype if- over + do i mc@ emit loop else drop drop then ;"));
    loadSource(PSTR(": . 0 num>str space type ;"));
    loadSource(PSTR(": .2 2 num>str type ; : .4 4 num>str type ;"));
    loadSource(PSTR(": hex $10 base ! ; : decimal #10 base ! ; : binary 2 base ! ;"));
    loadSource(PSTR(": allot here + (h) ! ;"));
    loadSource(PSTR(": -fl- last  ! last a@ (l) ! ;"));
    loadSource(PSTR("// : [ 0 state ! ; immediate"));
    loadSource(PSTR("// : ] 1 state ! ;"));
    loadSource(PSTR(": words 0 >r last begin"
        " 1- dup 24 * dict + 3 + dup mc@ #31 and swap 1+ swap mtype"
        " r> 1 + >r r@ 10 mod 0 = if cr else #9 emit then"
        " while- drop r> drop ;"));
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

    loadSource(PSTR(": mc@ mw@ $FF and ;"));
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
