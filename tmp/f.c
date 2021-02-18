#include "defs.h"

void CCOMMA(BYTE v) { push(v); fCCOMMA(); }
void WCOMMA(WORD v) { push(v); fWCOMMA(); }
void COMMA(CELL v)  { push(v); fCOMMA(); }
void ACOMMA(ADDR v) { push(v); fACOMMA(); }

CELL fGETXT() {
    CELL dpa = T;
    T += CELL_SZ + 1 + 1 + dict[dpa+CELL_SZ+1] + 1;
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

void fCREATE() {
    char *name = (char *)pop();
    sys->HERE = align2(sys->HERE);
    CELL newLAST = sys->HERE;
    printf("-define [%s] at %d (%lx)-", name, sys->HERE, sys->HERE);

    DICT_T *dp = (DICT_T *)&dict[sys->HERE];
    dp->prev = sys->LAST;
    dp->flags = 0;
    dp->len = strlen(name);
    strcpy(dp->name, name);
    sys->HERE += CELL_SZ + 1 + 1 + dp->len + 1;
    sys->LAST = newLAST;
}

void define(char *name) {
    push((CELL)name);
    fCREATE();
}

void fFIND() {
    char *name = (char *)&dict[pop()];
    printf("-lf:%s-", name);
    CELL cl = sys->LAST;
    while (cl) {
        DICT_T *dp = (DICT_T *)&dict[cl];
        if (strcmp(name, dp->name) == 0) {
            push(cl);
            return;
        }
        cl = dp->prev;
    }
    push(0);
}

void doCount() {
    char *x = (char *)T;
    T++;
    push((CELL)*x);
}

void run(CELL start, int num_cycles) {
    int callDepth = 1;
    PC = start;
    while (1) {
        BYTE IR = dict[PC++];
        if (IR == OP_CALL) { ++callDepth; }
        if (IR == OP_RET) {
            if (--callDepth < 1) { return; }
            PC = rpop();
        } else if (IR <= OP_BYE) {
            prims[IR]();
            if (IR == OP_BYE) { return; }
        } else {
            printf("unknown opcode: %d ($%02x)", IR, IR);
        }
        if (num_cycles) {
            if (--num_cycles < 1) { return; }
        }
    }
}

CELL toIn = 0;
BYTE nextChar() {
    if (dict[toIn]) return dict[toIn++];
    return 0;
}

void nextWord() {
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


void parseWord() {
    char *w = &dict[T];
    printf("-pw[%s]-", w);
    fFIND();
    fDROP();
}

CELL stringToDict(char *s) {
    CELL x = 3766;
    push(x);
    while (*s) {
        dict[x++] = *(s++);
    }
    dict[x] = 0;
    return pop();
}

void fPARSE_LINE() {
    toIn = pop();
    CELL buf = sys->HERE + 256;
    push(buf);
    nextWord();
    while (pop()) {
        push(buf);
        parseWord();
        push(buf);
        nextWord();
    }
}

void pl(char *word) {
    toIn = sys->HERE + 0x20;
    push(toIn);
    while (*word) {
        dict[toIn++] = *(word++);
    }
    dict[toIn] = 0;
    fPARSE_LINE();
}

void loadBaseSystem() {
    sys = (SYSVARS_T *)dict;
    sys->HERE = ADDR_HERE_BASE;
    sys->LAST = 0;
    sys->BASE = 10;
    sys->STATE = 0;

    pl(": test 71 10 /mod 92 ;");
    pl(": main test 456 99982 'A' TIB 1+ c! .s ;");
}

int main() {
    loadBaseSystem();
    push((CELL)"main");
    fFIND();
    if (T) {
        fGETXT();
        CELL xt = pop();
        printf("\nrun: %d (%04lx) ... ", xt, xt);
        run(xt, 100);
    }
    for (int i = 0; i < sys->HERE; i++) {
        if (i % 16 == 0) printf("\n %04x:", i);
        printf(" %02x", dict[i]);
    }
    printf("\nHERE: %d\n", sys->HERE);
}
