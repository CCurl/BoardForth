#include "board.h"
#ifndef __DEV_BOARD__
#include <windows.h>
#endif

#include "defs.h"
#pragma warning(disable: 4996)

CELL *dstk;
CELL *rstk;

BYTE dict[DICT_SZ];
SYSVARS_T* sys;
CELL loopSTK[12];
CELL loopDepth;
ALLOC_T allocTbl[ALLOC_SZ];
int num_alloced = 0;
CELL allocAddrBase = 0, allocCurFree = 0;

void run(CELL PC, CELL max_cycles) {
    CELL t1, t2, t3;
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
        case OP_TYPE:     // type (#41)
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
            t3 = pop();
            t2 = pop();
            t1 = pop();
            while (t3 > 0) {
                dict[t2++] = dict[t1++];
                t3--;
            }
            break;
        case OP_CMOVE2:     // cmove> (#48)
            t3 = pop();
            t2 = pop();
            t1 = pop();

            t2 += (t3 - 1);
            t1 += (t3 - 1);

            while (t3 > 0) {
                dict[t2--] = dict[t1--];
                t3--;
            }
            break;
        case OP_FILL:     // fill (#49)
            t3 = pop();
            t2 = pop();
            t1 = pop();
            while (t2 > 0) {
                dict[t1++] = (BYTE)t3;
                t2--;
            }
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
            if (loopDepth > 0) {
                int x = (loopDepth - 1) * 3;
                CELL f = loopSTK[x];
                CELL t = loopSTK[x + 1];
                loopSTK[x + 2] += pop();
                CELL i = loopSTK[x + 2];
                // printStringF("-LOOP(%ld,%ld,%ld)-", f, t, i);
                if ((f < i) && (i < t)) { push(1); return; }
                if ((t < i) && (i < f)) { push(1); return; }
                loopDepth -= 1;
                push(0);
            }
            else {
                printString("-LOOP:depthErr-");
                push(0);
            }
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
                DICT_T* dp = (DICT_T*)&dict[T];
                T += ADDR_SZ + dp->len + 3;
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
        case OP_J:     // j (#74)
            if (loopDepth > 1) {
                t1 = (loopDepth - 2) * 3;
                push(loopSTK[t1 + 2]);
            }
            else {
                printString("-J:depthErr-");
                push(0);
            }

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
    allocAddrBase = DICT_SZ;
    allocCurFree = DICT_SZ;
    sys->DSTACK = allocSpace(CELL_SZ * STK_SZ);
    sys->RSTACK = allocSpace(CELL_SZ * STK_SZ);
    sys->TIB = allocSpace(TIB_SZ);
    allocAddrBase = allocCurFree;
    allocFreeAll();
    dstk = (CELL*)&dict[sys->DSTACK];
    rstk = (CELL*)&dict[sys->RSTACK];
    loopDepth = 0;
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
    CELL *x = (CELL *)&dict[loc];
    return *x;
#endif
}

CELL wordAt(CELL loc) {
#ifdef __NEEDS_ALIGN__
    return (dict[loc + 1] << 8) | dict[loc];
#else
    WORD *x = (WORD *)&dict[loc];
    return (CELL)(*x);
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

#pragma region allocation
void allocDump() {
    printStringF("\r\nAlloc table (sz %d, %d used)", ALLOC_SZ, num_alloced);
    printString("\r\n-------------------------------");
    for (int i = 0; i < num_alloced; i++) {
        printStringF("\r\n%2d %04lx %4d %s", i, allocTbl[i].addr, (int)allocTbl[i].sz, (int)allocTbl[i].available ? "available" : "in-use");
    }
}

int allocFind(CELL addr) {
    for (int i = 0; i < num_alloced; i++) {
        if (allocTbl[i].addr == addr) return i;
    }
    return -1;
}

void allocFree(CELL addr) {
    // printStringF("-allocFree:%d-", (int)addr);
    int x = allocFind(addr);
    if (x >= 0) {
        // printStringF("-found:%d-", (int)x);
        allocTbl[x].available = 1;
        if ((x + 1) == num_alloced) { --num_alloced; }
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

CELL allocSpace(int sz) {
    int x = allocFindAvailable(sz);
    if (x >= 0) {
        // printStringF("-alloc:reuse:%d-", x);
        allocTbl[x].available = 0;
        return allocTbl[x].addr;
    }
    // printStringF("-alloc:%d,%d-\r\n", (int)sz, (int)allocCurFree);
    allocCurFree -= (sz);
    if (allocCurFree <= sys->HERE) {
        printString("-out of space!-");
        allocCurFree += sz;
        return 0;
    }
    if (num_alloced < ALLOC_SZ) {
        allocTbl[num_alloced].addr = allocCurFree;
        allocTbl[num_alloced].sz = sz;
        allocTbl[num_alloced++].available = 0;
    }
    else {
        printString("-allocTbl too small-");
    }
    return allocCurFree;
}
#pragma endregion

void fDUMPDICT() {
    char x[20];
    int n = 0;
    FILE* to = (FILE*)pop();
    to = to ? to : stdout;
    fprintf(to, "%04x %04x (%ld %ld)", (uint)sys->HERE, (uint)sys->LAST, sys->HERE, sys->LAST);
    for (int i = 0; i < sys->HERE; i++) {
        if (i % 16 == 0) {
            if (n) { x[n] = 0; fprintf(to, " ; %s", x); }
            fprintf(to, "\r\n %04x:", i);
            n = 0;
        }
        BYTE b = dict[i];
        x[n++] = ((31 < b) && (b < 128)) ? b : '.';
        fprintf(to, " %02x", dict[i]);
    }
    if (n) { x[n] = 0; fprintf(to, " ; %s", x); }
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
    printStringF("%c", (char)pop());
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
        push('('); fEMIT();
        for (int i = 1; i <= sys->DSP; i++) {
            push(' '); fEMIT();
            push(dstk[i]);
            push(0);
            fNUM2STR();
            fTYPE();
        }
        push(' '); fEMIT();
        push(')'); fEMIT();
    }
    else {
        printStringF("()");
    }
}
void fPARSEWORD() {    // opcode #59
    CELL wa = pop();
    char* w = (char*)&dict[wa];
    // printStringF("-pw[%s]-", w);
    push(wa); fFIND();
    if (pop()) {
        DICT_T* dp = (DICT_T*)&dict[T];

        runOpcode(OP_GETXT);
        CELL xt = pop();
        if (compiling(w, 0)) {
            if (dp->flags == 1) {
                // 1 => IMMEDIATE
                run(xt, 0);
            }
            else if (dp->flags == 2) {
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
    CELL buf = allocSpace(32);
    char* w = (char*)&dict[buf];
    push(buf);
    fNEXTWORD();
    while (pop()) {
        // printStringF("-pw:%s-", w);
        if (strcmp(w, "//") == 0) { break; }
        if (strcmp(w, "\\") == 0) { break; }
        push(buf);
        fPARSEWORD();
        push(buf);
        fNEXTWORD();
    }
    allocFree(buf);
}
void fCREATE() {       // opcode #64
    CELL wa = pop();
    char* name = (char*)&dict[wa];
#ifdef __NEEDS_ALIGN__
    push(sys->HERE);
    runOpcode(OP_ALIGN2);
    sys->HERE = pop();
#endif
    // printStringF("-define [%s] at %d (%lx)", name, sys->HERE, sys->HERE);

    DICT_T* dp = (DICT_T*)&dict[sys->HERE];
    dp->prev = (ADDR)sys->LAST;
    dp->flags = 0;
    dp->len = (BYTE)strlen(name);
    strcpy((char*)dp->name, name);
    sys->LAST = sys->HERE;
    sys->HERE += ADDR_SZ + dp->len + 3;
    // printStringF(",XT:%d (%lx)-", sys->HERE, sys->HERE);
}
//// (a1 -- [a2 1] | 0)
void fFIND() {         // opcode #65
    char* name = (char*)&dict[pop()];
    // printStringF("-lf:[%s]-", name);
    CELL cl = sys->LAST;
    while (cl) {
        DICT_T* dp = (DICT_T*)&dict[cl];
        if (strcmp(name, (char*)dp->name) == 0) {
            // printStringF("-FOUND! (%lx)-", cl);
            push(cl);
            push(1);
            return;
        }
        cl = (CELL)dp->prev;
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
    CELL num = pop();
    BYTE len = 0;
    int isNeg = (num < 0);
    CELL pad = allocSpace(48);
    CELL cp = pad + 47;
    dict[cp--] = (BYTE)0;
    num = (isNeg) ? -num : num;

    do {
        CELL r = (num % sys->BASE) + '0';
        if ('9' < r) { r += 7; }
        dict[cp--] = (BYTE)r;
        len++;
        num /= sys->BASE;
    } while (num > 0);

    while (len < reqLen) {
        dict[cp--] = '0';
        ++len;
    }
    dict[cp] = len;
    push(cp + 1);
    push(len);
    allocFree(pad);
}

/* -- NimbleText script:
$once
// vvvvv - NimbleText generated - vvvvv
void loadBaseSystem() {
$each
    parseLine_P(F("$row"));
$once
}
// ^^^^^ - NimbleText generated - ^^^^^
*/

// vvvvv - NimbleText generated - vvvvv
void loadBaseSystem() {
    loadSource(PSTR(": cell 4 ;           : addr 2 ;"));
    loadSource(PSTR("// : tib    #8 @ ;      : >in   #12 ;"));
    loadSource(PSTR(": (h) #16 ;          : here (h) @ ;"));
    loadSource(PSTR(": (l) #20 ;          : last (l) @ ;"));
    loadSource(PSTR(": base  #24 ;        : state #28 ;"));
    loadSource(PSTR(": sp0   #32 @ ;      // : rp0   #36 @ ;"));
    loadSource(PSTR(": (dsp) #40 ;        : dsp (dsp) @ ;"));
    loadSource(PSTR("// : (rsp) #44 ;        : rsp (rsp) @ ;"));
    loadSource(PSTR(": !sp 0 (dsp) ! ;    // : !rsp 0 (rsp) ! ;"));
    loadSource(PSTR("// : cells 4 * ;        // : cell+ 4 + ;"));
    loadSource(PSTR(": inline    2 last addr + c! ;"));
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
    loadSource(PSTR(": count dup 1+ swap c@ ;"));
    loadSource(PSTR(": . 0 num>str space type ;"));
    loadSource(PSTR(": .2 2 num>str type ; : .4 4 num>str type ;"));
    loadSource(PSTR(": hex $10 base ! ; : decimal #10 base ! ; : binary 2 base ! ;"));
    loadSource(PSTR(": allot here + (h) ! ;"));
    loadSource(PSTR("// : [ 0 state ! ; immediate"));
    loadSource(PSTR("// : ] 1 state ! ;"));
    loadSource(PSTR(": .wordl dup .4 space dup >body .4 addr + dup c@ . 1+ space count type cr ;"));
    loadSource(PSTR(": .word addr + 1+ count type space ;"));
    loadSource(PSTR(": wordsl last begin dup .wordl a@ while- drop ;"));
    loadSource(PSTR(": words  last begin dup .word  a@ while- drop ;"));
}
// ^^^^^ - NimbleText generated - ^^^^^


/* -- NimbleText script:
$once
// vvvvv - NimbleText generated - vvvvv
BYTE getOpcode(char *w) {
$each
    if (strcmp_PF(w, PSTR("$1")) == 0) return OP_$0;       //  opcode #$rownum
$once
    return 0xFF;
}
// ^^^^^ - NimbleText generated - ^^^^^
*/

// vvvvv - NimbleText generated - vvvvv
BYTE getOpcode(char* w) {
    if (strcmp_PF(w, PSTR("noop")) == 0) return OP_NOOP;       //  opcode #0
    if (strcmp_PF(w, PSTR("cliteral")) == 0) return OP_CLIT;       //  opcode #1
    if (strcmp_PF(w, PSTR("wliteral")) == 0) return OP_WLIT;       //  opcode #2
    if (strcmp_PF(w, PSTR("literal")) == 0) return OP_LIT;       //  opcode #3
    if (strcmp_PF(w, PSTR("c@")) == 0) return OP_CFETCH;       //  opcode #4
    if (strcmp_PF(w, PSTR("w@")) == 0) return OP_WFETCH;       //  opcode #5
    if (strcmp_PF(w, PSTR("a@")) == 0) return OP_AFETCH;       //  opcode #6
    if (strcmp_PF(w, PSTR("@")) == 0) return OP_FETCH;       //  opcode #7
    if (strcmp_PF(w, PSTR("c!")) == 0) return OP_CSTORE;       //  opcode #8
    if (strcmp_PF(w, PSTR("w!")) == 0) return OP_WSTORE;       //  opcode #9
    if (strcmp_PF(w, PSTR("a!")) == 0) return OP_ASTORE;       //  opcode #10
    if (strcmp_PF(w, PSTR("!")) == 0) return OP_STORE;       //  opcode #11
    if (strcmp_PF(w, PSTR("c,")) == 0) return OP_CCOMMA;       //  opcode #12
    if (strcmp_PF(w, PSTR("w,")) == 0) return OP_WCOMMA;       //  opcode #13
    if (strcmp_PF(w, PSTR(",")) == 0) return OP_COMMA;       //  opcode #14
    if (strcmp_PF(w, PSTR("a,")) == 0) return OP_ACOMMA;       //  opcode #15
    if (strcmp_PF(w, PSTR("call")) == 0) return OP_CALL;       //  opcode #16
    if (strcmp_PF(w, PSTR("exit")) == 0) return OP_RET;       //  opcode #17
    if (strcmp_PF(w, PSTR("-n-")) == 0) return OP_JMP;       //  opcode #18
    if (strcmp_PF(w, PSTR("-n-")) == 0) return OP_JMPZ;       //  opcode #19
    if (strcmp_PF(w, PSTR("-n-")) == 0) return OP_JMPNZ;       //  opcode #20
    if (strcmp_PF(w, PSTR("1-")) == 0) return OP_ONEMINUS;       //  opcode #21
    if (strcmp_PF(w, PSTR("1+")) == 0) return OP_ONEPLUS;       //  opcode #22
    if (strcmp_PF(w, PSTR("dup")) == 0) return OP_DUP;       //  opcode #23
    if (strcmp_PF(w, PSTR("swap")) == 0) return OP_SWAP;       //  opcode #24
    if (strcmp_PF(w, PSTR("drop")) == 0) return OP_DROP;       //  opcode #25
    if (strcmp_PF(w, PSTR("over")) == 0) return OP_OVER;       //  opcode #26
    if (strcmp_PF(w, PSTR("+")) == 0) return OP_ADD;       //  opcode #27
    if (strcmp_PF(w, PSTR("-")) == 0) return OP_SUB;       //  opcode #28
    if (strcmp_PF(w, PSTR("*")) == 0) return OP_MULT;       //  opcode #29
    if (strcmp_PF(w, PSTR("/mod")) == 0) return OP_SLMOD;       //  opcode #30
    if (strcmp_PF(w, PSTR("2*")) == 0) return OP_LSHIFT;       //  opcode #31
    if (strcmp_PF(w, PSTR("2/")) == 0) return OP_RSHIFT;       //  opcode #32
    if (strcmp_PF(w, PSTR("and")) == 0) return OP_AND;       //  opcode #33
    if (strcmp_PF(w, PSTR("or")) == 0) return OP_OR;       //  opcode #34
    if (strcmp_PF(w, PSTR("xor")) == 0) return OP_XOR;       //  opcode #35
    if (strcmp_PF(w, PSTR("not")) == 0) return OP_NOT;       //  opcode #36
    if (strcmp_PF(w, PSTR(">r")) == 0) return OP_DTOR;       //  opcode #37
    if (strcmp_PF(w, PSTR("r@")) == 0) return OP_RFETCH;       //  opcode #38
    if (strcmp_PF(w, PSTR("r>")) == 0) return OP_RTOD;       //  opcode #39
    if (strcmp_PF(w, PSTR("emit")) == 0) return OP_EMIT;       //  opcode #40
    if (strcmp_PF(w, PSTR("type")) == 0) return OP_TYPE;       //  opcode #41
    if (strcmp_PF(w, PSTR(".s")) == 0) return OP_DOTS;       //  opcode #42
    if (strcmp_PF(w, PSTR("s\"")) == 0) return OP_SQUOTE;       //  opcode #43
    if (strcmp_PF(w, PSTR("(")) == 0) return OP_PAREN;       //  opcode #44
    if (strcmp_PF(w, PSTR("wdtfeed")) == 0) return OP_WDTFEED;       //  opcode #45
    if (strcmp_PF(w, PSTR("brk")) == 0) return OP_BREAK;       //  opcode #46
    if (strcmp_PF(w, PSTR("cmove")) == 0) return OP_CMOVE;       //  opcode #47
    if (strcmp_PF(w, PSTR("cmove>")) == 0) return OP_CMOVE2;       //  opcode #48
    if (strcmp_PF(w, PSTR("fill")) == 0) return OP_FILL;       //  opcode #49
    if (strcmp_PF(w, PSTR("open-block")) == 0) return OP_OPENBLOCK;       //  opcode #50
    if (strcmp_PF(w, PSTR("file-close")) == 0) return OP_FILECLOSE;       //  opcode #51
    if (strcmp_PF(w, PSTR("file-read")) == 0) return OP_FILEREAD;       //  opcode #52
    if (strcmp_PF(w, PSTR("load")) == 0) return OP_LOAD;       //  opcode #53
    if (strcmp_PF(w, PSTR("thru")) == 0) return OP_THRU;       //  opcode #54
    if (strcmp_PF(w, PSTR("do")) == 0) return OP_DO;       //  opcode #55
    if (strcmp_PF(w, PSTR("loop")) == 0) return OP_LOOP;       //  opcode #56
    if (strcmp_PF(w, PSTR("loop+")) == 0) return OP_LOOPP;       //  opcode #57
    if (strcmp_PF(w, PSTR("debugger")) == 0) return OP_DEBUGGER;       //  opcode #58
    if (strcmp_PF(w, PSTR("parse-word")) == 0) return OP_PARSEWORD;       //  opcode #59
    if (strcmp_PF(w, PSTR("parse-line")) == 0) return OP_PARSELINE;       //  opcode #60
    if (strcmp_PF(w, PSTR(">body")) == 0) return OP_GETXT;       //  opcode #61
    if (strcmp_PF(w, PSTR("align2")) == 0) return OP_ALIGN2;       //  opcode #62
    if (strcmp_PF(w, PSTR("align4")) == 0) return OP_ALIGN4;       //  opcode #63
    if (strcmp_PF(w, PSTR("create")) == 0) return OP_CREATE;       //  opcode #64
    if (strcmp_PF(w, PSTR("find")) == 0) return OP_FIND;       //  opcode #65
    if (strcmp_PF(w, PSTR("next-word")) == 0) return OP_NEXTWORD;       //  opcode #66
    if (strcmp_PF(w, PSTR("number?")) == 0) return OP_ISNUMBER;       //  opcode #67
    if (strcmp_PF(w, PSTR("-n-")) == 0) return OP_NJMPZ;       //  opcode #68
    if (strcmp_PF(w, PSTR("-n-")) == 0) return OP_NJMPNZ;       //  opcode #69
    if (strcmp_PF(w, PSTR("<")) == 0) return OP_LESS;       //  opcode #70
    if (strcmp_PF(w, PSTR("=")) == 0) return OP_EQUALS;       //  opcode #71
    if (strcmp_PF(w, PSTR(">")) == 0) return OP_GREATER;       //  opcode #72
    if (strcmp_PF(w, PSTR("i")) == 0) return OP_I;       //  opcode #73
    if (strcmp_PF(w, PSTR("j")) == 0) return OP_J;       //  opcode #74
    if (strcmp_PF(w, PSTR("input")) == 0) return OP_INPUTPIN;       //  opcode #75
    if (strcmp_PF(w, PSTR("output")) == 0) return OP_OUTPUTPIN;       //  opcode #76
    if (strcmp_PF(w, PSTR("ms")) == 0) return OP_DELAY;       //  opcode #77
    if (strcmp_PF(w, PSTR("tick")) == 0) return OP_TICK;       //  opcode #78
    if (strcmp_PF(w, PSTR("ap!")) == 0) return OP_APINSTORE;       //  opcode #79
    if (strcmp_PF(w, PSTR("dp!")) == 0) return OP_DPINSTORE;       //  opcode #80
    if (strcmp_PF(w, PSTR("ap@")) == 0) return OP_APINFETCH;       //  opcode #81
    if (strcmp_PF(w, PSTR("dp@")) == 0) return OP_DPINFETCH;       //  opcode #82
    if (strcmp_PF(w, PSTR("mw@")) == 0) return OP_MWFETCH;       //  opcode #83
    if (strcmp_PF(w, PSTR("mc!")) == 0) return OP_MCSTORE;       //  opcode #84
    if (strcmp_PF(w, PSTR("num>str")) == 0) return OP_NUM2STR;       //  opcode #85
    if (strcmp_PF(w, PSTR("bye")) == 0) return OP_BYE;       //  opcode #86
    return 0xFF;
}
// ^^^^^ - NimbleText generated - ^^^^^

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
    if (to == 0) to = allocSpace((int)strlen(s) + 2);
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

void loadUserWords() {
    char* buf = (char*)&dict[sys->HERE + 256];
    sprintf(buf, ": d-start $%lx ;", (ulong)&dict[0]);
    printStringF("%s\r\n", buf);
    parseLine(buf);
    sprintf(buf, ": d-size #%lu ;", (ulong)DICT_SZ);
    printStringF("%s\r\n", buf);
    parseLine(buf);
    // sprintf(buf, ": dpin-base #%ld ; : apin-base #%ld ;", (long)0, (long)A0);
    // parseLine(buf);
    loadSource(PSTR(": mc@ dup 1+ mw@ $FF and ;"));
    loadSource(PSTR(": m@  dup 1+ 1+ mw@ $10000 * swap mw@ or ;"));
    loadSource(PSTR(": mw! over   $100 / over 1+ mc! mc! ;"));
    loadSource(PSTR(": m!  over $10000 / over 1+ 1+ mw! mw! ;"));
    loadSource(PSTR(": auto-run-last last >body 0 a! ;"));
    loadSource(PSTR(": auto-run-off 0 0 a! ;"));

    loadSource(PSTR(": elapsed tick swap - 1000 /mod . . ;"));
    loadSource(PSTR(": bm tick swap begin 1- while- drop elapsed ;"));
    loadSource(PSTR(": low->high over over > if swap then ;"));
    loadSource(PSTR(": high->low over over < if swap then ;"));
    loadSource(PSTR(": dump low->high do i c@ . loop ;"));
    loadSource(PSTR(": led 22 ; led output"));
    loadSource(PSTR(": led-on 1 led dp! ; : led-off 0 led dp! ;"));
    loadSource(PSTR(": blink led-on dup ms led-off dup ms ;"));
    loadSource(PSTR(": k 1000 * ; : mil k k ;"));
    loadSource(PSTR(": blinks 0 swap do blink loop ;"));
    loadSource(PSTR("variable pot  3 pot ! "));
    loadSource(PSTR("variable (but)  6 (but) ! "));
    loadSource(PSTR("variable pot-lv variable sens 4 sens !"));
    loadSource(PSTR(": button (but) @ ;"));
    loadSource(PSTR(": pot@ pot @ ap@ ;"));
    loadSource(PSTR(": bp->led button dp@ led dp! ;"));
    loadSource(PSTR(": .pot? pot@ dup pot-lv @ - abs sens @ > if dup . cr pot-lv ! else drop then ;"));
    loadSource(PSTR(": go bp->led ;"));
    loadSource(PSTR("led output button input"));
    loadSource(PSTR("auto-run-last"));
    // loadSource(PSTR(""));
}

void dumpDict() {
    printStringF("%04x %04x (%ld %ld)", sys->HERE, sys->LAST, sys->HERE, sys->LAST);
    for (int i = 0; i < sys->HERE; i++) {
        if (i % 16 == 0) printStringF("\r\n %04x:", i);
        printStringF(" %02x", dict[i]);
    }
}

void ok() {
    printString(" ok. ");
    fDOTS();
    printString("\r\n");
}
