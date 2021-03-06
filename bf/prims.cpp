#include "board.h"
#ifndef __DEV_BOARD__
    #include <windows.h>
#endif

#include "defs.h"

CELL *dstk;
CELL *rstk;

BYTE IR;
CELL PC;
BYTE dict[DICT_SZ];
SYSVARS_T *sys;
CELL loopSTK[12];
CELL loopDepth;
ALLOC_T allocTbl[ALLOC_SZ];
int num_alloced = 0;
CELL allocAddrBase = 0, allocCurFree = 0;

void run(CELL start, CELL max_cycles) {
    PC = start;
    // printStringF("\nrun: %d (%04lx), %d cycles ... ", PC, PC, max_cycles);
    while (1) {
        BYTE IR = dict[PC++];
        if (IR == OP_RET) {
            if (sys->RSP < 1) { return; }
            PC = rpop();
        } else if (IR <= OP_BYE) {
            prims[IR]();
            if (IR == OP_BYE) { return; }
        } else {
            printStringF("-%04lx: unknown opcode: %d ($%02x)-", PC-1, IR, IR);
        }
        if (max_cycles) {
            if (--max_cycles < 1) { return; }
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
    sys->DSP = (sys->DSP < STK_SZ) ? sys->DSP+1 : STK_SZ;
    T = v;
}
CELL pop() {
    sys->DSP = (sys->DSP > 0) ? sys->DSP-1 : 0;
    return dstk[sys->DSP+1];
}

void rpush(CELL v) {
    sys->RSP = (sys->RSP < STK_SZ) ? sys->RSP+1 : STK_SZ;
    R = v; 
}
CELL rpop() {
    sys->RSP = (sys->RSP > 0) ? sys->RSP-1 : 0;
    return rstk[sys->RSP+1];
}

void vmInit() {
    sys = (SYSVARS_T *)dict;
    sys->HERE = ADDR_HERE_BASE;
    sys->LAST = 0;
    sys->BASE = 10;
    sys->STATE = 0;
    sys->DSP = 0;
    sys->RSP = 0;
    allocAddrBase = DICT_SZ;
    allocCurFree = DICT_SZ;
    sys->DSTACK = allocSpace(CELL_SZ*STK_SZ);
    sys->RSTACK = allocSpace(CELL_SZ*STK_SZ);
    sys->TIB = allocSpace(TIB_SZ);
    allocAddrBase = allocCurFree;
    allocFreeAll();
    dstk = (CELL *)&dict[sys->DSTACK];
    rstk = (CELL *)&dict[sys->RSTACK];
    loopDepth = 0;
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
    char buf[64];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    printString(buf);
}

CELL cellAt(CELL loc) {
    return (dict[loc+3] << 24) + (dict[loc+2] << 16) + (dict[loc+1] <<  8) + dict[loc];
}

CELL wordAt(CELL loc) {
    return (dict[loc+1] << 8) + dict[loc];
}

CELL addrAt(CELL loc) {         // opcode #16
    return (ADDR_SZ == 2) ? wordAt(loc) : cellAt(loc);
}

void wordStore(CELL addr, CELL val) {
    dict[addr]   = (val & 0xFF);
    dict[addr+1] = (val >>  8) & 0xFF;
}

void cellStore(CELL addr, CELL val) {
    dict[addr++] = ((val)       & 0xFF);
    dict[addr++] = ((val >>  8) & 0xFF);
    dict[addr++] = ((val >> 16) & 0xFF);
    dict[addr++] = ((val >> 24) & 0xFF);
}

void addrStore(CELL addr, CELL val) {
    (ADDR_SZ == 2) ? wordStore(addr, val) : cellStore(addr, val);
}

#pragma region allocation
void allocDump() {
    printStringF("\nAlloc table (sz %d, %d used)", ALLOC_SZ, num_alloced);
    printString("\n-------------------------------");
    for (int i = 0; i < num_alloced; i++) {
        printStringF("\n%2d %04lx %4d %s", i, allocTbl[i].addr, (int)allocTbl[i].sz, (int)allocTbl[i].available ? "available" : "in-use");
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

CELL allocSpace(WORD sz) {
    int x = allocFindAvailable(sz);
    if (x >= 0) {
        // printStringF("-alloc:reuse:%d-", x);
        allocTbl[x].available = 0;
        return allocTbl[x].addr;
    }
    // printStringF("-alloc:%d,%d-\n", (int)sz, (int)allocCurFree);
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
    } else {
        printString("-allocTbl too small-");
    }
    return allocCurFree;
}
#pragma endregion

void fDUMPDICT() {
    FILE *to = (FILE *)pop();
    to = to ? to : stdout;
    fprintf(to, "%04x %04x (%ld %ld)", sys->HERE, sys->LAST, sys->HERE, sys->LAST);
    for (int i = 0; i < sys->HERE; i++) {
        if (i % 16 == 0) fprintf(to, "\n %04x:", i);
        fprintf(to, " %02x", dict[i]);
    }
}

int compiling(char *w, int errIfNot) {
    if ((sys->STATE == 0) && (errIfNot)) {
        printStringF("[%s]: Compile only.", w);
    }
    return (sys->STATE == 0) ? 0 : 1;
}

int interpreting(char *w, int errIfNot) {
    if ((sys->STATE != 0) && (errIfNot)) {
        printStringF("[%s]: Interpreting only.", w);
    }
    return (sys->STATE == 0) ? 1 : 0;
}

/* NimbleText script
$once
// vvvvv - NimbleText generated - vvvvv
FP prims[] = {
$each
    f<%($0 + ',               ').substring(0,13)%>     // OP_$0 (#$rownum) ***$row***
$once
    0};
// ^^^^^ - NimbleText generated - ^^^^^
*/

// vvvvv - NimbleText generated - vvvvv
FP prims[] = {
    fNOOP,             // OP_NOOP (#0) ***NOOP NOOP***
    fCLIT,             // OP_CLIT (#1) ***CLIT CLITERAL ( -- N8 )***
    fWLIT,             // OP_WLIT (#2) ***WLIT WLITERAL ( -- N16 )***
    fLIT,              // OP_LIT (#3) ***LIT LITERAL ( -- N )***
    fCFETCH,           // OP_CFETCH (#4) ***CFETCH C@ (A -- N8)***
    fWFETCH,           // OP_WFETCH (#5) ***WFETCH W@ (A -- N16)***
    fAFETCH,           // OP_AFETCH (#6) ***AFETCH A@ (A -- N)***
    fFETCH,            // OP_FETCH (#7) ***FETCH @ (A -- N8)***
    fCSTORE,           // OP_CSTORE (#8) ***CSTORE C! ( N A -- )***
    fWSTORE,           // OP_WSTORE (#9) ***WSTORE W! ( N A -- )***
    fASTORE,           // OP_ASTORE (#10) ***ASTORE A! ( N A -- )***
    fSTORE,            // OP_STORE (#11) ***STORE ! (  N A -- )***
    fCCOMMA,           // OP_CCOMMA (#12) ***CCOMMA C, ( N -- )***
    fWCOMMA,           // OP_WCOMMA (#13) ***WCOMMA W, ( N -- )***
    fCOMMA,            // OP_COMMA (#14) ***COMMA , ( N -- )***
    fACOMMA,           // OP_ACOMMA (#15) ***ACOMMA A, ( N -- )***
    fCALL,             // OP_CALL (#16) ***CALL CALL ( -- )***
    fRET,              // OP_RET (#17) ***RET EXIT ( -- )***
    fJMP,              // OP_JMP (#18) ***JMP -N- ( -- ) ***
    fJMPZ,             // OP_JMPZ (#19) ***JMPZ -N- ( N -- )***
    fJMPNZ,            // OP_JMPNZ (#20) ***JMPNZ -N- ( N -- )***
    fONEMINUS,         // OP_ONEMINUS (#21) ***ONEMINUS 1- ( N -- N-1 )***
    fONEPLUS,          // OP_ONEPLUS (#22) ***ONEPLUS 1+ ( N -- N+1 )***
    fDUP,              // OP_DUP (#23) ***DUP DUP ( N -- N N )***
    fSWAP,             // OP_SWAP (#24) ***SWAP SWAP ( N1 N2 -- N2 N1 )***
    fDROP,             // OP_DROP (#25) ***DROP DROP (N -- )***
    fOVER,             // OP_OVER (#26) ***OVER OVER ( N1 N2 -- N1 N2 N1 )***
    fADD,              // OP_ADD (#27) ***ADD + ( N1 N2 -- N3 )***
    fSUB,              // OP_SUB (#28) ***SUB - ( N1 N2 -- N3 )***
    fMULT,             // OP_MULT (#29) ***MULT * ( N1 N2 -- N3 )***
    fSLMOD,            // OP_SLMOD (#30) ***SLMOD /MOD ( N1 N2 -- N3 N4 )***
    fLSHIFT,           // OP_LSHIFT (#31) ***LSHIFT << ( N -- N*2 )***
    fRSHIFT,           // OP_RSHIFT (#32) ***RSHIFT >> ( N -- N/2 )***
    fAND,              // OP_AND (#33) ***AND AND  ( N1 N2 -- N3 )***
    fOR,               // OP_OR (#34) ***OR OR ( N1 N2 -- N3 )***
    fXOR,              // OP_XOR (#35) ***XOR XOR ( N1 N2 -- N3 )***
    fNOT,              // OP_NOT (#36) ***NOT NOT ( N1 -- N2 )***
    fDTOR,             // OP_DTOR (#37) ***DTOR >R ( N -- )***
    fRFETCH,           // OP_RFETCH (#38) ***RFETCH R@ (-- N )***
    fRTOD,             // OP_RTOD (#39) ***RTOD R> ( -- N )***
    fEMIT,             // OP_EMIT (#40) ***EMIT EMIT (N -- )***
    fTYPE,             // OP_TYPE (#41) ***TYPE type (a n -- )***
    fDOTS,             // OP_DOTS (#42) ***DOTS .S ( -- )***
    fDOTQUOTE,         // OP_DOTQUOTE (#43) ***DOTQUOTE .\" ( -- )***
    fPAREN,            // OP_PAREN (#44) ***PAREN ( ( -- )***
    fWDTFEED,          // OP_WDTFEED (#45) ***WDTFEED WDTFEED ( -- )***
    fBREAK,            // OP_BREAK (#46) ***BREAK BRK ( -- )***
    fCMOVE,            // OP_CMOVE (#47) ***CMOVE CMOVE ( a1 a2 u -- )***
    fCMOVE2,           // OP_CMOVE2 (#48) ***CMOVE2 CMOVE> ( a1 a2 u -- )***
    fFILL,             // OP_FILL (#49) ***FILL FILL ( a u n -- )***
    fOPENBLOCK,        // OP_OPENBLOCK (#50) ***OPENBLOCK OPEN-BLOCK***
    fFILECLOSE,        // OP_FILECLOSE (#51) ***FILECLOSE FILE-CLOSE***
    fFILEREAD,         // OP_FILEREAD (#52) ***FILEREAD FILE-READ***
    fLOAD,             // OP_LOAD (#53) ***LOAD LOAD***
    fTHRU,             // OP_THRU (#54) ***THRU THRU***
    fDO,               // OP_DO (#55) ***DO DO ( f t -- )***
    fLOOP,             // OP_LOOP (#56) ***LOOP LOOP ( -- )***
    fLOOPP,            // OP_LOOPP (#57) ***LOOPP LOOP+  ( n -- )***
    fUNUSED7,          // OP_UNUSED7 (#58) ***UNUSED7 -n- ( -- )***
    fPARSEWORD,        // OP_PARSEWORD (#59) ***PARSEWORD PARSE-WORD ( A -- )***
    fPARSELINE,        // OP_PARSELINE (#60) ***PARSELINE PARSE-LINE (A -- )***
    fGETXT,            // OP_GETXT (#61) ***GETXT >BODY ( A1 -- A2 )***
    fALIGN2,           // OP_ALIGN2 (#62) ***ALIGN2 ALIGN2 ( N1 -- N2 )***
    fALIGN4,           // OP_ALIGN4 (#63) ***ALIGN4 ALIGN4 ( N1 -- N2 )***
    fCREATE,           // OP_CREATE (#64) ***CREATE CREATE ( A -- )***
    fFIND,             // OP_FIND (#65) ***FIND FIND ( A1 -- (A1 1)|0 )***
    fNEXTWORD,         // OP_NEXTWORD (#66) ***NEXTWORD NEXT-WORD ( A -- )***
    fISNUMBER,         // OP_ISNUMBER (#67) ***ISNUMBER NUMBER? ( A -- (N 1)|0 )***
    fNJMPZ,            // OP_NJMPZ (#68) ***NJMPZ -N- ( N -- N )***
    fNJMPNZ,           // OP_NJMPNZ (#69) ***NJMPNZ -N- ( N -- N )***
    fLESS,             // OP_LESS (#70) ***LESS < ( N1 N2 -- N3 )***
    fEQUALS,           // OP_EQUALS (#71) ***EQUALS = ( N1 N2 -- N3 )***
    fGREATER,          // OP_GREATER (#72) ***GREATER > ( N1 N2 -- N3 )***
    fI,                // OP_I (#73) ***I I ( -- n )***
    fJ,                // OP_J (#74) ***J J ( -- n )***
    fINPUTPIN,         // OP_INPUTPIN (#75) ***INPUTPIN input ( n -- )***
    fOUTPUTPIN,        // OP_OUTPUTPIN (#76) ***OUTPUTPIN output ( n -- )***
    fDELAY,            // OP_DELAY (#77) ***DELAY MS ( n -- )***
    fTICK,             // OP_TICK (#78) ***DELAY MS ( n -- )***
    fAPINSTORE,        // OP_APINSTORE (#79) ***APINSTORE  ap! ( n1 n2 -- )***
    fDPINSTORE,        // OP_DPINSTORE (#80) ***DPINSTORE dp! ( n1 n2 -- )***
    fAPINFETCH,        // OP_APINFETCH (#81) ***APINFETCH ap@ ( n1 -- n2 )***
    fDPINFETCH,        // OP_DPINFETCH (#82) ***DPINFETCH dp@ ( n1 -- n2 )***
    fMWFETCH,          // OP_MWFETCH (#83) ***MWFETCH mw@ ( n1 -- n2 )***
    fMCSTORE,          // OP_MCSTORE (#84) ***MCSTORE mc! ( n1 n2 -- )***
    fNUM2STR,          // OP_NUM2STR (#85) ***NUM2STR num>str ( n l -- a )***
    fBYE,              // OP_BYE (#86) ***BYE bye ( -- )***
    0};
// ^^^^^ - NimbleText generated - ^^^^^


void fNOOP() {         // opcode #0
}
void fCLIT() {         // opcode #1
    push(dict[PC++]);
}
void fWLIT() {         // opcode #2
    push(wordAt(PC));
    PC += WORD_SZ;
}
void fLIT() {          // opcode #3
    push(cellAt(PC));
    PC += CELL_SZ;
}
void fCFETCH() {       // opcode #4
    CELL addr = T;
    if ((0 <= addr) && (addr < DICT_SZ)) {
        T = dict[addr];
        return;
    }
    printStringF("Invalid address: %ld ($%04lX)", addr, addr);
}
void fWFETCH() {       // opcode #5
    CELL addr = T;
    if ((0 <= addr) && ((addr+2) < DICT_SZ)) {
        T = wordAt(addr);
        return;
    }
    printStringF("Invalid address: %ld ($%04lX)", addr, addr);
}
void fAFETCH() {       // opcode #6
    (ADDR_SZ == 2) ? fWFETCH() : fFETCH();
}
void fFETCH() {        // opcode #7
    CELL addr = T;
    if ((0 <= addr) && ((addr+4) < DICT_SZ)) {
        T = cellAt(addr);
        return;
    }
    printStringF("Invalid address: %ld ($%04lX)", addr, addr);
}
void fCSTORE() {       // opcode #8
    CELL addr = pop();
    CELL val = pop();
    if ((0 <= addr) && (addr < DICT_SZ)) {
        dict[addr++] = (val & 0xFF);
    }
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
    if ((0 <= addr) && ((addr+4) < DICT_SZ)) {
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
void fCALL() {         // opcode #16
    rpush(PC+ADDR_SZ);
    PC = addrAt(PC);
    // printStringF("-call:%lx-", PC);
}
void fRET() {          // opcode #17
    // handled in run()
}
void fJMP() {          // opcode #18
    PC = addrAt(PC);
}
void fJMPZ() {         // opcode #19
    if (pop() == 0) PC = addrAt(PC);
    else PC += ADDR_SZ;
}
void fJMPNZ() {        // opcode #20
    if (pop()) PC = addrAt(PC);
    else PC += ADDR_SZ;
}
void fONEMINUS() {     // opcode #21
    T -= 1;
}
void fONEPLUS() {      // opcode #22
    T += 1;
}
void fDUP() {          // opcode #23
    push(T);
}
void fSWAP() {         // opcode #24
    CELL t = T; T = N; N = t;
}
void fDROP() {         // opcode #25
    pop();
}
void fOVER() {         // opcode #26
    push(N);
}
void fADD() {          // opcode #27
    N += T;pop();
}
void fSUB() {          // opcode #28
    N -= T; pop();
}
void fMULT() {         // opcode #29
    N *= T; pop();
}
void fSLMOD() {        // opcode #30
    CELL x = N, y = T;
    if (y) {
        T = x/y;
        N = x%y;
    } else {
        printString("divide by 0!");
    }
}
void fLSHIFT() {       // opcode #31
    T = T << 1;
}
void fRSHIFT() {       // opcode #32
    T = T >> 1;
}
void fAND() {          // opcode #33
    N &= T; pop();
}
void fOR() {           // opcode #34
    N |= T; pop();
}
void fXOR() {          // opcode #35
    N ^= T; pop();
}
void fNOT() {          // opcode #36
    T = ~T;
}
void fDTOR() {         // opcode #37
    rpush(pop());
}
void fRFETCH() {       // opcode #38
    push(R);
}
void fRTOD() {         // opcode #39
    push(rpop());
}
void fEMIT() {
    printStringF("%c", (char)pop());
}
void fTYPE() {
    CELL n = pop();
    CELL a = pop();
    char x[2];
    x[1] = 0;
    for (int i = 0; i < n; i++ ) {
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
    } else {
        printStringF("()");
    }
}
void fDOTQUOTE() {     // opcode #43
    N = N*T; push(T); pop();
}
void fPAREN() {        // opcode #44
    N = N*T; push(T); pop();
}
void fWDTFEED() {      // opcode #45
    N = N*T; push(T); pop();
}
void fBREAK() {        // opcode #46
    N = N*T; push(T); pop();
}
// OP_CMOVE (#47)    : CMOVE ( TODO -- TODO ) ... ;
void fCMOVE() {    
    CELL num = pop();
    CELL to = pop();
    CELL from = pop();
    while (num > 0) {
        dict[to++] = dict[from++];
        num--;
    }
}
// OP_CMOVE2 (#48)    : CMOVE> ( TODO -- TODO ) ... ;
void fCMOVE2() {   
    CELL num = pop();
    CELL to = pop();
    CELL from = pop();

    to += (num-1);
    from += (num-1);

    while (num > 0) {
        dict[to--] = dict[from--];
        num--;
    }
}
// OP_FILL (#49)    : FILL ( TODO -- TODO ) ... ;
void fFILL() {    
    CELL val = pop();
    CELL num = pop();
    CELL to = pop();
    while (num > 0) {
        dict[to++] = val;
        num--;
    }
}
void fOPENBLOCK() {    // opcode #50
    N = N*T; push(T); pop();
}
void fFILECLOSE() {    // opcode #51
    N = N*T; push(T); pop();
}
void fFILEREAD() {     // opcode #52
    N = N*T; push(T); pop();
}
void fLOAD() {         // opcode #53
    N = N*T; push(T); pop();
}
void fTHRU() {         // opcode #54
    N = N*T; push(T); pop();
}
// OP_DO (#55)    : DO ( f t -- ) ... ;
void fDO() {       
    if (loopDepth < 4) {
        CELL t = pop();
        CELL f = pop();
        int x = loopDepth * 3;
        // printStringF("-DO(%ld,%ld,%d)-", f, t, f);
        loopSTK[x] = f;
        loopSTK[x+1] = t;
        loopSTK[x+2] = f;
        ++loopDepth;
    } else {
        printString("-DO:too deep-");
    }
}
// OP_LOOP (#56)    : LOOP ( -- ) ... ;
void fLOOP() {     
    if (loopDepth > 0) {
        int x = (loopDepth-1) * 3;
        CELL f = loopSTK[x];
        CELL t = loopSTK[x+1];
        loopSTK[x+2] += 1;
        CELL i = loopSTK[x+2];
        // printString("-LOOP(%ld,%ld,%d)-", f, t, i);
        if ((f < i) && (i < t)) { push(1); return; }
        loopDepth -= 1;
        push(0);
    }
    else {
        printString("-LOOP:depthErr-");
        push(0);
    }
}
// OP_LOOPP (#57)    : LOOP+ ( n -- ) ... ;
void fLOOPP() {    
    if (loopDepth > 0) {
        int x = (loopDepth-1) * 3;
        CELL f = loopSTK[x];
        CELL t = loopSTK[x+1];
        loopSTK[x+2] += pop();
        CELL i = loopSTK[x+2];
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
}
void fUNUSED7() {         // opcode #58
}
// ( a -- )
void fPARSEWORD() {    // opcode #59
    CELL wa = pop();
    char *w = (char *)&dict[wa];
    // printStringF("-pw[%s]-", w);
    push(wa); fFIND();
    if (pop()) {
        DICT_T *dp = (DICT_T *)&dict[T];
        fGETXT();
        CELL xt = pop();
        if (compiling(w, 0)) {
            if (dp->flags == 1) {
                // 1 => IMMEDIATE
                run(xt, 0);
            } else if (dp->flags == 2) {
                // 2 => INLINE
                BYTE x = dict[xt];
                while (x != OP_RET) {
                    CCOMMA(x);
                    x = dict[++xt];
                }
            } else {
                CCOMMA(OP_CALL);
                ACOMMA((ADDR)xt);
            }
        } else {
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

    if (strcmp(w, ";") == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_RET);
        sys->STATE = 0;
        return;
    }

    if (strcmp(w, "if") == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_JMPZ);
        push(sys->HERE);
        ACOMMA(0);
        return;
    }

    if (strcmp(w, "if-") == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_NJMPZ);
        push(sys->HERE);
        ACOMMA(0);
        return;
    }

    if (strcmp(w, "else") == 0) {
        if (! compiling(w, 1)) { return; }
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
        if (! compiling(w, 1)) { return; }
        push(sys->HERE);
        fSWAP();
        fASTORE();
        return;
    }

    if (strcmp(w, "begin") == 0) {
        if (! compiling(w, 1)) { return; }
        push(sys->HERE);
        return;
    }

    if (strcmp(w, "repeat") == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_JMP);
        fACOMMA();
        return;
    }

    if (strcmp(w, "while") == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_JMPNZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "until") == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_JMPZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "while-") == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_NJMPNZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "until-") == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_NJMPZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "do") == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_DO);
        push(sys->HERE);
        return;
    }

    if (strcmp(w, "leave") == 0) {
        if (! compiling(w, 1)) { return; }
        printString("WARNING: LEAVE not supported!");
        return;
    }

    if (strcmp(w, "loop") == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_LOOP);
        CCOMMA(OP_JMPNZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "loop+") == 0) {
        if (! compiling(w, 1)) { return; }
        CCOMMA(OP_LOOPP);
        CCOMMA(OP_JMPNZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, ":") == 0) {
        if (! interpreting(w, 1)) { return; }
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
        if (! interpreting(w, 1)) { return; }
        push(wa);
        fNEXTWORD();
        if (pop()) {
            push(wa);
            fCREATE();
            CCOMMA(OP_LIT);
            COMMA(sys->HERE+CELL_SZ+1);
            CCOMMA(OP_RET);
            COMMA(0);
        }
        return;
    }

    if (strcmp(w, "constant") == 0) {
        if (! interpreting(w, 1)) { return; }
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

    BYTE op = getOpcode(w);
    if (op < 0xFF) {
        if (compiling(w, 0)) {
            CCOMMA(op);
        } else {
            CELL xt = sys->HERE+0x20;
            dict[xt] = op;
            dict[xt+1] = OP_RET;
            run(xt, 0);
        }
        return;
    }
    sys->STATE = 0;
    printStringF("[%s]??", w);
}
void fPARSELINE() {    // opcode #60
    // printString("x");
    sys->TOIN = pop();
    CELL buf = allocSpace(32);
    char *w = (char *)&dict[buf];
    push(buf);
    fNEXTWORD();
    while (pop()) {
        if (strcmp(w, "//") == 0) { break; }
        if (strcmp(w, "\\") == 0) { break; }
        push(buf);
        fPARSEWORD();
        push(buf);
        fNEXTWORD();
    }
    allocFree(buf);
}
void fGETXT() {        // opcode #61
    DICT_T *dp = (DICT_T *)&dict[T];
    T += ADDR_SZ + dp->len + 3;
}
void fALIGN2() {       // opcode #62
    CELL val = T;
    if (val & 0x01) { ++val; }
    T = val;
}
void fALIGN4() {       // opcode #63
    CELL val = T;
    while (val & 0x03) { ++val; }
    T = val;
}
void fCREATE() {       // opcode #64
    CELL wa = pop();
    char *name = (char *)&dict[wa];
    sys->HERE = align2(sys->HERE);
    // printStringF("-define [%s] at %d (%lx)", name, sys->HERE, sys->HERE);

    DICT_T *dp = (DICT_T *)&dict[sys->HERE];
    dp->prev = (ADDR)sys->LAST;
    dp->flags = 0;
    dp->len = strlen(name);
    strcpy((char *)dp->name, name);
    sys->LAST = sys->HERE;
    sys->HERE += ADDR_SZ + dp->len + 3;
    // printStringF(",XT:%d (%lx)-", sys->HERE, sys->HERE);
}
// (a1 -- [a2 1] | 0)
void fFIND() {         // opcode #65
    char *name = (char *)&dict[pop()];
    // printStringF("-lf:[%s]-", name);
    CELL cl = sys->LAST;
    while (cl) {
        DICT_T *dp = (DICT_T *)&dict[cl];
        if (strcmp(name, (char *)dp->name) == 0) {
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
    char *w = (char *)&dict[wa];

    if ((*w == '\'') && (*(w+2) == '\'') && (*(w+3) == 0)) {
        push(*(w+1));
        push(1);
        return;
    }

    if (*w == '#') { is_decimal(w+1); return; }
    if (*w == '$') { is_hex(w+1);     return; }
    if (*w == '%') { is_binary(w+1);  return; }

    if (sys->BASE == 10) { is_decimal(w); return; }
    if (sys->BASE == 16) { is_hex(w);     return; }
    if (sys->BASE ==  2) { is_binary(w);  return; }
    push(0);
}
void fNJMPZ() {        // opcode #68
    if (T == 0) PC = addrAt(PC);
    else PC += ADDR_SZ;
}
void fNJMPNZ() {       // opcode #69
    if (T) PC = addrAt(PC);
    else PC += ADDR_SZ;
}
// OP_LESS (#70)    : < ( TODO -- TODO ) ... ;
void fLESS() {     
    N = (N < T) ? 1 : 0; pop();
}
// OP_EQUALS (#71)    : = ( TODO -- TODO ) ... ;
void fEQUALS() {   
    N = (N == T) ? 1 : 0; pop();
}
// OP_GREATER (#72)    : > ( TODO -- TODO ) ... ;
void fGREATER() {  
    N = (N > T) ? 1 : 0; pop();
}
// OP_I (#73)    : I ( -- n ) ... ;
void fI() {        
    if (loopDepth > 0) {
        CELL x = (loopDepth-1) * 3;
        push(loopSTK[x+2]);
    }
    else {
        printString("-I:depthErr-");
        push(0);
    }
}
// OP_J (#74)    : J ( -- n ) ... ;
void fJ() {        
    if (loopDepth > 1) {
        CELL x = (loopDepth-2) * 3;
        push(loopSTK[x+2]);
    }
    else {
        printString("-J:depthErr-");
        push(0);
    }
}
// OP_INPUTPIN (#75)    : input ( n -- ) ... ;
void fINPUTPIN() { 
    CELL pin = pop();
    #ifdef __DEV_BOARD__
        // printStringF("-pinMode(%d, INPUT)-", pin);
        pinMode(pin, INPUT);
    #else
        printStringF("-pinMode(%d, INPUT)-", pin);
    #endif
}
// OP_OUTPUTPIN (#76)    : output ( n -- ) ... ;
void fOUTPUTPIN() { 
    CELL pin = pop();
    #ifdef __DEV_BOARD__
        // printStringF("-pinMode(%d, OUTPUT)-", pin);
        pinMode(pin, OUTPUT);
    #else
        printStringF("-pinMode(%d, OUTPUT)-", pin);
    #endif
}
// OP_DELAY (#77)    : MS ( n -- ) ... ;
void fDELAY() { 
    CELL ms = pop();
    #ifdef __DEV_BOARD__
        delay(ms);
    #else
        Sleep(ms);
    #endif
}
// OP_TICK (#78)    : TICK ( -- n ) ... ;
void fTICK() { 
    #ifdef __DEV_BOARD__
        push(millis());
    #else
        push(GetTickCount());
    #endif
}
// OP_APINSTORE (#79)    : ap! ( n1 n2 -- ) ... ;
void fAPINSTORE() {
    CELL pin = pop();
    CELL val = pop();
    #ifdef __DEV_BOARD__
        // printStringF("-analogWrite(%d, OUTPUT)-", pin);
        analogWrite((int)pin, (int)val);
    #else
        printStringF("-analogWrite(%ld, %ld)-", pin, val);
    #endif
}
// OP_DPINSTORE (#80)    : dp! ( n1 n2 -- ) ... ;
void fDPINSTORE() {
    CELL pin = pop();
    CELL val = pop();
    #ifdef __DEV_BOARD__
        // printStringF("-digitalWrite(%d, %d)-", (int)pin, (int)val);
        digitalWrite((int)pin, (int)val);
    #else
        printStringF("-digitalWrite(%ld, %ld)-", pin, val);
    #endif
}
// OP_APINFETCH (#81)    : ap@ ( n -- n ) ... ;
void fAPINFETCH() {
    #ifdef __DEV_BOARD__
        // printStringF("-analogRead(%d, A0=%d)-", T, A0);
        T = analogRead((int)T);
    #else
        printStringF("-analogRead(%ld)-", T);
    #endif
}
// OP_DPINFETCH (#82)    : dp@ ( n -- n ) ... ;
void fDPINFETCH() {
    #ifdef __DEV_BOARD__
        // printStringF("-digitalRead(%d)-", T);
        T = digitalRead((int)T);
    #else
        printStringF("-digitalRead(%ld)-", T);
    #endif
}
// OP_MWFETCH (#83)    : mw@ ( n1 -- n2 ) ... ;
void fMWFETCH() {  
    T = wordAt(T);
}
// OP_MCSTORE (#84)    : mc! ( n1 n2 -- ) ... ;
void fMCSTORE() {  
    BYTE *a = (BYTE *)pop();
    BYTE v = (BYTE)pop();
    *a = v;
}
// OP_NUM2STR (#85)    : num>str ( n l -- a ) ... ;
void fNUM2STR() {      
    BYTE reqLen = (BYTE)pop();
    CELL num = pop();
    BYTE len = 0;
    int isNeg = (num < 0);
    CELL pad = allocSpace(48);
    CELL cp = pad+47;
    dict[cp--] = (BYTE) 0;
    num = (isNeg) ? -num : num;

    do {
        BYTE r = (num % sys->BASE) + '0';
        if ('9' < r) { r += 7; }
        dict[cp--] = r;
        len++;
        num /= sys->BASE;
    } while (num > 0);

    while (len < reqLen) {
        dict[cp--] = '0';
        ++len;
    }
    dict[cp] = len;
    push(cp+1);
    push(len);
    allocFree(pad);
}
// OP_BYE (#86)    : bye ( -- ) ... ;
void fBYE() {      
    // TODO N = N*T; push(T); pop();
}


/* NimbleText script
$once
// vvvvv - NimbleText generated - vvvvv
$each
// OP_$0 (#$rownum)    : $1 ( TODO -- TODO ) ... ;
void f<%($0 + '() {               ').substring(0,13)%>
    // TODO N = N*T; push(T); pop();
}
$once
// ^^^^^ - NimbleText generated - ^^^^^
*/
