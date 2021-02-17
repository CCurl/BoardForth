#include "defs.h"

CELL dstk[STK_SZ+1]; int DSP = 0;
CELL rstk[STK_SZ+1]; int RSP = 0;

BYTE IR;
CELL PC;
CELL HERE = 0, LAST = 0;
CELL BASE = 10, STATE = 0;

void push(CELL v) {
    DSP = (DSP < STK_SZ) ? DSP+1 : STK_SZ;
    T = v;
}

CELL pop() {
    DSP = (DSP > 0) ? DSP-1 : 0;
    return dstk[DSP+1];
}

void rpush(CELL v) { if (++RSP > STK_SZ) RSP = STK_SZ; R = v; }
CELL rpop() { if (--RSP < 1) RSP = 0; return rstk[RSP+1]; }

BYTE dict[DICT_SZ];

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
    DICT_T *dp = (DICT_T *)align2(HERE);
    FP handler = (FP)pop();
    char *name = (char *)pop();
    dp->prev = LAST;
    dp->flags = 0;
    dp->len = strlen(name);
    strcpy(dp->name, name);
    HERE = align2(HERE + CELL_SZ + 2 + dp->len + 1);
    LAST = (CELL)dp;
}

void doFind() {
    DICT_T *dp = (DICT_T *)LAST;
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
        FP prim = NULL;
        BYTE IR = dict[PC++];
        if (IR == OP_RET) {
            if (--callDepth < 1) { return; }
            PC = rpop();
            continue;
        }
        if (IR <= OP_LAST) {
            prim = prims[IR];
        }
        if (prim) {
            prim();
        } else {
            printf("unknown opcode: %d ($%02x)", IR, IR);
        }
        if (num_cycles) {
            if (--num_cycles < 1) { return; }
        }
    }
}

int main() {
    push(22);
    CCOMMA(OP_DUP);
    CCOMMA(OP_ONEPLUS);
    CCOMMA(OP_CLIT);
    CCOMMA(17);
    CCOMMA(OP_WLIT);
    WCOMMA(456);
    CCOMMA(OP_LIT);
    COMMA(99982);
    CCOMMA(OP_DOTS);
    CCOMMA(OP_RET);
    run(0, 0);
    for (int i = 0; i < HERE; i++) {
        printf("%02d ", dict[i]);
    }
    printf("\n");
}
