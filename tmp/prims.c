#include "defs.h"

CELL *dstk;
CELL *rstk;

BYTE IR;
CELL PC;
BYTE dict[DICT_SZ];
SYSVARS_T *sys;

void run(CELL start, CELL max_cycles) {
    PC = start;
    // printf("\nrun: %d (%04lx), %d cycles ... ", PC, PC, max_cycles);
    while (1) {
        BYTE IR = dict[PC++];
        if (IR == OP_RET) {
            if (sys->RSP < 1) { return; }
            PC = rpop();
        } else if (IR <= OP_BYE) {
            prims[IR]();
            if (IR == OP_BYE) { return; }
        } else {
            printf("-%04lx: unknown opcode: %d ($%02x)-", PC-1, IR, IR);
        }
        if (max_cycles) {
            if (--max_cycles < 1) { return; }
        }
    }
}

void push(CELL v) { sys->DSP = (sys->DSP < STK_SZ) ? sys->DSP+1 : STK_SZ; T = v; }
CELL pop() { sys->DSP = (sys->DSP > 0) ? sys->DSP-1 : 0; return dstk[sys->DSP+1]; }

void rpush(CELL v) { sys->RSP = (sys->RSP < STK_SZ) ? sys->RSP+1 : STK_SZ; R = v; }
CELL rpop() { sys->RSP = (sys->RSP > 0) ? sys->RSP-1 : 0; return rstk[sys->RSP+1]; }


CELL cellAt(CELL loc) {
    CELL x = dict[loc++];
    x += (dict[loc++] <<  8);
    x += (dict[loc++] << 16);
    x += (dict[loc++] << 24);
    return x;
}

CELL wordAt(CELL loc) {
    CELL x = dict[loc++];
    x += (dict[loc] <<  8);
    return x;
}

CELL addrAt(CELL loc) {         // opcode #16
    return (ADDR_SZ == 2) ? wordAt(loc) : cellAt(loc);
}

void wordStore(CELL addr, CELL val) {
    // printf("-w! %ld to [%ld]-", val, addr);
    if ((0 <= addr) && ((addr+2) < DICT_SZ)) {
        dict[addr++] = (val & 0xFF);
        dict[addr++] = (val >>  8) & 0xFF;
    }
}
void cellStore(CELL addr, CELL val) {
    if ((0 <= addr) && ((addr+4) < DICT_SZ)) {
        dict[addr++] = (val & 0xFF);
        dict[addr++] = (val >>  8) & 0xFF;
        dict[addr++] = (val >> 16) & 0xFF;
        dict[addr++] = (val >> 24) & 0xFF;
    }
}
void addrStore(CELL addr, CELL val) {
    (ADDR_SZ == 2) ? wordStore(addr, val) : cellStore(addr, val);
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
    fRET,              // OP_RET (#17) ***RET RET ( -- )***
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
    fDOT,              // OP_DOT (#41) ***DOT . (N -- )***
    fDOTS,             // OP_DOTS (#42) ***DOTS .S ( -- )***
    fDOTQUOTE,         // OP_DOTQUOTE (#43) ***DOTQUOTE .\" ( -- )***
    fPAREN,            // OP_PAREN (#44) ***PAREN ( ( -- )***
    fWDTFEED,          // OP_WDTFEED (#45) ***WDTFEED WDTFEED ( -- )***
    fBREAK,            // OP_BREAK (#46) ***BREAK BRK ( -- )***
    fNOOP,             // OP_UNUSED1 (#48) ***xxx ( -- )***
    fNOOP,             // OP_UNUSED1 (#48) ***xxx ( -- )***
    fNOOP,             // OP_UNUSED1 (#49) ***xxx ( -- )***
    fOPENBLOCK,        // OP_OPENBLOCK (#50) ***OPENBLOCK OPEN-BLOCK***
    fFILECLOSE,        // OP_FILECLOSE (#51) ***FILECLOSE FILE-CLOSE***
    fFILEREAD,         // OP_FILEREAD (#52) ***FILEREAD FILE-READ***
    fLOAD,             // OP_LOAD (#53) ***LOAD LOAD***
    fTHRU,             // OP_THRU (#54) ***THRU THRU***
    fNOOP,             // OP_UNUSED1 (#55) ***xxx ( -- )***
    fNOOP,             // OP_UNUSED1 (#56) ***xxx ( -- )***
    fNOOP,             // OP_UNUSED1 (#57) ***xxx ( -- )***
    fNOOP,             // OP_UNUSED2 (#58) ***xxx ( -- )***
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
    fBYE,              // OP_BYE (#73) ***BYE BYE ( -- )***
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
    printf("Invalid address: %ld ($%04lX)", addr, addr);
}
void fWFETCH() {       // opcode #5
    CELL addr = T;
    if ((0 <= addr) && ((addr+2) < DICT_SZ)) {
        T = wordAt(addr);
        return;
    }
    printf("Invalid address: %ld ($%04lX)", addr, addr);
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

    switch (addr)
    {
        case DIGITAL_PIN_BASE: printf("-@DP_base-"); break;
        case ANALOG_PIN_BASE:  printf("-@AP_base-"); break;

    default:
        printf("Invalid address: %ld ($%04lX)", addr, addr);
    }
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

    switch (addr)
    {
        case DIGITAL_PIN_BASE: printf("-!DP_base-"); break;
        case ANALOG_PIN_BASE:  printf("-!AP_base-"); break;
    
    default:
        printf("Invalid address: %ld ($%04lX)", addr, addr);
    }
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
    // printf("-call:%lx-", PC);
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
        printf("divide by 0!");
    }
}
void fLSHIFT() {       // opcode #31
    T = T << 1;
}
void fRSHIFT() {       // opcode #32
    T = T >> 1;
}
void fAND() {          // opcode #33
    N &= T;
}
void fOR() {           // opcode #34
    N |= T;
}
void fXOR() {          // opcode #35
    N ^= T;
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
    printf("%c", (char)pop());
}
void fDOT() {
    CELL v = pop();
    if (sys->BASE == 10) {
        printf(" %ld", v);
    } else if (sys->BASE == 16) {
        printf(" %lx", v);
    } else {
        printf(" %ld (in %ld)", v, sys->BASE);
    }
}
void fDOTS() {
    if (sys->DSP) {
        push('('); fEMIT();
        for (int i = 1; i <= sys->DSP; i++) {
            push(dstk[i]);
            fDOT();
        }
        push(' '); fEMIT();
        push(')'); fEMIT();
    } else {
        push('('); fEMIT();
        push(237); fEMIT();
        push(')'); fEMIT();
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
void fUNUSED1() {          // opcode #47
}
void fUNUSED2() {         // opcode #48
}
void fUNUSED3() {         // opcode #49
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
void fUNUSED4() {         // opcode #55
}
void fUNUSED5() {        // opcode #56
}
void fUNUSED6() {         // opcode #57
}
void fUNUSED7() {         // opcode #58
}
// ( a -- )
void fPARSEWORD() {    // opcode #59
    CELL wa = pop();
    char *w = &dict[wa];
    // printf("-pw[%s]-", w);
    push(wa); fFIND();
    if (pop()) {
        DICT_T *dp = (DICT_T *)&dict[T];
        fGETXT();
        CELL xt = pop();
        if ((sys->STATE == 1) && (dp->flags == 0)) {
            CCOMMA(OP_CALL);
            ACOMMA((ADDR)xt);
        } else {
            run(xt, 0);
        }
        return;
    }

    push(wa); fISNUMBER();
    if (pop()) {
        if (sys->STATE == 1) {
            if (T < 0x0100) {
                CCOMMA(OP_CLIT);
                fCCOMMA();
            } else if (T < 0x010000) {
                CCOMMA(OP_WLIT);
                fWCOMMA();
            } else {
                CCOMMA(OP_LIT);
                fCOMMA();
            }
        }
        return;
    }

    if (strcmp(w, ":") == 0) {
        push(wa);
        fNEXTWORD();
        if (pop()) {
            push(wa);
            fCREATE();
            sys->STATE = 1;
        }
        return;
    }

    if (strcmp(w, ";") == 0) {
        CCOMMA(OP_RET);
        sys->STATE = 0;
        return;
    }

    if (strcmp(w, "IF") == 0) {
        CCOMMA(OP_JMPZ);
        push(sys->HERE);
        ACOMMA(0);
        return;
    }

    if (strcmp(w, "IF-") == 0) {
        CCOMMA(OP_NJMPZ);
        push(sys->HERE);
        ACOMMA(0);
        return;
    }

    if (strcmp(w, "ELSE") == 0) {
        CCOMMA(OP_JMP);
        push(sys->HERE);
        fSWAP();
        ACOMMA(0);
        push(sys->HERE);
        fSWAP();
        fASTORE();
        return;
    }

    if (strcmp(w, "THEN") == 0) {
        push(sys->HERE);
        fSWAP();
        fASTORE();
        return;
    }

    if (strcmp(w, "BEGIN") == 0) {
        push(sys->HERE);
        return;
    }

    if (strcmp(w, "AGAIN") == 0) {
        CCOMMA(OP_JMP);
        fACOMMA();
        return;
    }

    if (strcmp(w, "WHILE") == 0) {
        CCOMMA(OP_JMPNZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "UNTIL") == 0) {
        CCOMMA(OP_JMPZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "WHILE-") == 0) {
        CCOMMA(OP_NJMPNZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "UNTIL-") == 0) {
        CCOMMA(OP_NJMPZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "VARIABLE") == 0) {
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

    if (strcmp(w, "CONSTANT") == 0) {
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
        if (sys->STATE == 1) {
            CCOMMA(op);
        } else {
            CELL xt = sys->HERE+24;
            dict[xt] = op;
            dict[xt+1] = OP_RET;
            run(xt, 0);
        }
        return;
    }
    printf("[%s]??", w);
}
void fPARSELINE() {    // opcode #60
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
    // printf("-define [%s] at %d (%lx)", name, sys->HERE, sys->HERE);

    DICT_T *dp = (DICT_T *)&dict[sys->HERE];
    dp->prev = (ADDR)sys->LAST;
    dp->flags = 0;
    dp->len = strlen(name);
    strcpy(dp->name, name);
    sys->LAST = sys->HERE;
    sys->HERE += ADDR_SZ + dp->len + 3;
    // printf(",XT:%d (%lx)-", sys->HERE, sys->HERE);
}
// (a1 -- [a2 1] | 0)
void fFIND() {         // opcode #65
    char *name = (char *)&dict[pop()];
    // printf("-lf:[%s]-", name);
    CELL cl = sys->LAST;
    while (cl) {
        DICT_T *dp = (DICT_T *)&dict[cl];
        if (strcmp(name, dp->name) == 0) {
            // printf("-FOUND! (%lx)-", cl);
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
    char *w = &dict[wa];

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
void fBYE() {          // opcode #70
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


