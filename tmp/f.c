#include "defs.h"

#pragma region allocation
ALLOC_T alloced[ALLOC_SZ];
int num_alloced = 0;
CELL curFree;

void allocDump() {
    printf("\nAlloc table (sz %d, %d used)", ALLOC_SZ, num_alloced);
    for (int i = 0; i < num_alloced; i++) {
        printf("\n%2d %04lx %4d %s", i, alloced[i].addr, (int)alloced[i].sz, (int)alloced[i].available ? "available" : "in-use");
    }
}

int allocFind(CELL addr) {
    for (int i = 0; i < num_alloced; i++) {
        if (alloced[i].addr == addr) return i;
    }
    return -1;
}

void allocFree(CELL addr) {
    int x = allocFind(addr);
    if (x >= 0) {
        alloced[x].available = 1;
    }
}

void allocFreeAll() {
    curFree = ADDR_ALLOC_BASE;
    for (int i = 0; i < ALLOC_SZ; i++) alloced[i].available = 1;
    num_alloced = 0;
}

int allocFindAvailable(WORD sz) {
    for (int i = 0; i < num_alloced; i++) {
        if ((alloced[i].available) && (alloced[i].sz >= sz)) return i;
    }
    return -1;
}

CELL allocSpace(WORD sz) {
    int x = allocFindAvailable(sz);
    if (x >= 0) {
        alloced[x].available = 0;
        return alloced[x].addr;
    }
    curFree -= (sz);
    if (num_alloced < ALLOC_SZ) {
        alloced[num_alloced].addr = curFree;
        alloced[num_alloced].sz = sz;
        alloced[num_alloced++].available = 0;
    } else {
        printf("-alloc tbl too small-");
    }
    return curFree;
}
#pragma endregion

void CCOMMA(BYTE v) { push(v); fCCOMMA(); }
void WCOMMA(WORD v) { push(v); fWCOMMA(); }
void COMMA(CELL v)  { push(v); fCOMMA();  }
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

// vvvvv - NimbleText generated - vvvvv
BYTE getOpcode(char *w) {
    if (strcmp(w, F("noop")) == 0) return OP_NOOP;       //  opcode #0
    if (strcmp(w, F("cliteral")) == 0) return OP_CLIT;       //  opcode #1
    if (strcmp(w, F("wliteral")) == 0) return OP_WLIT;       //  opcode #2
    if (strcmp(w, F("literal")) == 0) return OP_LIT;       //  opcode #3
    if (strcmp(w, F("c@")) == 0) return OP_CFETCH;       //  opcode #4
    if (strcmp(w, F("w@")) == 0) return OP_WFETCH;       //  opcode #5
    if (strcmp(w, F("a@")) == 0) return OP_AFETCH;       //  opcode #6
    if (strcmp(w, F("@")) == 0) return OP_FETCH;       //  opcode #7
    if (strcmp(w, F("c!")) == 0) return OP_CSTORE;       //  opcode #8
    if (strcmp(w, F("w!")) == 0) return OP_WSTORE;       //  opcode #9
    if (strcmp(w, F("a!")) == 0) return OP_ASTORE;       //  opcode #10
    if (strcmp(w, F("!")) == 0) return OP_STORE;       //  opcode #11
    if (strcmp(w, F("c,")) == 0) return OP_CCOMMA;       //  opcode #12
    if (strcmp(w, F("w,")) == 0) return OP_WCOMMA;       //  opcode #13
    if (strcmp(w, F(",")) == 0) return OP_COMMA;       //  opcode #14
    if (strcmp(w, F("a,")) == 0) return OP_ACOMMA;       //  opcode #15
    if (strcmp(w, F("call")) == 0) return OP_CALL;       //  opcode #16
    if (strcmp(w, F("ret")) == 0) return OP_RET;       //  opcode #17
    // if (strcmp(w, F("-n-")) == 0) return OP_JMP;       //  opcode #18
    // if (strcmp(w, F("-n-")) == 0) return OP_JMPZ;       //  opcode #19
    // if (strcmp(w, F("-n-")) == 0) return OP_JMPNZ;       //  opcode #20
    if (strcmp(w, F("1-")) == 0) return OP_ONEMINUS;       //  opcode #21
    if (strcmp(w, F("1+")) == 0) return OP_ONEPLUS;       //  opcode #22
    if (strcmp(w, F("dup")) == 0) return OP_DUP;       //  opcode #23
    if (strcmp(w, F("swap")) == 0) return OP_SWAP;       //  opcode #24
    if (strcmp(w, F("drop")) == 0) return OP_DROP;       //  opcode #25
    if (strcmp(w, F("over")) == 0) return OP_OVER;       //  opcode #26
    if (strcmp(w, F("+")) == 0) return OP_ADD;       //  opcode #27
    if (strcmp(w, F("-")) == 0) return OP_SUB;       //  opcode #28
    if (strcmp(w, F("*")) == 0) return OP_MULT;       //  opcode #29
    if (strcmp(w, F("/mod")) == 0) return OP_SLMOD;       //  opcode #30
    if (strcmp(w, F("2*")) == 0) return OP_LSHIFT;       //  opcode #31
    if (strcmp(w, F("2/")) == 0) return OP_RSHIFT;       //  opcode #32
    if (strcmp(w, F("and")) == 0) return OP_AND;       //  opcode #33
    if (strcmp(w, F("or")) == 0) return OP_OR;       //  opcode #34
    if (strcmp(w, F("xor")) == 0) return OP_XOR;       //  opcode #35
    if (strcmp(w, F("not")) == 0) return OP_NOT;       //  opcode #36
    if (strcmp(w, F(">r")) == 0) return OP_DTOR;       //  opcode #37
    if (strcmp(w, F("r@")) == 0) return OP_RFETCH;       //  opcode #38
    if (strcmp(w, F("r>")) == 0) return OP_RTOD;       //  opcode #39
    if (strcmp(w, F("emit")) == 0) return OP_EMIT;       //  opcode #40
    if (strcmp(w, F(".")) == 0) return OP_DOT;       //  opcode #41
    if (strcmp(w, F(".s")) == 0) return OP_DOTS;       //  opcode #42
    if (strcmp(w, F(".\"")) == 0) return OP_DOTQUOTE;       //  opcode #43
    if (strcmp(w, F("(")) == 0) return OP_PAREN;       //  opcode #44
    if (strcmp(w, F("wdtfeed")) == 0) return OP_WDTFEED;       //  opcode #45
    if (strcmp(w, F("brk")) == 0) return OP_BREAK;       //  opcode #46
    if (strcmp(w, F("tib")) == 0) return OP_TIB;       //  opcode #47
    if (strcmp(w, F("#tib")) == 0) return OP_NTIB;       //  opcode #48
    if (strcmp(w, F(">in")) == 0) return OP_TOIN;       //  opcode #49
    if (strcmp(w, F("open-block")) == 0) return OP_OPENBLOCK;       //  opcode #50
    if (strcmp(w, F("file-close")) == 0) return OP_FILECLOSE;       //  opcode #51
    if (strcmp(w, F("file-read")) == 0) return OP_FILEREAD;       //  opcode #52
    if (strcmp(w, F("load")) == 0) return OP_LOAD;       //  opcode #53
    if (strcmp(w, F("thru")) == 0) return OP_THRU;       //  opcode #54
    if (strcmp(w, F("base")) == 0) return OP_BASE;       //  opcode #55
    if (strcmp(w, F("state")) == 0) return OP_STATE;       //  opcode #56
    if (strcmp(w, F("(here)")) == 0) return OP_HERE;       //  opcode #57
    if (strcmp(w, F("(last)")) == 0) return OP_LAST;       //  opcode #58
    if (strcmp(w, F("parse-word")) == 0) return OP_PARSEWORD;       //  opcode #59
    if (strcmp(w, F("parse-line")) == 0) return OP_PARSELINE;       //  opcode #60
    if (strcmp(w, F(">body")) == 0) return OP_GETXT;       //  opcode #61
    if (strcmp(w, F("align2")) == 0) return OP_ALIGN2;       //  opcode #62
    if (strcmp(w, F("align4")) == 0) return OP_ALIGN4;       //  opcode #63
    if (strcmp(w, F("create")) == 0) return OP_CREATE;       //  opcode #64
    if (strcmp(w, F("find")) == 0) return OP_FIND;       //  opcode #65
    if (strcmp(w, F("next-word")) == 0) return OP_NEXTWORD;       //  opcode #66
    if (strcmp(w, F("number?")) == 0) return OP_ISNUMBER;       //  opcode #67
    // if (strcmp(w, F("-n-")) == 0) return OP_NJMPZ;       //  opcode #68
    // if (strcmp(w, F("-n-")) == 0) return OP_NJMPNZ;       //  opcode #69
    if (strcmp(w, F("bye")) == 0) return OP_BYE;       //  opcode #70
    return 0xFF;
}
// ^^^^^ - NimbleText generated - ^^^^^

CELL stringToDict(char *s, CELL to) {
    if (to == 0) to = allocSpace(strlen(s)+2);
    CELL x = to;
    while (*s) {
        dict[x++] = *(s++);
    }
    dict[x] = 0;
    return to;
}

void parseLine(char *line) {
    stringToDict(line, sys->TIB);
    push(sys->TIB);
    fPARSELINE();
}

void runTests() {
    parseLine(F(": ok SPACE 'o' emit 'k' emit SPACE .s CR ;"));
    parseLine(F("1 2 tuck . . . ok"));
    parseLine(F("1 2 1 nip . . ok"));
    parseLine(F("45 10 /mod . . ok"));
    parseLine(F("45 10 / . ok"));
    parseLine(F("45 10 mod . ok"));
    parseLine(F("1 2 3 .s ROT .s ROT .s ROT .s 2DROP drop .s ok"));
    parseLine(F("222 constant con con . .s ok"));
    parseLine(F("variable x con 2* x ! x @ . x @ 2/ . .s ok"));
    
}

void loadBaseSystem() {
    sys = (SYSVARS_T *)dict;
    sys->HERE = ADDR_HERE_BASE;
    sys->LAST = 0;
    sys->BASE = 10;
    sys->STATE = 0;
    sys->TIB = ADDR_ALLOC_BASE+1;
    loadSystem();
}

int main() {
    allocFreeAll();
    loadBaseSystem();
    runTests();
    FILE *fp = fopen("f.bin","wt");
    if (fp) {
        fprintf(fp, "%04x %04x (%ld %ld)", sys->HERE, sys->LAST, sys->HERE, sys->LAST);
        for (int i = 0; i < sys->HERE; i++) {
            if (i % 16 == 0) fprintf(fp, "\n %04x:", i);
            fprintf(fp, " %02x", dict[i]);
        }
        fclose(fp);
    }
    allocDump();
    printf("\n");
}
