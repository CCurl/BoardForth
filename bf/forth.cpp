#include "board.h"
#ifndef __DEV_BOARD__
#include <windows.h>
#endif

#include "defs.h"
#pragma warning(disable: 4996)

CELL* dstk;
CELL* rstk;

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
        // printStringF("\n-PC-%d/%lx:IR-%d/%x-", PC-1, PC-1, (int)IR, (unsigned int)IR); fDOTS();
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
            if (pop() == 0) {
                PC = addrAt(PC);
            }
            else {
                PC += ADDR_SZ;
            }
            break;
        case OP_JMPNZ:     // -n- (#20)
            if (pop() != 0) {
                PC = addrAt(PC);
            }
            else {
                PC += ADDR_SZ;
            }
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
            N *= 2;
            break;
        case OP_RSHIFT:     // >> (#32)
            N *= 2;
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
        case OP_DOTQUOTE:     // .\" (#43)
            // TODO: N += T; pop();
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
        case OP_UNUSED7:     // -n- (#58)
            N += T; pop();
            break;
        case OP_PARSEWORD:     // parse-word (#59)
            fPARSEWORD();
            break;
        case OP_PARSELINE:     // parse-line (#60)
            fPARSELINE();
            break;
        case OP_GETXT:     // >body (#61)
            fGETXT();
            break;
        case OP_ALIGN2:     // align2 (#62)
            fALIGN2();
            break;
        case OP_ALIGN4:     // align4 (#63)
            fALIGN4();
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
            if (T == 0) {
                PC = addrAt(PC);
            }
            else {
                PC += ADDR_SZ;
            }
            break;
        case OP_NJMPNZ:     // -n- (#69)
            if (T != 0) {
                PC = addrAt(PC);
            }
            else {
                PC += ADDR_SZ;
            }
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
            fINPUTPIN();
            break;
        case OP_OUTPUTPIN:     // output (#76)
            fOUTPUTPIN();
            break;
        case OP_DELAY:     // ms (#77)
            fDELAY();
            break;
        case OP_TICK:     // ms (#78)
            fTICK();
            break;
        case OP_APINSTORE:     //  (#79)
            fAPINSTORE();
            break;
        case OP_DPINSTORE:     // dp! (#80)
            fDPINSTORE();
            break;
        case OP_APINFETCH:     // ap@ (#81)
            fAPINFETCH();
            break;
        case OP_DPINFETCH:     // dp@ (#82)
            fDPINFETCH();
            break;
        case OP_MWFETCH:     // mw@ (#83)
            a1 = (BYTE*)T;
            T = (*(a1 + 1) << 8) | (*a1);
            break;
        case OP_MCSTORE:     // mc! (#84)
            fMCSTORE();
            break;
        case OP_NUM2STR:     // num>str (#85)
            fNUM2STR();
            break;
            // case OP_COM:     // com (#86)
            //     fco
            //     break;
        case OP_BYE:     // bye (#87)
            // N += T; pop();
            break;

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
    return (dict[loc + 3] << 24) + (dict[loc + 2] << 16) + (dict[loc + 1] << 8) + dict[loc];
}

CELL wordAt(CELL loc) {
    return (dict[loc + 1] << 8) + dict[loc];
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
    fprintf(to, "%04x %04x (%ld %ld)", sys->HERE, sys->LAST, sys->HERE, sys->LAST);
    for (int i = 0; i < sys->HERE; i++) {
        if (i % 16 == 0) {
            if (n) { x[n] = 0; fprintf(to, " ; %s", x); }
            fprintf(to, "\n %04x:", i);
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
// FP prims[] = {
//     fNOOP,             // OP_NOOP (#0) ***NOOP NOOP***
//     fCLIT,             // OP_CLIT (#1) ***CLIT CLITERAL ( -- N8 )***
//     fWLIT,             // OP_WLIT (#2) ***WLIT WLITERAL ( -- N16 )***
//     fLIT,              // OP_LIT (#3) ***LIT LITERAL ( -- N )***
//     fCFETCH,           // OP_CFETCH (#4) ***CFETCH C@ (A -- N8)***
//     fWFETCH,           // OP_WFETCH (#5) ***WFETCH W@ (A -- N16)***
//     fAFETCH,           // OP_AFETCH (#6) ***AFETCH A@ (A -- N)***
//     fFETCH,            // OP_FETCH (#7) ***FETCH @ (A -- N8)***
//     fCSTORE,           // OP_CSTORE (#8) ***CSTORE C! ( N A -- )***
//     fWSTORE,           // OP_WSTORE (#9) ***WSTORE W! ( N A -- )***
//     fASTORE,           // OP_ASTORE (#10) ***ASTORE A! ( N A -- )***
//     fSTORE,            // OP_STORE (#11) ***STORE ! (  N A -- )***
//     fCCOMMA,           // OP_CCOMMA (#12) ***CCOMMA C, ( N -- )***
//     fWCOMMA,           // OP_WCOMMA (#13) ***WCOMMA W, ( N -- )***
//     fCOMMA,            // OP_COMMA (#14) ***COMMA , ( N -- )***
//     fACOMMA,           // OP_ACOMMA (#15) ***ACOMMA A, ( N -- )***
//     fCALL,             // OP_CALL (#16) ***CALL CALL ( -- )***
//     fRET,              // OP_RET (#17) ***RET EXIT ( -- )***
//     fJMP,              // OP_JMP (#18) ***JMP -N- ( -- ) ***
//     fJMPZ,             // OP_JMPZ (#19) ***JMPZ -N- ( N -- )***
//     fJMPNZ,            // OP_JMPNZ (#20) ***JMPNZ -N- ( N -- )***
//     fONEMINUS,         // OP_ONEMINUS (#21) ***ONEMINUS 1- ( N -- N-1 )***
//     fONEPLUS,          // OP_ONEPLUS (#22) ***ONEPLUS 1+ ( N -- N+1 )***
//     fDUP,              // OP_DUP (#23) ***DUP DUP ( N -- N N )***
//     fSWAP,             // OP_SWAP (#24) ***SWAP SWAP ( N1 N2 -- N2 N1 )***
//     fDROP,             // OP_DROP (#25) ***DROP DROP (N -- )***
//     fOVER,             // OP_OVER (#26) ***OVER OVER ( N1 N2 -- N1 N2 N1 )***
//     fADD,              // OP_ADD (#27) ***ADD + ( N1 N2 -- N3 )***
//     fSUB,              // OP_SUB (#28) ***SUB - ( N1 N2 -- N3 )***
//     fMULT,             // OP_MULT (#29) ***MULT * ( N1 N2 -- N3 )***
//     fSLMOD,            // OP_SLMOD (#30) ***SLMOD /MOD ( N1 N2 -- N3 N4 )***
//     fLSHIFT,           // OP_LSHIFT (#31) ***LSHIFT << ( N -- N*2 )***
//     fRSHIFT,           // OP_RSHIFT (#32) ***RSHIFT >> ( N -- N/2 )***
//     fAND,              // OP_AND (#33) ***AND AND  ( N1 N2 -- N3 )***
//     fOR,               // OP_OR (#34) ***OR OR ( N1 N2 -- N3 )***
//     fXOR,              // OP_XOR (#35) ***XOR XOR ( N1 N2 -- N3 )***
//     fNOT,              // OP_NOT (#36) ***NOT NOT ( N1 -- N2 )***
//     fDTOR,             // OP_DTOR (#37) ***DTOR >R ( N -- )***
//     fRFETCH,           // OP_RFETCH (#38) ***RFETCH R@ (-- N )***
//     fRTOD,             // OP_RTOD (#39) ***RTOD R> ( -- N )***
//     fEMIT,             // OP_EMIT (#40) ***EMIT EMIT (N -- )***
//     fTYPE,             // OP_TYPE (#41) ***TYPE type (a n -- )***
//     fDOTS,             // OP_DOTS (#42) ***DOTS .S ( -- )***
//     fDOTQUOTE,         // OP_DOTQUOTE (#43) ***DOTQUOTE .\" ( -- )***
//     fPAREN,            // OP_PAREN (#44) ***PAREN ( ( -- )***
//     fWDTFEED,          // OP_WDTFEED (#45) ***WDTFEED WDTFEED ( -- )***
//     fBREAK,            // OP_BREAK (#46) ***BREAK BRK ( -- )***
//     fCMOVE,            // OP_CMOVE (#47) ***CMOVE CMOVE ( a1 a2 u -- )***
//     fCMOVE2,           // OP_CMOVE2 (#48) ***CMOVE2 CMOVE> ( a1 a2 u -- )***
//     fFILL,             // OP_FILL (#49) ***FILL FILL ( a u n -- )***
//     fOPENBLOCK,        // OP_OPENBLOCK (#50) ***OPENBLOCK OPEN-BLOCK***
//     fFILECLOSE,        // OP_FILECLOSE (#51) ***FILECLOSE FILE-CLOSE***
//     fFILEREAD,         // OP_FILEREAD (#52) ***FILEREAD FILE-READ***
//     fLOAD,             // OP_LOAD (#53) ***LOAD LOAD***
//     fTHRU,             // OP_THRU (#54) ***THRU THRU***
//     fDO,               // OP_DO (#55) ***DO DO ( f t -- )***
//     fLOOP,             // OP_LOOP (#56) ***LOOP LOOP ( -- )***
//     fLOOPP,            // OP_LOOPP (#57) ***LOOPP LOOP+  ( n -- )***
//     fUNUSED7,          // OP_UNUSED7 (#58) ***UNUSED7 -n- ( -- )***
//     fPARSEWORD,        // OP_PARSEWORD (#59) ***PARSEWORD PARSE-WORD ( A -- )***
//     fPARSELINE,        // OP_PARSELINE (#60) ***PARSELINE PARSE-LINE (A -- )***
//     fGETXT,            // OP_GETXT (#61) ***GETXT >BODY ( A1 -- A2 )***
//     fALIGN2,           // OP_ALIGN2 (#62) ***ALIGN2 ALIGN2 ( N1 -- N2 )***
//     fALIGN4,           // OP_ALIGN4 (#63) ***ALIGN4 ALIGN4 ( N1 -- N2 )***
//     fCREATE,           // OP_CREATE (#64) ***CREATE CREATE ( A -- )***
//     fFIND,             // OP_FIND (#65) ***FIND FIND ( A1 -- (A1 1)|0 )***
//     fNEXTWORD,         // OP_NEXTWORD (#66) ***NEXTWORD NEXT-WORD ( A -- )***
//     fISNUMBER,         // OP_ISNUMBER (#67) ***ISNUMBER NUMBER? ( A -- (N 1)|0 )***
//     fNJMPZ,            // OP_NJMPZ (#68) ***NJMPZ -N- ( N -- N )***
//     fNJMPNZ,           // OP_NJMPNZ (#69) ***NJMPNZ -N- ( N -- N )***
//     fLESS,             // OP_LESS (#70) ***LESS < ( N1 N2 -- N3 )***
//     fEQUALS,           // OP_EQUALS (#71) ***EQUALS = ( N1 N2 -- N3 )***
//     fGREATER,          // OP_GREATER (#72) ***GREATER > ( N1 N2 -- N3 )***
//     fI,                // OP_I (#73) ***I I ( -- n )***
//     fJ,                // OP_J (#74) ***J J ( -- n )***
//     fINPUTPIN,         // OP_INPUTPIN (#75) ***INPUTPIN input ( n -- )***
//     fOUTPUTPIN,        // OP_OUTPUTPIN (#76) ***OUTPUTPIN output ( n -- )***
//     fDELAY,            // OP_DELAY (#77) ***DELAY MS ( n -- )***
//     fTICK,             // OP_TICK (#78) ***DELAY MS ( n -- )***
//     fAPINSTORE,        // OP_APINSTORE (#79) ***APINSTORE  ap! ( n1 n2 -- )***
//     fDPINSTORE,        // OP_DPINSTORE (#80) ***DPINSTORE dp! ( n1 n2 -- )***
//     fAPINFETCH,        // OP_APINFETCH (#81) ***APINFETCH ap@ ( n1 -- n2 )***
//     fDPINFETCH,        // OP_DPINFETCH (#82) ***DPINFETCH dp@ ( n1 -- n2 )***
//     fMWFETCH,          // OP_MWFETCH (#83) ***MWFETCH mw@ ( n1 -- n2 )***
//     fMCSTORE,          // OP_MCSTORE (#84) ***MCSTORE mc! ( n1 n2 -- )***
//     fNUM2STR,          // OP_NUM2STR (#85) ***NUM2STR num>str ( n l -- a )***
//     fBYE,              // OP_BYE (#86) ***BYE bye ( -- )***
//     0};
// ^^^^^ - NimbleText generated - ^^^^^


void fNOOP() {         // opcode #0
}
void fCLIT() {         // opcode #1
    // push(dict[PC++]);
}
void fWLIT() {         // opcode #2
    // push(wordAt(PC));
    // PC += WORD_SZ;
}
void fLIT() {          // opcode #3
    // push(cellAt(PC));
    // PC += CELL_SZ;
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
    if ((0 <= addr) && ((addr + 2) < DICT_SZ)) {
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
    if ((0 <= addr) && ((addr + 4) < DICT_SZ)) {
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
void fCALL() {         // opcode #16
    // rpush(PC_OLD+ADDR_SZ);
    // PC_OLD = addrAt(PC_OLD);
    // printStringF("-call:%lx-", PC);
}
void fRET() {          // opcode #17
    // handled in run()
}
void fJMP() {          // opcode #18
    // PC_OLD = addrAt(PC_OLD);
}
void fJMPZ() {         // opcode #19
    // if (pop() == 0) PC_OLD = addrAt(PC_OLD);
    // else PC_OLD += ADDR_SZ;
}
void fJMPNZ() {        // opcode #20
    // if (pop()) PC_OLD = addrAt(PC_OLD);
    // else PC_OLD += ADDR_SZ;
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
    N += T; pop();
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
        T = x / y;
        N = x % y;
    }
    else {
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
void fDOTQUOTE() {     // opcode #43
    N = N * T; push(T); pop();
}
void fPAREN() {        // opcode #44
    N = N * T; push(T); pop();
}
void fWDTFEED() {      // opcode #45
    N = N * T; push(T); pop();
}
void fBREAK() {        // opcode #46
    N = N * T; push(T); pop();
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

    to += (num - 1);
    from += (num - 1);

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
        dict[to++] = (BYTE)val;
        num--;
    }
}
void fOPENBLOCK() {    // opcode #50
    N = N * T; push(T); pop();
}
void fFILECLOSE() {    // opcode #51
    N = N * T; push(T); pop();
}
void fFILEREAD() {     // opcode #52
    N = N * T; push(T); pop();
}
void fLOAD() {         // opcode #53
    N = N * T; push(T); pop();
}
void fTHRU() {         // opcode #54
    N = N * T; push(T); pop();
}
// OP_DO (#55)    : DO ( f t -- ) ... ;
void fDO() {
    // if (loopDepth < 4) {
    //     CELL t = pop();
    //     CELL f = pop();
    //     int x = loopDepth * 3;
    //     // printStringF("-DO(%ld,%ld,%d)-", f, t, f);
    //     loopSTK[x] = f;
    //     loopSTK[x+1] = t;
    //     loopSTK[x+2] = f;
    //     ++loopDepth;
    // } else {
    //     printString("-DO:too deep-");
    // }
}
// OP_LOOP (#56)    : LOOP ( -- ) ... ;
void fLOOP() {
    // if (loopDepth > 0) {
    //     int x = (loopDepth-1) * 3;
    //     CELL f = loopSTK[x];
    //     CELL t = loopSTK[x+1];
    //     loopSTK[x+2] += 1;
    //     CELL i = loopSTK[x+2];
    //     // printString("-LOOP(%ld,%ld,%d)-", f, t, i);
    //     if ((f < i) && (i < t)) { push(1); return; }
    //     loopDepth -= 1;
    //     push(0);
    // }
    // else {
    //     printString("-LOOP:depthErr-");
    //     push(0);
    // }
}
// OP_LOOPP (#57)    : LOOP+ ( n -- ) ... ;
void fLOOPP() {
    // if (loopDepth > 0) {
    //     int x = (loopDepth-1) * 3;
    //     CELL f = loopSTK[x];
    //     CELL t = loopSTK[x+1];
    //     loopSTK[x+2] += pop();
    //     CELL i = loopSTK[x+2];
    //     // printStringF("-LOOP(%ld,%ld,%ld)-", f, t, i);
    //     if ((f < i) && (i < t)) { push(1); return; }
    //     if ((t < i) && (i < f)) { push(1); return; }
    //     loopDepth -= 1;
    //     push(0);
    // }
    // else {
    //     printString("-LOOP:depthErr-");
    //     push(0);
    // }
}
void fUNUSED7() {         // opcode #58
}
// ( a -- )
void fPARSEWORD() {    // opcode #59
    CELL wa = pop();
    char* w = (char*)&dict[wa];
    // printStringF("-pw[%s]-", w);
    push(wa); fFIND();
    if (pop()) {
        DICT_T* dp = (DICT_T*)&dict[T];
        fGETXT();
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
void fGETXT() {        // opcode #61
    DICT_T* dp = (DICT_T*)&dict[T];
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
    char* name = (char*)&dict[wa];
    sys->HERE = align2(sys->HERE);
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
// (a1 -- [a2 1] | 0)
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
void fNJMPZ() {        // opcode #68
    // if (T == 0) PC_OLD = addrAt(PC_OLD);
    // else PC_OLD += ADDR_SZ;
}
void fNJMPNZ() {       // opcode #69
    // if (T) PC_OLD = addrAt(PC_OLD);
    // else PC_OLD += ADDR_SZ;
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
        CELL x = (loopDepth - 1) * 3;
        push(loopSTK[x + 2]);
    }
    else {
        printString("-I:depthErr-");
        push(0);
    }
}
// OP_J (#74)    : J ( -- n ) ... ;
void fJ() {
    // if (loopDepth > 1) {
    //     CELL x = (loopDepth-2) * 3;
    //     push(loopSTK[x+2]);
    // }
    // else {
    //     printString("-J:depthErr-");
    //     push(0);
    // }
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
}
// OP_MCSTORE (#84)    : mc! ( n1 n2 -- ) ... ;
void fMCSTORE() {
    BYTE* a = (BYTE*)pop();
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





#include"defs.h"


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





#include "defs.h"

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
    if (strcmp_PF(w, PSTR(".\"")) == 0) return OP_DOTQUOTE;       //  opcode #43
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
    if (strcmp_PF(w, PSTR("-n-")) == 0) return OP_UNUSED7;       //  opcode #58
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





#include "defs.h"

void CCOMMA(BYTE v) { push(v); fCCOMMA(); }
void WCOMMA(WORD v) { push(v); fWCOMMA(); }
void COMMA(CELL v) { push(v); fCOMMA(); }
void ACOMMA(ADDR v) { push(v); fACOMMA(); }

CELL getXT(CELL addr) {
    push(addr);
    fGETXT();
    return pop();
}

CELL align4(CELL val) {
    push(val);
    fALIGN4();
    return pop();
}

CELL align2(CELL val) {
    push(val);
    fALIGN2();
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
    // printStringF(":%d-\n", (int)to);
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
    char* buf = (char*)&dict[sys->HERE + 16];
    sprintf(buf, ": ds $%p ;", &dict[0]);
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
    loadSource(PSTR(": led 13 ; led output"));
    loadSource(PSTR(": led-on 1 led dp! ; : led-off 0 led dp! ;"));
    loadSource(PSTR(": blink led-on dup ms led-off dup ms ;"));
    loadSource(PSTR(": k 1000 * ; : mil k k ;"));
    loadSource(PSTR(": blinks 0 swap do blink loop ;"));
    loadSource(PSTR("variable pot  3 pot ! "));
    loadSource(PSTR("variable but  6 but ! "));
    loadSource(PSTR(": init pot @ input but @ input ;"));
    loadSource(PSTR("variable pot-lv variable sens 4 sens !"));
    loadSource(PSTR(": but@ but @ dp@ ;"));
    loadSource(PSTR(": pot@ pot @ ap@ ;"));
    loadSource(PSTR(": bp->led but@ if led-on else led-off then ;"));
    loadSource(PSTR(": .pot? pot@ dup pot-lv @ - abs sens @ > if dup . cr pot-lv ! else drop then ;"));
    loadSource(PSTR(": go bp->led .pot? ;"));
    loadSource(PSTR("init // auto-run-last"));
    // loadSource(PSTR(""));
}

void dumpDict() {
    printStringF("%04x %04x (%ld %ld)", sys->HERE, sys->LAST, sys->HERE, sys->LAST);
    for (int i = 0; i < sys->HERE; i++) {
        if (i % 16 == 0) printStringF("\n %04x:", i);
        printStringF(" %02x", dict[i]);
    }
}

void ok() {
    printString(" ok. ");
    fDOTS();
    printString("\n");
}