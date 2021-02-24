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
BYTE getOpcode(char *w) {
    if (strcmp_PF(w, PSTR("NOOP")) == 0) return OP_NOOP;       //  opcode #0
    if (strcmp_PF(w, PSTR("CLITERAL")) == 0) return OP_CLIT;       //  opcode #1
    if (strcmp_PF(w, PSTR("WLITERAL")) == 0) return OP_WLIT;       //  opcode #2
    if (strcmp_PF(w, PSTR("LITERAL")) == 0) return OP_LIT;       //  opcode #3
    if (strcmp_PF(w, PSTR("C@")) == 0) return OP_CFETCH;       //  opcode #4
    if (strcmp_PF(w, PSTR("W@")) == 0) return OP_WFETCH;       //  opcode #5
    if (strcmp_PF(w, PSTR("A@")) == 0) return OP_AFETCH;       //  opcode #6
    if (strcmp_PF(w, PSTR("@")) == 0) return OP_FETCH;       //  opcode #7
    if (strcmp_PF(w, PSTR("C!")) == 0) return OP_CSTORE;       //  opcode #8
    if (strcmp_PF(w, PSTR("W!")) == 0) return OP_WSTORE;       //  opcode #9
    if (strcmp_PF(w, PSTR("A!")) == 0) return OP_ASTORE;       //  opcode #10
    if (strcmp_PF(w, PSTR("!")) == 0) return OP_STORE;       //  opcode #11
    if (strcmp_PF(w, PSTR("C,")) == 0) return OP_CCOMMA;       //  opcode #12
    if (strcmp_PF(w, PSTR("W,")) == 0) return OP_WCOMMA;       //  opcode #13
    if (strcmp_PF(w, PSTR(",")) == 0) return OP_COMMA;       //  opcode #14
    if (strcmp_PF(w, PSTR("A,")) == 0) return OP_ACOMMA;       //  opcode #15
    if (strcmp_PF(w, PSTR("CALL")) == 0) return OP_CALL;       //  opcode #16
    if (strcmp_PF(w, PSTR("EXIT")) == 0) return OP_RET;       //  opcode #17
    if (strcmp_PF(w, PSTR("-N-")) == 0) return OP_JMP;       //  opcode #18
    if (strcmp_PF(w, PSTR("-N-")) == 0) return OP_JMPZ;       //  opcode #19
    if (strcmp_PF(w, PSTR("-N-")) == 0) return OP_JMPNZ;       //  opcode #20
    if (strcmp_PF(w, PSTR("1-")) == 0) return OP_ONEMINUS;       //  opcode #21
    if (strcmp_PF(w, PSTR("1+")) == 0) return OP_ONEPLUS;       //  opcode #22
    if (strcmp_PF(w, PSTR("DUP")) == 0) return OP_DUP;       //  opcode #23
    if (strcmp_PF(w, PSTR("SWAP")) == 0) return OP_SWAP;       //  opcode #24
    if (strcmp_PF(w, PSTR("DROP")) == 0) return OP_DROP;       //  opcode #25
    if (strcmp_PF(w, PSTR("OVER")) == 0) return OP_OVER;       //  opcode #26
    if (strcmp_PF(w, PSTR("+")) == 0) return OP_ADD;       //  opcode #27
    if (strcmp_PF(w, PSTR("-")) == 0) return OP_SUB;       //  opcode #28
    if (strcmp_PF(w, PSTR("*")) == 0) return OP_MULT;       //  opcode #29
    if (strcmp_PF(w, PSTR("/MOD")) == 0) return OP_SLMOD;       //  opcode #30
    if (strcmp_PF(w, PSTR("<<")) == 0) return OP_LSHIFT;       //  opcode #31
    if (strcmp_PF(w, PSTR(">>")) == 0) return OP_RSHIFT;       //  opcode #32
    if (strcmp_PF(w, PSTR("AND")) == 0) return OP_AND;       //  opcode #33
    if (strcmp_PF(w, PSTR("OR")) == 0) return OP_OR;       //  opcode #34
    if (strcmp_PF(w, PSTR("XOR")) == 0) return OP_XOR;       //  opcode #35
    if (strcmp_PF(w, PSTR("NOT")) == 0) return OP_NOT;       //  opcode #36
    if (strcmp_PF(w, PSTR(">R")) == 0) return OP_DTOR;       //  opcode #37
    if (strcmp_PF(w, PSTR("R@")) == 0) return OP_RFETCH;       //  opcode #38
    if (strcmp_PF(w, PSTR("R>")) == 0) return OP_RTOD;       //  opcode #39
    if (strcmp_PF(w, PSTR("EMIT")) == 0) return OP_EMIT;       //  opcode #40
    if (strcmp_PF(w, PSTR(".")) == 0) return OP_DOT;       //  opcode #41
    if (strcmp_PF(w, PSTR(".S")) == 0) return OP_DOTS;       //  opcode #42
    if (strcmp_PF(w, PSTR(".\"")) == 0) return OP_DOTQUOTE;       //  opcode #43
    if (strcmp_PF(w, PSTR("(")) == 0) return OP_PAREN;       //  opcode #44
    if (strcmp_PF(w, PSTR("WDTFEED")) == 0) return OP_WDTFEED;       //  opcode #45
    if (strcmp_PF(w, PSTR("BRK")) == 0) return OP_BREAK;       //  opcode #46
    if (strcmp_PF(w, PSTR("CMOVE")) == 0) return OP_CMOVE;       //  opcode #47
    if (strcmp_PF(w, PSTR("CMOVE>")) == 0) return OP_CMOVE2;       //  opcode #48
    if (strcmp_PF(w, PSTR("FILL")) == 0) return OP_FILL;       //  opcode #49
    if (strcmp_PF(w, PSTR("OPEN-BLOCK")) == 0) return OP_OPENBLOCK;       //  opcode #50
    if (strcmp_PF(w, PSTR("FILE-CLOSE")) == 0) return OP_FILECLOSE;       //  opcode #51
    if (strcmp_PF(w, PSTR("FILE-READ")) == 0) return OP_FILEREAD;       //  opcode #52
    if (strcmp_PF(w, PSTR("LOAD")) == 0) return OP_LOAD;       //  opcode #53
    if (strcmp_PF(w, PSTR("THRU")) == 0) return OP_THRU;       //  opcode #54
    if (strcmp_PF(w, PSTR("DO")) == 0) return OP_DO;       //  opcode #55
    if (strcmp_PF(w, PSTR("LOOP")) == 0) return OP_LOOP;       //  opcode #56
    if (strcmp_PF(w, PSTR("LOOP+")) == 0) return OP_LOOPP;       //  opcode #57
    if (strcmp_PF(w, PSTR("-n-")) == 0) return OP_UNUSED7;       //  opcode #58
    if (strcmp_PF(w, PSTR("PARSE-WORD")) == 0) return OP_PARSEWORD;       //  opcode #59
    if (strcmp_PF(w, PSTR("PARSE-LINE")) == 0) return OP_PARSELINE;       //  opcode #60
    if (strcmp_PF(w, PSTR(">BODY")) == 0) return OP_GETXT;       //  opcode #61
    if (strcmp_PF(w, PSTR("ALIGN2")) == 0) return OP_ALIGN2;       //  opcode #62
    if (strcmp_PF(w, PSTR("ALIGN4")) == 0) return OP_ALIGN4;       //  opcode #63
    if (strcmp_PF(w, PSTR("CREATE")) == 0) return OP_CREATE;       //  opcode #64
    if (strcmp_PF(w, PSTR("FIND")) == 0) return OP_FIND;       //  opcode #65
    if (strcmp_PF(w, PSTR("NEXT-WORD")) == 0) return OP_NEXTWORD;       //  opcode #66
    if (strcmp_PF(w, PSTR("NUMBER?")) == 0) return OP_ISNUMBER;       //  opcode #67
    if (strcmp_PF(w, PSTR("-N-")) == 0) return OP_NJMPZ;       //  opcode #68
    if (strcmp_PF(w, PSTR("-N-")) == 0) return OP_NJMPNZ;       //  opcode #69
    if (strcmp_PF(w, PSTR("<")) == 0) return OP_LESS;       //  opcode #70
    if (strcmp_PF(w, PSTR("=")) == 0) return OP_EQUALS;       //  opcode #71
    if (strcmp_PF(w, PSTR(">")) == 0) return OP_GREATER;       //  opcode #72
    if (strcmp_PF(w, PSTR("I")) == 0) return OP_I;       //  opcode #73
    if (strcmp_PF(w, PSTR("J")) == 0) return OP_J;       //  opcode #74
    if (strcmp_PF(w, PSTR("input-pin")) == 0) return OP_INPUTPIN;       //  opcode #75
    if (strcmp_PF(w, PSTR("output-pin")) == 0) return OP_OUTPUTPIN;       //  opcode #76
    if (strcmp_PF(w, PSTR("MS")) == 0) return OP_DELAY;       //  opcode #77
    if (strcmp_PF(w, PSTR("TICK")) == 0) return OP_TICK;       //  opcode #78
    if (strcmp_PF(w, PSTR("BYE")) == 0) return OP_BYE;       //  opcode #79
    return 0xFF;
}
// ^^^^^ - NimbleText generated - ^^^^^