#include "defs.h"

void CCOMMA(BYTE v) { push(v); fCCOMMA(); }
void WCOMMA(WORD v) { push(v); fWCOMMA(); }
void COMMA(CELL v)  { push(v); fCOMMA(); }
void ACOMMA(ADDR v) { push(v); fACOMMA(); }

// Align on 2-byte boundary
CELL align2(CELL val) {
    if (val & 0x01) { ++val; }
    return val;
}

// Align on 4-byte boundary
CELL align4(CELL val) {
    while (val & 0x03) { ++val; }
    return val;
}

void define() {
    DICT_T *dp = (DICT_T *)align2(sys->HERE);
    FP handler = (FP)pop();
    char *name = (char *)pop();
    dp->prev = sys->LAST;
    dp->flags = 0;
    dp->len = strlen(name);
    strcpy(dp->name, name);
    sys->HERE = align2(sys->HERE + CELL_SZ + 2 + dp->len + 1);
    sys->LAST = (CELL)dp;
}

void doFind() {
    DICT_T *dp = (DICT_T *)sys->LAST;
    char *name = (char *)pop();
    while (dp) {
        if (strcmp(name, dp->name) == 0) {
            push((CELL)dp);
            return;
        }
        dp = (DICT_T *)(dp->prev);
    }
    push((CELL)NULL);
}

void doCount() {
    char *x = (char *)T;
    T++;
    push((CELL)*x);
}

void doDup() {
    printf("dup called.\n");
    push(T);
}

BYTE nextChar() { return 0; } 

void doNextWord() {
    char c, *len = (char *)pop();
    *(len) = 0;
    char *word = len+1;

    // skip ws
    c = nextChar();
    while (c && (c < 33)) {
        c = nextChar();
    }

    // collect word
    while (c && (32 < c)) {
        *(len)++;
        *(word++) = c;
        c = nextChar();
    }
    
    *(word) = 0;
    push((CELL)len);
}


void run(CELL start, int num_cycles) {
    int callDepth = 1;
    PC = start;
    while (1) {
        BYTE IR = dict[PC++];
        if (IR == OP_RET) {
            if (--callDepth < 1) { return; }
            PC = rpop();
        } else if (IR <= OP_BYE) {
            prims[IR]();
        } else {
            printf("unknown opcode: %d ($%02x)", IR, IR);
        }
        if (num_cycles) {
            if (--num_cycles < 1) { return; }
        }
    }
}

int main() {
    sys = (SYSVARS_T *)dict;
    sys->HERE = ADDR_HERE_BASE;
    sys->LAST = 0;
    sys->BASE = 10;
    sys->STATE = 0;
    CCOMMA(OP_CLIT);
    CCOMMA(71);
    CCOMMA(OP_CLIT);
    CCOMMA(10);
    CCOMMA(OP_SLMOD);
    CCOMMA(OP_CLIT);
    CCOMMA(92);
    CCOMMA(OP_WLIT);
    WCOMMA(456);
    CCOMMA(OP_LIT);
    COMMA(99982);
    CCOMMA(OP_CLIT);
    CCOMMA('A');
    CCOMMA(OP_TIB);
    CCOMMA(OP_ONEPLUS);
    CCOMMA(OP_CSTORE);
    CCOMMA(OP_DOTS);
    CCOMMA(OP_RET);
    run(ADDR_HERE_BASE, 0);
    for (int i = 0; i < sys->HERE; i++) {
        if (i % 16 == 0) printf("\n %04x:", i);
        printf(" %02x", dict[i]);
    }
    printf("\nHERE: %d\n", sys->HERE);
}
