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
    f<%($0 + ',               ').substring(0,13)%>     // OP_$0 (#$rownum, $1)
$once
    0};
// ^^^^^ - NimbleText generated - ^^^^^
*/
// vvvvv - NimbleText generated - vvvvv
FP prims[] = {
    fNOOP,             // OP_NOOP (#0, noop)
    fCLIT,             // OP_CLIT (#1, cliteral)
    fWLIT,             // OP_WLIT (#2, wliteral)
    fLIT,              // OP_LIT (#3, literal)
    fCFETCH,           // OP_CFETCH (#4, c@)
    fWFETCH,           // OP_WFETCH (#5, w@)
    fAFETCH,           // OP_AFETCH (#6, a@)
    fFETCH,            // OP_FETCH (#7, @)
    fCSTORE,           // OP_CSTORE (#8, c!)
    fWSTORE,           // OP_WSTORE (#9, w!)
    fASTORE,           // OP_ASTORE (#10, a!)
    fSTORE,            // OP_STORE (#11, !)
    fCCOMMA,           // OP_CCOMMA (#12, c,)
    fWCOMMA,           // OP_WCOMMA (#13, w,)
    fCOMMA,            // OP_COMMA (#14, ,)
    fACOMMA,           // OP_ACOMMA (#15, a,)
    fCALL,             // OP_CALL (#16, call)
    fRET,              // OP_RET (#17, ret)
    fJMP,              // OP_JMP (#18, -n-)
    fJMPZ,             // OP_JMPZ (#19, -n-)
    fJMPNZ,            // OP_JMPNZ (#20, -n-)
    fONEMINUS,         // OP_ONEMINUS (#21, 1-)
    fONEPLUS,          // OP_ONEPLUS (#22, 1+)
    fDUP,              // OP_DUP (#23, dup)
    fSWAP,             // OP_SWAP (#24, swap)
    fDROP,             // OP_DROP (#25, drop)
    fOVER,             // OP_OVER (#26, over)
    fADD,              // OP_ADD (#27, +)
    fSUB,              // OP_SUB (#28, -)
    fMULT,             // OP_MULT (#29, *)
    fSLMOD,            // OP_SLMOD (#30, /mod)
    fLSHIFT,           // OP_LSHIFT (#31, <<)
    fRSHIFT,           // OP_RSHIFT (#32, >>)
    fAND,              // OP_AND (#33, and)
    fOR,               // OP_OR (#34, or)
    fXOR,              // OP_XOR (#35, xor)
    fNOT,              // OP_NOT (#36, not)
    fDTOR,             // OP_DTOR (#37, >r)
    fRFETCH,           // OP_RFETCH (#38, r@)
    fRTOD,             // OP_RTOD (#39, r>)
    fEMIT,             // OP_EMIT (#40, emit)
    fDOT,              // OP_DOT (#41, .)
    fDOTS,             // OP_DOTS (#42, .s)
    fDOTQUOTE,         // OP_DOTQUOTE (#43, .\")
    fPAREN,            // OP_PAREN (#44, ()
    fWDTFEED,          // OP_WDTFEED (#45, wdtfeed)
    fBREAK,            // OP_BREAK (#46, brk)
    fTIB,              // OP_TIB (#47, tib)
    fNTIB,             // OP_NTIB (#48, #tib)
    fTOIN,             // OP_TOIN (#49, >in)
    fOPENBLOCK,        // OP_OPENBLOCK (#50, open-block)
    fFILECLOSE,        // OP_FILECLOSE (#51, file-close)
    fFILEREAD,         // OP_FILEREAD (#52, file-read)
    fLOAD,             // OP_LOAD (#53, load)
    fTHRU,             // OP_THRU (#54, thru)
    fBASE,             // OP_BASE (#55, base)
    fSTATE,            // OP_STATE (#56, state)
    fHERE,             // OP_HERE (#57, (here))
    fLAST,             // OP_LAST (#58, (last))
    fPARSEWORD,        // OP_PARSEWORD (#59, parse-word)
    fPARSELINE,        // OP_PARSELINE (#60, parse-line)
    fGETXT,            // OP_GETXT (#61, get-xt)
    fALIGN2,           // OP_ALIGN2 (#62, align2)
    fALIGN4,           // OP_ALIGN4 (#63, align4)
    fCREATE,           // OP_CREATE (#64, create)
    fFIND,             // OP_FIND (#65, find)
    fNEXTWORD,         // OP_NEXTWORD (#66, next-word)
    fISNUMBER,         // OP_ISNUMBER (#67, number?)
    fNJMPZ,            // OP_NJMPZ (#68, -n-)
    fNJMPNZ,           // OP_NJMPNZ (#69, -n-)
    fLESS,             // OP_LESS (#70, <)
    fEQUALS,           // OP_EQUALS (#71, =)
    fGREATER,          // OP_GREATER (#72, >)
    fBYE,              // OP_BYE (#73, bye)
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
void fTIB() {          // opcode #47
    push(sys->TIB);
}
void fNTIB() {         // opcode #48
    N = N*T; push(T); pop();
}
void fTOIN() {         // opcode #49
    push(ADDR_TOIN);
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
void fBASE() {         // opcode #55
    push(ADDR_BASE);
}
void fSTATE() {        // opcode #56
    push(ADDR_STATE);
}
void fHERE() {         // opcode #57
    push(ADDR_HERE);
}
void fLAST() {         // opcode #58
    push(ADDR_LAST);
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

    if (strcmp(w, "if") == 0) {
        CCOMMA(OP_JMPZ);
        push(sys->HERE);
        ACOMMA(0);
        return;
    }

    if (strcmp(w, "if-") == 0) {
        CCOMMA(OP_NJMPZ);
        push(sys->HERE);
        ACOMMA(0);
        return;
    }

    if (strcmp(w, "then") == 0) {
        push(sys->HERE);
        fSWAP();
        fASTORE();
        return;
    }

    if (strcmp(w, "begin") == 0) {
        push(sys->HERE);
        return;
    }

    if (strcmp(w, "again") == 0) {
        CCOMMA(OP_JMP);
        fACOMMA();
        return;
    }

    if (strcmp(w, "while") == 0) {
        CCOMMA(OP_JMPNZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "until") == 0) {
        CCOMMA(OP_JMPZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "while-") == 0) {
        CCOMMA(OP_NJMPNZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "until-") == 0) {
        CCOMMA(OP_NJMPZ);
        fACOMMA();
        return;
    }

    if (strcmp(w, "variable") == 0) {
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
    N = (N < T) ? 1 : 0; pop();
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


