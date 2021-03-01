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
    // printStringF("-sd.%d", (int)to);
    if (to == 0) to = allocSpace(strlen(s)+2);
    // printStringF(":%d-\n", (int)to);
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

void loadUserWords() {
    char *buf = (char *)&dict[sys->HERE + 16];
    sprintf(buf, ": ds $%lx ;", &dict[0]);
    parseLine(buf);
    loadSource(PSTR(": mw@ dup 1+  mc@   $100 * swap mc@ or ;"));
    loadSource(PSTR(": m@  dup 2 + mw@ $10000 * swap mw@ or ;"));
    loadSource(PSTR(": mw! over   $100 / over 1+ mc! mc! ;"));
    loadSource(PSTR(": m!  over $10000 / over 2+ mw! mw! ;"));
    loadSource(PSTR(": auto-run-last last >body 0 a! ;"));
    loadSource(PSTR(": auto-run-off 0 0 a! ;"));

    loadSource(PSTR(": elapsed tick swap - 1000 /mod . . ;"));
    loadSource(PSTR(": bm tick swap begin 1- while- drop elapsed ;"));
    loadSource(PSTR(": low->high 2dup > if swap then ;"));
    loadSource(PSTR(": high->low 2dup < if swap then ;"));
    loadSource(PSTR(": dump low->high do i c@ . loop ;"));
    loadSource(PSTR(": led 13 ; led output-pin"));
    loadSource(PSTR(": led-on 1 led dpin! ; : led-off 0 led dpin! ;"));
    loadSource(PSTR(": blink led-on dup ms led-off dup ms ;"));
    loadSource(PSTR(": k 1000 * ; : mil k k ;"));
    loadSource(PSTR(": blinks 0 swap do blink loop ;"));
    loadSource(PSTR(": pp  3 ; pp input-pin"));
    loadSource(PSTR(": bp 33 ; bp input-pin"));
    loadSource(PSTR(": pp@ pp apin@ ; "));
    loadSource(PSTR(": bp@ bp dpin@ ; : bp->led bp@ led dpin! ;"));
    loadSource(PSTR("variable pplv"));
    loadSource(PSTR(": .pp dup pplv @ - abs 3 > if .. cr pplv ! else drop then ;"));
    loadSource(PSTR(": go bp->led pp@ .pp ;"));
    // loadSource(PSTR(""));
}

void dumpDict() {
    printStringF("%04x %04x (%ld %ld)", sys->HERE, sys->LAST, sys->HERE, sys->LAST);
    for (int i = 0; i < sys->HERE; i++) {
        if (i % 16 == 0) printStringF("\n %04x:", i);
        printStringF(" %02x", dict[i]);
    }
}

void ok() {
    printString(" ok. ");
    fDOTS();
    printString("\n");
}

#ifndef __DEV_BOARD__
void loadSource(const char *src) {
    parseLine((char *)src);
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
    while (1) {
        ok();
        gets(tib);
        if (strcmp(tib, "bye") == 0) return;
        doHistory(tib);
        push(sys->TIB);
        fPARSELINE();
    }
}

int main() {
    printString("BoardForth v0.0.1 - Chris Curl\n");
    printString("Source: https://github.com/CCurl/BoardForth \n");
    printStringF("Dictionary size is: %d ($%04x) bytes.\n", (int)DICT_SZ, (int)DICT_SZ);
    printStringF("Hello.");
    vmInit();
    loadBaseSystem();
    loadUserWords();
    repl();
    FILE *fp = fopen("f.bin","wt");
    if (fp) {
        push((CELL)fp);
        fDUMPDICT();
        fclose(fp);
    }
    // allocDump();
    // printStringF("\n");
}
#endif
