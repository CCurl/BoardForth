#include "defs.h"

/* -- NimbleText script:
$once
// vvvvv - NimbleText generated - vvvvv
BYTE getOpcode(char *w) {
$each
    if (strcmp(w, F("$1")) == 0) return OP_$0;       //  opcode #$rownum
$once
    return 0xFF;
}
// ^^^^^ - NimbleText generated - ^^^^^
*/

// vvvvv - NimbleText generated - vvvvv
BYTE getOpcode(char *w) {
    if (strcmp(w, F("NOOP")) == 0) return OP_NOOP;       //  opcode #0
    if (strcmp(w, F("CLITERAL")) == 0) return OP_CLIT;       //  opcode #1
    if (strcmp(w, F("WLITERAL")) == 0) return OP_WLIT;       //  opcode #2
    if (strcmp(w, F("LITERAL")) == 0) return OP_LIT;       //  opcode #3
    if (strcmp(w, F("C@")) == 0) return OP_CFETCH;       //  opcode #4
    if (strcmp(w, F("W@")) == 0) return OP_WFETCH;       //  opcode #5
    if (strcmp(w, F("A@")) == 0) return OP_AFETCH;       //  opcode #6
    if (strcmp(w, F("@")) == 0) return OP_FETCH;       //  opcode #7
    if (strcmp(w, F("C!")) == 0) return OP_CSTORE;       //  opcode #8
    if (strcmp(w, F("W!")) == 0) return OP_WSTORE;       //  opcode #9
    if (strcmp(w, F("A!")) == 0) return OP_ASTORE;       //  opcode #10
    if (strcmp(w, F("!")) == 0) return OP_STORE;       //  opcode #11
    if (strcmp(w, F("C,")) == 0) return OP_CCOMMA;       //  opcode #12
    if (strcmp(w, F("W,")) == 0) return OP_WCOMMA;       //  opcode #13
    if (strcmp(w, F(",")) == 0) return OP_COMMA;       //  opcode #14
    if (strcmp(w, F("A,")) == 0) return OP_ACOMMA;       //  opcode #15
    if (strcmp(w, F("CALL")) == 0) return OP_CALL;       //  opcode #16
    if (strcmp(w, F("RET")) == 0) return OP_RET;       //  opcode #17
    // if (strcmp(w, F("-N-")) == 0) return OP_JMP;       //  opcode #18
    // if (strcmp(w, F("-N-")) == 0) return OP_JMPZ;       //  opcode #19
    // if (strcmp(w, F("-N-")) == 0) return OP_JMPNZ;       //  opcode #20
    if (strcmp(w, F("1-")) == 0) return OP_ONEMINUS;       //  opcode #21
    if (strcmp(w, F("1+")) == 0) return OP_ONEPLUS;       //  opcode #22
    if (strcmp(w, F("DUP")) == 0) return OP_DUP;       //  opcode #23
    if (strcmp(w, F("SWAP")) == 0) return OP_SWAP;       //  opcode #24
    if (strcmp(w, F("DROP")) == 0) return OP_DROP;       //  opcode #25
    if (strcmp(w, F("OVER")) == 0) return OP_OVER;       //  opcode #26
    if (strcmp(w, F("+")) == 0) return OP_ADD;       //  opcode #27
    if (strcmp(w, F("-")) == 0) return OP_SUB;       //  opcode #28
    if (strcmp(w, F("*")) == 0) return OP_MULT;       //  opcode #29
    if (strcmp(w, F("/MOD")) == 0) return OP_SLMOD;       //  opcode #30
    if (strcmp(w, F("<<")) == 0) return OP_LSHIFT;       //  opcode #31
    if (strcmp(w, F(">>")) == 0) return OP_RSHIFT;       //  opcode #32
    if (strcmp(w, F("AND")) == 0) return OP_AND;       //  opcode #33
    if (strcmp(w, F("OR")) == 0) return OP_OR;       //  opcode #34
    if (strcmp(w, F("XOR")) == 0) return OP_XOR;       //  opcode #35
    if (strcmp(w, F("NOT")) == 0) return OP_NOT;       //  opcode #36
    if (strcmp(w, F(">R")) == 0) return OP_DTOR;       //  opcode #37
    if (strcmp(w, F("R@")) == 0) return OP_RFETCH;       //  opcode #38
    if (strcmp(w, F("R>")) == 0) return OP_RTOD;       //  opcode #39
    if (strcmp(w, F("EMIT")) == 0) return OP_EMIT;       //  opcode #40
    if (strcmp(w, F(".")) == 0) return OP_DOT;       //  opcode #41
    if (strcmp(w, F(".S")) == 0) return OP_DOTS;       //  opcode #42
    if (strcmp(w, F(".\"")) == 0) return OP_DOTQUOTE;       //  opcode #43
    if (strcmp(w, F("(")) == 0) return OP_PAREN;       //  opcode #44
    if (strcmp(w, F("WDTFEED")) == 0) return OP_WDTFEED;       //  opcode #45
    if (strcmp(w, F("BRK")) == 0) return OP_BREAK;       //  opcode #46
    if (strcmp(w, F("CMOVE")) == 0) return OP_CMOVE;       //  opcode #47
    if (strcmp(w, F("CMOVE>")) == 0) return OP_CMOVE2;       //  opcode #48
    if (strcmp(w, F("FILL")) == 0) return OP_FILL;       //  opcode #49
    if (strcmp(w, F("OPEN-BLOCK")) == 0) return OP_OPENBLOCK;       //  opcode #50
    if (strcmp(w, F("FILE-CLOSE")) == 0) return OP_FILECLOSE;       //  opcode #51
    if (strcmp(w, F("FILE-READ")) == 0) return OP_FILEREAD;       //  opcode #52
    if (strcmp(w, F("LOAD")) == 0) return OP_LOAD;       //  opcode #53
    if (strcmp(w, F("THRU")) == 0) return OP_THRU;       //  opcode #54
    // if (strcmp(w, F("-n-")) == 0) return OP_UNUSED4;       //  opcode #55
    // if (strcmp(w, F("-n-")) == 0) return OP_UNUSED5;       //  opcode #56
    // if (strcmp(w, F("-n-")) == 0) return OP_UNUSED6;       //  opcode #57
    // if (strcmp(w, F("-n-")) == 0) return OP_UNUSED7;       //  opcode #58
    if (strcmp(w, F("PARSE-WORD")) == 0) return OP_PARSEWORD;       //  opcode #59
    if (strcmp(w, F("PARSE-LINE")) == 0) return OP_PARSELINE;       //  opcode #60
    if (strcmp(w, F(">BODY")) == 0) return OP_GETXT;       //  opcode #61
    if (strcmp(w, F("ALIGN2")) == 0) return OP_ALIGN2;       //  opcode #62
    if (strcmp(w, F("ALIGN4")) == 0) return OP_ALIGN4;       //  opcode #63
    if (strcmp(w, F("CREATE")) == 0) return OP_CREATE;       //  opcode #64
    if (strcmp(w, F("FIND")) == 0) return OP_FIND;       //  opcode #65
    if (strcmp(w, F("NEXT-WORD")) == 0) return OP_NEXTWORD;       //  opcode #66
    if (strcmp(w, F("NUMBER?")) == 0) return OP_ISNUMBER;       //  opcode #67
    // if (strcmp(w, F("-N-")) == 0) return OP_NJMPZ;       //  opcode #68
    // if (strcmp(w, F("-N-")) == 0) return OP_NJMPNZ;       //  opcode #69
    if (strcmp(w, F("<")) == 0) return OP_LESS;       //  opcode #70
    if (strcmp(w, F("=")) == 0) return OP_EQUALS;       //  opcode #71
    if (strcmp(w, F(">")) == 0) return OP_GREATER;       //  opcode #72
    if (strcmp(w, F("BYE")) == 0) return OP_BYE;       //  opcode #73
    return 0xFF;
}
// ^^^^^ - NimbleText generated - ^^^^^
