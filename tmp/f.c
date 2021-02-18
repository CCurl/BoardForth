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

void fGETXT() {
    DICT_T *dp = (DICT_T *)&dict[T];
    T += ADDR_SZ + dp->len + 3;
}

CELL getXT(CELL addr) {
    push(addr);
    fGETXT();
    return pop();
}

// Align on 2-byte boundary
void fALIGN2() {
    CELL val = T;
    if (val & 0x01) { ++val; }
    T = val;
}

// Align on 4-byte boundary
void fALIGN4() {
    CELL val = T;
    while (val & 0x03) { ++val; }
    T = val;
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

// ( a -- )
void fCREATE() {
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

void define(char *name) {
    CELL tmp = stringToDict(name, 0);
    push(tmp);
    fCREATE();
    allocFree(tmp);
}

// (a1 -- [a2 1] | 0)
void fFIND() {
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

void doCount() {
    char *x = (char *)T;
    T++;
    push((CELL)*x);
}


CELL toIn = 0;
BYTE nextChar() {
    if (dict[toIn]) return dict[toIn++];
    return 0;
}

// ( a -- n )
void fNEXTWORD() {
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

// (a -- [n 1] | 0)
void fISNUMBER() {
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

BYTE getOpcode(char *w) {
// vvvvv -- NimbleText generated -- vvvvv
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
    if (strcmp(w, F("bye")) == 0) return OP_BYE;
// ^^^^^ -- NimbleText generated -- ^^^^^
    return 0xFF;
}

// ( a -- )
void fPARSEWORD() {
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

void fPARSE_LINE() {
    toIn = pop();
    CELL buf = allocSpace(32);
    push(buf);
    fNEXTWORD();
    while (pop()) {
        push(buf);
        fPARSEWORD();
        push(buf);
        fNEXTWORD();
    }
    allocFree(buf);
}

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
    fPARSE_LINE();
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
    parseLine(F("cr 'L' emit (last) dup . @ . 33 ok drop 'H' emit (here) dup . @ . ok"));
    parseLine(F("1 2 3 + + . ok"));
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
