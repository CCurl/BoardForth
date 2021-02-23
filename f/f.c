#include "defs.h"

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
    parseLine(F("0 ?DUP .s drop 755 ?DUP .s 2DROP ok"));
    parseLine(F("1 2 tuck . . . ok"));
    parseLine(F("1 2 1 nip . . ok"));
    parseLine(F("45 10 /mod . . ok"));
    parseLine(F("45 10 / . ok"));
    parseLine(F("45 10 mod . ok"));
    parseLine(F("1 2 3 .s ROT .s -ROT .s 2DROP drop ok"));
    parseLine(F("222 constant con con . ok"));
    parseLine(F("variable x con 2* x ! x @ . x @ 2/ . ok"));
    parseLine(F("123 x ! 17 x +! x @ . ok"));
    parseLine(F(": k 1000 * ; : mil k k ;"));
    parseLine(F(": bm 's' emit begin 1- while- drop 'e' emit ; 100 mil bm ok"));
}

void doHistory(char *l) {
    FILE *fp = fopen("history.txt", "at");
    if (fp) {
        fprintf(fp, "%s\n", l);
        fclose(fp);
    }
}

void repl() {
    char *tib = (char *)&dict[sys->TIB];
    printStringF("Hello.");
    while (1) {
        printString(" ok. "); fDOTS(); printString("\n"); 
        gets(tib);
        if (strcmp(tib, "bye") == 0) return;
        doHistory(tib);
        push(sys->TIB);
        fPARSELINE();
    }
}

void loadUserWords() {
    parseLine(": dPin# $01000000 + ;");
    parseLine(": aPin# $02000000 + ;");
    parseLine(": >lh 2DUP > IF SWAP THEN ;");
    parseLine(": dump >lh DO I C@ . LOOP ;");
    // parseLine(": xxxxx :");
}

#ifndef __DEV_DOARD__
int main() {
    printString("BoardForth v0.0.1 - Chris Curl\n");
    printString("Source: https://github.com/CCurl/BoardForth \n");
    vmInit();
    loadBaseSystem();
    loadUserWords();
    // runTests();
    repl();
    FILE *fp = fopen("f.bin","wt");
    if (fp) {
        fprintf(fp, "%04x %04x (%ld %ld)", sys->HERE, sys->LAST, sys->HERE, sys->LAST);
        for (int i = 0; i < sys->HERE; i++) {
            if (i % 16 == 0) fprintf(fp, "\n %04x:", i);
            fprintf(fp, " %02x", dict[i]);
        }
        fclose(fp);
    }
    // allocDump();
    // printStringF("\n");
}
#endif