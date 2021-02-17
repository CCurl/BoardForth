#include "defs.h"

void CCOMMA(BYTE v) {
    push(v);
    fCCOMMA();
}
void WCOMMA(WORD v) {
    push(v);
    fWCOMMA();
}

void COMMA(CELL v) {
    push(v);
    fCOMMA();
}

void ACOMMA(ADDR v) {
    push(v);
    fCOMMA();
}

// vvvvv -- NimbleText generated -- vvvvv
FP prims[] = {
    fNOOP,             // opcode #0
    fCLIT,             // opcode #1
    fWLIT,             // opcode #2
    fLIT,              // opcode #3
    fCFETCH,           // opcode #4
    fWFETCH,           // opcode #5
    fAFETCH,           // opcode #6
    fFETCH,            // opcode #7
    fCSTORE,           // opcode #8
    fWSTORE,           // opcode #9
    fASTORE,           // opcode #10
    fSTORE,            // opcode #11
    fCCOMMA,           // opcode #12
    fWCOMMA,           // opcode #13
    fCOMMA,            // opcode #14
    fACOMMA,           // opcode #15
    fCALL,             // opcode #16
    fRET,              // opcode #17
    fJMP,              // opcode #18
    fJMPZ,             // opcode #19
    fJMPNZ,            // opcode #20
    fONEMINUS,         // opcode #21
    fONEPLUS,          // opcode #22
    fDUP,              // opcode #23
    fSWAP,             // opcode #24
    fDROP,             // opcode #25
    fOVER,             // opcode #26
    fADD,              // opcode #27
    fSUB,              // opcode #28
    fMULT,             // opcode #29
    fDIV,              // opcode #30
    fLSHIFT,           // opcode #31
    fRSHIFT,           // opcode #32
    fAND,              // opcode #33
    fOR,               // opcode #34
    fXOR,              // opcode #35
    fNOT,              // opcode #36
    fDTOR,             // opcode #37
    fRFETCH,           // opcode #38
    fRTOD,             // opcode #39
    fEMIT,             // opcode #40
    fDOT,              // opcode #41
    fDOTS,             // opcode #42
    fDOTQUOTE,         // opcode #43
    fPAREN,            // opcode #44
    fWDTFEED,          // opcode #45
    fBREAK,            // opcode #46
    fTIB,              // opcode #47
    fNTIB,             // opcode #48
    fTOIN,             // opcode #49
    fOPENBLOCK,        // opcode #50
    fFILECLOSE,        // opcode #51
    fFILEREAD,         // opcode #52
    fLOAD,             // opcode #53
    fTHRU,             // opcode #54
    fBASE,             // opcode #55
    fSTATE,            // opcode #56
    fHERE,             // opcode #57
    fLAST,             // opcode #58
    0 };
// ^^^^^ -- NimbleText generated -- ^^^^^

void fNOOP() {         // opcode #0
}
void fCLIT() {         // opcode #1
    push(dict[PC++]);
}
void fWLIT() {         // opcode #2
    CELL x = dict[PC++];
    x = (dict[PC++] << 8) + x;
    push(x);
}
void fLIT() {          // opcode #3
    CELL x = dict[PC++];
    x = (dict[PC++] << 8) + x;
    x = (dict[PC++] << 16) + x;
    x = (dict[PC++] << 24) + x;
    push(x);
}
void fCFETCH() {       // opcode #4
    N = N*T; push(T); pop();
}
void fWFETCH() {       // opcode #5
    N = N*T; push(T); pop();
}
void fAFETCH() {       // opcode #6
    N = N*T; push(T); pop();
}
void fFETCH() {        // opcode #7
    N = N*T; push(T); pop();
}
void fCSTORE() {       // opcode #8
    N = N*T; push(T); pop();
}
void fWSTORE() {       // opcode #9
    N = N*T; push(T); pop();
}
void fASTORE() {       // opcode #10
    N = N*T; push(T); pop();
}
void fSTORE() {        // opcode #11
    N = N*T; push(T); pop();
}
void fCCOMMA() {       // opcode #12
    dict[HERE++] = (BYTE)pop();
}
void fWCOMMA() {       // opcode #13
    WORD x = (WORD)pop();
    dict[HERE++] = (BYTE)(x & 0xFF);
    dict[HERE++] = (BYTE)((x>>8) & 0xFF);
}
void fCOMMA() {        // opcode #14
    CELL x = (CELL)pop();
    dict[HERE++] = (BYTE)( x        & 0xFF);
    dict[HERE++] = (BYTE)((x >>  8) & 0xFF);
    dict[HERE++] = (BYTE)((x >> 16) & 0xFF);
    dict[HERE++] = (BYTE)((x >> 24) & 0xFF);
}
void fACOMMA() {       // opcode #15
    (ADDR_SZ == 2) ? fWCOMMA() : fCOMMA();
}
void fCALL() {         // opcode #16
    N = N*T; push(T); pop();
}
void fRET() {          // opcode #17
    N = N*T; push(T); pop();
}
void fJMP() {          // opcode #18
    N = N*T; push(T); pop();
}
void fJMPZ() {         // opcode #19
    N = N*T; push(T); pop();
}
void fJMPNZ() {        // opcode #20
    N = N*T; push(T); pop();
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
    N = N*T; push(T); pop();
}
void fDROP() {         // opcode #25
    pop();
}
void fOVER() {         // opcode #26
    push(N);
}
void fADD() {          // opcode #27
    N = N*T; push(T); pop();
}
void fSUB() {          // opcode #28
    N = N*T; push(T); pop();
}
void fMULT() {         // opcode #29
    N = N*T; push(T); pop();
}
void fDIV() {          // opcode #30
    N = N*T; push(T); pop();
}
void fLSHIFT() {       // opcode #31
    N = N*T; push(T); pop();
}
void fRSHIFT() {       // opcode #32
    N = N*T; push(T); pop();
}
void fAND() {          // opcode #33
    N = N*T; push(T); pop();
}
void fOR() {           // opcode #34
    N = N*T; push(T); pop();
}
void fXOR() {          // opcode #35
    N = N*T; push(T); pop();
}
void fNOT() {          // opcode #36
    N = N*T; push(T); pop();
}
void fDTOR() {         // opcode #37
    N = N*T; push(T); pop();
}
void fRFETCH() {       // opcode #38
    N = N*T; push(T); pop();
}
void fRTOD() {         // opcode #39
    N = N*T; push(T); pop();
}
void fEMIT() {
    printf("%c", (char)pop());
}
void fDOT() {
    printf(" %ld", pop());
}
void fDOTS() {
    if (DSP) {
        push('('); fEMIT();
        for (int i = 1; i <= DSP; i++) {
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
    N = N*T; push(T); pop();
}
void fNTIB() {         // opcode #48
    N = N*T; push(T); pop();
}
void fTOIN() {         // opcode #49
    N = N*T; push(T); pop();
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
    N = N*T; push(T); pop();
}
void fSTATE() {        // opcode #56
    N = N*T; push(T); pop();
}
void fHERE() {         // opcode #57
    N = N*T; push(T); pop();
}
void fLAST() {         // opcode #58
    N = N*T; push(T); pop();
}
