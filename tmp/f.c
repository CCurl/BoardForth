#include "defs.h"

void CCOMMA(BYTE v) { push(v); fCCOMMA(); }
void WCOMMA(WORD v) { push(v); fWCOMMA(); }
void COMMA(CELL v)  { push(v); fCOMMA();  }
void ACOMMA(ADDR v) { push(v); fACOMMA(); }

CELL stringToDict(char *, CELL);

#pragma region allocation
#define ALLOC_SZ 24
typedef struct {
    CELL addr;
    BYTE available;
    WORD sz;
} ALLOC_T;
ALLOC_T alloced[ALLOC_SZ];

int num_alloced = 0;
CELL curFree;

void allocDump() {
    printf("\nAlloc table (sz %d, %d used)", ALLOC_SZ, num_alloced);
    for (int i = 0; i < num_alloced; i++) {
        printf("\n%2d %04lx %4d %d", i, alloced[i].addr, (int)alloced[i].sz, (int)alloced[i].available);
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
    printf("-define [%s] at %d (%lx)", name, sys->HERE, sys->HERE);

    DICT_T *dp = (DICT_T *)&dict[sys->HERE];
    dp->prev = (ADDR)sys->LAST;
    dp->flags = 0;
    dp->len = strlen(name);
    strcpy(dp->name, name);
    sys->LAST = sys->HERE;
    sys->HERE += ADDR_SZ + dp->len + 3;
    printf(",XT:%d (%lx)-", sys->HERE, sys->HERE);
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

// (a -- [n 1] | 0)
void fISNUMBER() {
    CELL wa = T;
    char *w = &dict[wa];
    // printf("-num?[%s]-", w);
    T = 0;
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
    }

    push(wa); fISNUMBER();
    if (pop()) {
        if (sys->STATE == 1) {
            CCOMMA(OP_LIT);
            fCOMMA();
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
    }
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

    parseLine(": tmp 71 10 /mod 92 ;");
    parseLine(": test-123 71 10 /mod 92 ;");
    parseLine(": main tmp 456 99982 'A' TIB 1+ c! .s ;");
}

int main() {
    allocFreeAll();
    loadBaseSystem();
    CELL addr = stringToDict("main", 0);
    push(addr);
    fFIND();
    if (pop()) {
        fGETXT();
        run(pop(), 100);
    }
    allocFree(addr);
    for (int i = 0; i < sys->HERE; i++) {
        if (i % 16 == 0) printf("\n %04x:", i);
        printf(" %02x", dict[i]);
    }
    allocDump();
    printf("\nHERE: %d (%lx)\n", sys->HERE, sys->HERE);
    fDOTS(); printf("\n");
}
