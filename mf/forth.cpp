// ---------------------------------------------------------------------
// forth.c
// ---------------------------------------------------------------------

#include "defs.h"

int digitalRead(int pin) {return 0;}
int analogRead(int pin) {return 0;}
void digitalWrite(int pin, int val) {}
void analogWrite(int pin, int val) {}
void delay(int ms) {}
long millis() { return 0; }

BYTE IR;
ADDR PC;
ADDR HERE;
ADDR LAST;
char *toIN;
BYTE dict[DICT_SZ];
char TIB[TIB_SZ];
CELL BASE, STATE, DSP, RSP;
CELL loopSTK[12];
CELL dstk[STK_SZ+1];
CELL rstk[STK_SZ+1];
CELL loopDepth;
ALLOC_T allocTbl[ALLOC_SZ];
int num_alloced = 0, numTIB = 0;
ADDR allocAddrBase, allocCurFree;
FP prims[256];

void vmInit() {
    init_handlers();
    HERE = &dict[0];
    LAST = 0;
    BASE = 10;
    STATE = 0;
    DSP = 0;
    RSP = 0;
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

void addrStore(ADDR addr, CELL val) {
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

void fDUMPDICT() {
    ADDR addr = &dict[0];
    FILE *to = (FILE *)pop();
    to = to ? to : stdout;
    fprintf(to, "%04lx %04lx (%lu %lu)", (ulong)HERE, (ulong)LAST, (ulong)HERE, (ulong)LAST);
    for (int i = 0; i < 999999; i++) {
        if (addr >= HERE) { return; }
        if (i % 16 == 0) fprintf(to, "\r\n %08lx:", (ulong)addr);
        fprintf(to, " %02x", *(addr++));
    }
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

void init_handlers() {
    for (int i = 0; i < 256; i++) {
        prims[i] = 0;
    }
    prims[OP_NOOP] = fNOOP;
    prims[OP_CLIT] = fCLIT;
    prims[OP_WLIT] = fWLIT;
    prims[OP_LIT] = fLIT;
    prims[OP_CFETCH] = fCFETCH;
    prims[OP_WFETCH] = fWFETCH;
    prims[OP_AFETCH] = fAFETCH;
    prims[OP_FETCH] = fFETCH;
    prims[OP_CSTORE] = fCSTORE;
    prims[OP_WSTORE] = fWSTORE;
    prims[OP_ASTORE] = fASTORE;
    prims[OP_STORE] = fSTORE;
    prims[OP_CCOMMA] = fCCOMMA;
    prims[OP_WCOMMA] = fWCOMMA;
    prims[OP_COMMA] = fCOMMA;
    prims[OP_ACOMMA] = fACOMMA;
    prims[OP_CALL] = fCALL;
    prims[OP_RET] = fRET;
    prims[OP_JMP] = fJMP;
    prims[OP_JMPZ] = fJMPZ;
    prims[OP_JMPNZ] = fJMPNZ;
    prims[OP_ONEMINUS] = fONEMINUS;
    prims[OP_ONEPLUS] = fONEPLUS;
    prims[OP_DUP] = fDUP;
    prims[OP_SWAP] = fSWAP;
    prims[OP_DROP] = fDROP;
    prims[OP_OVER] = fOVER;
    prims[OP_ADD] = fADD;
    prims[OP_SUB] = fSUB;
    prims[OP_MULT] = fMULT;
    prims[OP_SLMOD] = fSLMOD;
    prims[OP_LSHIFT] = fLSHIFT;
    prims[OP_RSHIFT] = fRSHIFT;
    prims[OP_AND] = fAND;
    prims[OP_OR] = fOR;
    prims[OP_XOR] = fXOR;
    prims[OP_NOT] = fNOT;
    prims[OP_DTOR] = fDTOR;
    prims[OP_RFETCH] = fRFETCH;
    prims[OP_RTOD] = fRTOD;
    prims[OP_EMIT] = fEMIT;
    prims[OP_TYPE] = fTYPE;
    prims[OP_DOTS] = fDOTS;
    prims[OP_SQUOTE] = fDOTQUOTE;
    prims[OP_PAREN] = fPAREN;
    prims[OP_WDTFEED] = fWDTFEED;
    prims[OP_BREAK] = fBREAK;
    prims[OP_CMOVE] = fCMOVE;
    prims[OP_CMOVE2] = fCMOVE2;
    prims[OP_FILL] = fFILL;
    prims[OP_OPENBLOCK] = fOPENBLOCK;
    prims[OP_FILECLOSE] = fFILECLOSE;
    prims[OP_FILEREAD] = fFILEREAD;
    prims[OP_LOAD] = fLOAD;
    prims[OP_THRU] = fTHRU;
    prims[OP_DO] = fDO;
    prims[OP_LOOP] = fLOOP;
    prims[OP_LOOPP] = fLOOPP;
    prims[OP_PARSEWORD] = fPARSEWORD;
    prims[OP_PARSELINE] = fPARSELINE;
    prims[OP_GETXT] = fGETXT;
    prims[OP_ALIGN2] = fALIGN2;
    prims[OP_ALIGN4] = fALIGN4;
    prims[OP_CREATE] = fCREATE;
    prims[OP_FIND] = fFIND;
    prims[OP_NEXTWORD] = fNEXTWORD;
    prims[OP_ISNUMBER] = fISNUMBER;
    prims[OP_NJMPZ] = fNJMPZ;
    prims[OP_NJMPNZ] = fNJMPNZ;
    prims[OP_LESS] = fLESS;
    prims[OP_EQUALS] = fEQUALS;
    prims[OP_GREATER] = fGREATER;
    prims[OP_I] = fI;
    prims[OP_J] = fJ;
    prims[OP_INPUTPIN] = fINPUTPIN;
    prims[OP_OUTPUTPIN] = fOUTPUTPIN;
    prims[OP_DELAY] = fDELAY;
    prims[OP_TICK] = fTICK;
    prims[OP_APINSTORE] = fAPINSTORE;
    prims[OP_DPINSTORE] = fDPINSTORE;
    prims[OP_APINFETCH] = fAPINFETCH;
    prims[OP_DPINFETCH] = fDPINFETCH;
    prims[OP_MWFETCH] = fMWFETCH;
    prims[OP_MCSTORE] = fMCSTORE;
    prims[OP_NUM2STR] = fNUM2STR;
    prims[OP_COM] = fCOM;
    prims[OP_BYE] = fBYE;
}

void fNOOP() {         // opcode #0
}
void fCLIT() {         // opcode #1
    push(*(PC++));
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
    ADDR addr = (ADDR)T;
    T = *addr;
}
void fWFETCH() {       // opcode #5
    ADDR addr = (ADDR)T;
    T = wordAt(addr);
}
void fAFETCH() {       // opcode #6
    (ADDR_SZ == 2) ? fWFETCH() : fFETCH();
}
void fFETCH() {        // opcode #7
    CELL addr = T;
    T = cellAt((ADDR)addr);
}
void fCSTORE() {       // opcode #8
    ADDR addr = (ADDR)pop();
    CELL val = pop();
    *(addr) = (val & 0xFF);
}
void fWSTORE() {       // opcode #9
    ADDR addr = (ADDR)pop();
    CELL val = pop();
    wordStore(addr, val);
}
void fASTORE() {       // opcode #10
    (ADDR_SZ == 2) ? fWSTORE() : fSTORE();
}
void fSTORE() {        // opcode #11
    ADDR addr = (ADDR)pop();
    CELL val = pop();
    cellStore(addr, val);
}
void fCCOMMA() {       // opcode #12
    *(HERE++) = (BYTE)pop();
}
void fWCOMMA() {       // opcode #13
    WORD x = (WORD)pop();
    wordStore(HERE, x);
    HERE += WORD_SZ;
}
void fCOMMA() {        // opcode #14
    CELL x = pop();
    cellStore(HERE, x);
    HERE += CELL_SZ;
}
void fACOMMA() {       // opcode #15
    (ADDR_SZ == 2) ? fWCOMMA() : fCOMMA();
}
void fCALL() {         // opcode #16
    rpush((CELL)PC+ADDR_SZ);
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
// OP_NOT (#36)    : not ( n -- 0|1 ) ... ;
void fNOT() {          
    T = (T) ? 0 : 1;
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
    int n = (int)pop();
    char *a = (char *)pop();
    char x[2];
    x[1] = 0;
    for (int i = 0; i < n; i++ ) {
        x[0] = *(a++);
        printString(x);
    }
}
void fDOTS() {
    if (DSP) {
        push('('); fEMIT();
        for (int i = 1; i <= DSP; i++) {
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
    ADDR to = (ADDR)pop();
    ADDR from = (ADDR)pop();
    while (num > 0) {
        *(to++) = *(from++);
        num--;
    }
}
// OP_CMOVE2 (#48)    : CMOVE> ( TODO -- TODO ) ... ;
void fCMOVE2() {   
    CELL num = pop();
    ADDR to = (ADDR)pop();
    ADDR from = (ADDR)pop();

    to += (num-1);
    from += (num-1);

    while (num > 0) {
        *(to--) = *(from--);
        num--;
    }
}
// OP_FILL (#49)    : FILL ( TODO -- TODO ) ... ;
void fFILL() {    
    CELL val = pop();
    CELL num = pop();
    ADDR to = (ADDR)pop();
    while (num > 0) {
        *(to++) = val;
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
void fPARSELINE() {    // opcode #60
    toIN = (char *)pop();
    // printStringF("-toIn: %s-", toIN);
    char *buf = (char *)allocSpace(32);
    push((CELL)buf);
    fNEXTWORD();
    while (pop()) {
        // printStringF("-word: %s-", buf);
        if (strcmp(buf, "//") == 0) { break; }
        if (strcmp(buf, "\\") == 0) { break; }
        push((CELL)buf);
        fPARSEWORD();
        push((CELL)buf);
        fNEXTWORD();
    }
    allocFree((ADDR)buf);
}
void fGETXT() {        // opcode #61
    DICT_T *dp = (DICT_T *)T;
    T += (ADDR_SZ + dp->len + 3);
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
    char *name = (char *)pop();
    HERE = align4(HERE);
    // printStringF("-define [%s] at %d (%lx)", name, HERE, HERE);

    DICT_T *dp = (DICT_T *)HERE;
    dp->prev = (ADDR)LAST;
    dp->flags = 0;
    dp->len = strlen(name);
    strcpy(dp->name, name);
    LAST = HERE;
    HERE += ADDR_SZ + dp->len + 3;
    // printStringF(",XT:%d (%lx)-", HERE, HERE);
}
// (a1 -- [a2 1] | 0)
void fFIND() {         // opcode #65
    char *name = (char *)pop();
    // printStringF("-find:[%s]-", name);
    DICT_T *dp = (DICT_T *)LAST;
    while (dp) {
        // printStringF("-==%s?-", dp->name);
        if (strcmp(name, dp->name) == 0) {
            // printStringF("-FOUND! (%lx)-", dp);
            push((CELL)dp);
            push(1);
            return;
        }
        dp = (DICT_T *)dp->prev;
    }
    push(0);
}
void fNEXTWORD() {     // opcode #66
    char *to = (char *)pop();
    char c = nextChar();
    // printf("-c=%d-", c);
    int len = 0;
    while (c && (c < 33)) { c = nextChar(); }
    while (c && (32 < c)) {
        *(to++) = c;
        c = nextChar(); 
        // printf("-c=%d-", c);
        len++;
    }
    *(to) = 0;
    push(len);
}
void fISNUMBER() {     // opcode #67
    char *w = (char *)pop();

    if ((*w == '\'') && (*(w+2) == '\'') && (*(w+3) == 0)) {
        push(*(w+1));
        push(1);
        return;
    }

    if (*w == '#') { is_decimal(w+1); return; }
    if (*w == '$') { is_hex(w+1);     return; }
    if (*w == '%') { is_binary(w+1);  return; }

    if (BASE == 10) { is_decimal(w); return; }
    if (BASE == 16) { is_hex(w);     return; }
    if (BASE ==  2) { is_binary(w);  return; }
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
        printStringF("-pinMode(%d, INPUT)-", pin);
        // pinMode(pin, INPUT);
    #else
        printStringF("-pinMode(%d, INPUT)-", pin);
    #endif
}
// OP_OUTPUTPIN (#76)    : output ( n -- ) ... ;
void fOUTPUTPIN() { 
    CELL pin = pop();
    #ifdef __DEV_BOARD__
        printStringF("-pinMode(%d, OUTPUT)-", pin);
        // pinMode(pin, OUTPUT);
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
        PinName pinNum = (PinName)pop();
        DigitalOut pin(pinNum);
        pin = val;
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
    T = wordAt((ADDR)T);
}
// OP_MCSTORE (#84)    : mc! ( n1 n2 -- ) ... ;
void fMCSTORE() {  
    ADDR a = (ADDR)pop();
    BYTE v = (BYTE)pop();
    *a = v;
}
// OP_NUM2STR (#85)    : num>str ( n l -- a ) ... ;
void fNUM2STR() {      
    BYTE reqLen = (BYTE)pop();
    CELL num = pop();
    BYTE len = 0;
    int isNeg = (num < 0);
    ADDR pad = HERE + 10;
    // allocFree(pad);
    ADDR cp = pad+35;
    *(cp--) = (BYTE) 0;
    num = (isNeg) ? -num : num;

    do {
        BYTE r = (num % BASE) + '0';
        if ('9' < r) { r += 7; }
        *(cp--) = r;
        len++;
        num /= BASE;
    } while (num > 0);

    while (len < reqLen) {
        *(cp--) = '0';
        ++len;
    }
    *(cp) = len;
    push((CELL)(cp+1));
    push(len);
}
// OP_COM (#86)    : com ( n1 -- n2 ) ... ;
void fCOM() {      
    T = ~T;
}
// OP_BYE (#87)    : bye ( -- ) ... ;
void fKEMIT() {
    CELL key = pop();
    CELL mods = pop();
    #ifdef __HAS_KEYBOARD__
        printStringF("-kb:%d,%d-", key, mods);
    #else
        printStringF("-kb:%d,%d-", key, mods);
    #endif
}
// OP_BYE (#88)    : bye ( -- ) ... ;
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

// ( a -- )
void fPARSEWORD() {    // opcode #59
    char *w = (char *)pop();
    // printStringF("-pw[%s]-", w);
    push((CELL)w); fFIND();
    if (pop()) {
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

    push((CELL)w); fISNUMBER();
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
        fNEXTWORD();
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
    strcpy(TIB, line);
    push((CELL)&TIB[0]);
    fPARSELINE();
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
BYTE getOpcode(char *w) {
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
    if (strcmp_PF(w, PSTR("com")) == 0) return OP_COM;       //  opcode #86
    // if (strcmp_PF(w, PSTR("kemit")) == 0) return OP_KEMIT;       //  opcode #87
    if (strcmp_PF(w, PSTR("bye")) == 0) return OP_BYE;       //  opcode #88
    return 0xFF;
}
// ^^^^^ - NimbleText generated - ^^^^^

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------

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
