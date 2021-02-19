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

// vvvvv -- NimbleText generated -- vvvvv
BYTE getOpcode(char *w) {
    if (strcmp(w, F("noop")) == 0) return OP_NOOP;
    if (strcmp(w, F("cliteral")) == 0) return OP_CLIT;
    if (strcmp(w, F("wliteral")) == 0) return OP_WLIT;
    if (strcmp(w, F("literal")) == 0) return OP_LIT;
    if (strcmp(w, F("c@")) == 0) return OP_CFETCH;
    if (strcmp(w, F("w@")) == 0) return OP_WFETCH;
    if (strcmp(w, F("a@")) == 0) return OP_AFETCH;
    if (strcmp(w, F("@")) == 0) return OP_FETCH;
    if (strcmp(w, F("c!")) == 0) return OP_CSTORE;
    if (strcmp(w, F("w!")) == 0) return OP_WSTORE;
    if (strcmp(w, F("a!")) == 0) return OP_ASTORE;
    if (strcmp(w, F("!")) == 0) return OP_STORE;
    if (strcmp(w, F("c,")) == 0) return OP_CCOMMA;
    if (strcmp(w, F("w,")) == 0) return OP_WCOMMA;
    if (strcmp(w, F(",")) == 0) return OP_COMMA;
    if (strcmp(w, F("a,")) == 0) return OP_ACOMMA;
    if (strcmp(w, F("call")) == 0) return OP_CALL;
    if (strcmp(w, F("ret")) == 0) return OP_RET;
    if (strcmp(w, F("jmp")) == 0) return OP_JMP;
    if (strcmp(w, F("jmpz")) == 0) return OP_JMPZ;
    if (strcmp(w, F("jmpnz")) == 0) return OP_JMPNZ;
    if (strcmp(w, F("1-")) == 0) return OP_ONEMINUS;
    if (strcmp(w, F("1+")) == 0) return OP_ONEPLUS;
    if (strcmp(w, F("dup")) == 0) return OP_DUP;
    if (strcmp(w, F("swap")) == 0) return OP_SWAP;
    if (strcmp(w, F("drop")) == 0) return OP_DROP;
    if (strcmp(w, F("over")) == 0) return OP_OVER;
    if (strcmp(w, F("+")) == 0) return OP_ADD;
    if (strcmp(w, F("-")) == 0) return OP_SUB;
    if (strcmp(w, F("*")) == 0) return OP_MULT;
    if (strcmp(w, F("/mod")) == 0) return OP_SLMOD;
    if (strcmp(w, F("<<")) == 0) return OP_LSHIFT;
    if (strcmp(w, F(">>")) == 0) return OP_RSHIFT;
    if (strcmp(w, F("and")) == 0) return OP_AND;
    if (strcmp(w, F("or")) == 0) return OP_OR;
    if (strcmp(w, F("xor")) == 0) return OP_XOR;
    if (strcmp(w, F("not")) == 0) return OP_NOT;
    if (strcmp(w, F(">r")) == 0) return OP_DTOR;
    if (strcmp(w, F("r@")) == 0) return OP_RFETCH;
    if (strcmp(w, F("r>")) == 0) return OP_RTOD;
    if (strcmp(w, F("emit")) == 0) return OP_EMIT;
    if (strcmp(w, F(".")) == 0) return OP_DOT;
    if (strcmp(w, F(".s")) == 0) return OP_DOTS;
    if (strcmp(w, F(".\"")) == 0) return OP_DOTQUOTE;
    if (strcmp(w, F("(")) == 0) return OP_PAREN;
    if (strcmp(w, F("wdtfeed")) == 0) return OP_WDTFEED;
    if (strcmp(w, F("brk")) == 0) return OP_BREAK;
    if (strcmp(w, F("tib")) == 0) return OP_TIB;
    if (strcmp(w, F("#tib")) == 0) return OP_NTIB;
    if (strcmp(w, F(">in")) == 0) return OP_TOIN;
    if (strcmp(w, F("open-block")) == 0) return OP_OPENBLOCK;
    if (strcmp(w, F("file-close")) == 0) return OP_FILECLOSE;
    if (strcmp(w, F("file-read")) == 0) return OP_FILEREAD;
    if (strcmp(w, F("load")) == 0) return OP_LOAD;
    if (strcmp(w, F("thru")) == 0) return OP_THRU;
    if (strcmp(w, F("base")) == 0) return OP_BASE;
    if (strcmp(w, F("state")) == 0) return OP_STATE;
    if (strcmp(w, F("(here)")) == 0) return OP_HERE;
    if (strcmp(w, F("(last)")) == 0) return OP_LAST;
    if (strcmp(w, F("parse-word")) == 0) return OP_PARSEWORD;
    if (strcmp(w, F("parse-line")) == 0) return OP_PARSELINE;
    if (strcmp(w, F("get-xt")) == 0) return OP_GETXT;
    if (strcmp(w, F("align2")) == 0) return OP_ALIGN2;
    if (strcmp(w, F("align4")) == 0) return OP_ALIGN4;
    if (strcmp(w, F("create")) == 0) return OP_CREATE;
    if (strcmp(w, F("find")) == 0) return OP_FIND;
    if (strcmp(w, F("next-word")) == 0) return OP_NEXTWORD;
    if (strcmp(w, F("number?")) == 0) return OP_ISNUMBER;
    if (strcmp(w, F("bye")) == 0) return OP_BYE;
    return 0xFF;
}
// ^^^^^ -- NimbleText generated -- ^^^^^

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

void loadBaseSystem() {
    sys = (SYSVARS_T *)dict;
    sys->HERE = ADDR_HERE_BASE;
    sys->LAST = 0;
    sys->BASE = 10;
    sys->STATE = 0;
    sys->TIB = ADDR_ALLOC_BASE+1;

    parseLine(F(": tmp 92 . ; : cr #13 emit #10 emit ;"));
    parseLine(F(": test-123 71 10 /mod swap . . ;"));
    parseLine(F(": space $20 emit ; : ok space 'o' emit 'k' emit '.' emit space .s cr ;"));
    parseLine(F(": main cr tmp 456 99982 . . test-123 'A' emit $63 1+ . cr ;"));
    parseLine(F(": hex $10 base ! ; : decimal #10 base ! ;"));
    parseLine(F("cr 'L' emit (last) dup . @ . 33 ok drop 'H' emit (here) dup . @ . ok"));
    parseLine(F("1 2 3 + + . >in dup . @ dup . hex . ok"));
}

int main() {
    allocFreeAll();
    loadBaseSystem();
    for (int i = 0; i < sys->HERE; i++) {
        if (i % 16 == 0) printf("\n %04x:", i);
        printf(" %02x", dict[i]);
    }
    CELL addr = stringToDict("main", 0);
    push(addr);
    fFIND();
    allocFree(addr);
    if (pop()) {
        fGETXT();
        run(pop(), 0);
    }
    allocDump();
    printf("\n");
}
