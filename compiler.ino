#include "keywords.h"
char *toIN;
int status;

char nextChar() {
  // sendOutput("["); sendOutput(*toIN); sendOutput("]");
  if (*toIN) {
    return *(toIN++);
  }
  return 0;
}

char skipWS() {
  // sendOutput("<<");
  char c = nextChar();
  while (c && (c <= 0x20)) c = nextChar();
  // sendOutput(">>");
  return c;
}

int nextWord(char *w) {
  int len = 0;
  char c = skipWS();
  while (c && (c > 0x20)) {
    *(w++) = c;
    ++len;
    c = nextChar();
  }

  *w = 0;
  return len;
}

ulong is_binary(char *word) {
  ulong num = 0;
  while (*word) {
    char c = *(word++);
    if ((c >= '0') && (c <= '1')) {
      num *= 2;
      num += (c - '0');
    } else {
      return 0;
    }
  }
  status = 1;
  return num;
}

ulong is_hex(char *word) {
  ulong num = 0;
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
    return 0;
  }
  status = 1;
  return num;
}

ulong is_decimal(char *word) {
  ulong num = 0;
  int is_neg = 0;
  if (*word == '-') {
    word++;
    is_neg = 1;
    if (*word == 0) { return 0; }
  }
  while (*word) {
    char c = *(word++);
    if ((c >= '0') && (c <= '9')) {
      num *= 10;
      num += (c - '0');
    } else {
      return 0;
    }
  }
  status = 1;
  return is_neg ? (ulong)((long)(-num)) : num;
}

ulong is_number(char *word) {
  status = 0;

  if ((*word == '\'') && (*(word+2) == '\'') && (*(word+3) == 0)) {
    status = 1;
    return *(word+1);
  }

  if (*word == '#') return is_decimal(word+1);
  if (*word == '$') return is_hex(word+1);
  if (*word == '%') return is_binary(word+1);

  if (BASE == 10) return is_decimal(word);
  if (BASE == 16) return is_hex(word);
  if (BASE ==  2) return is_binary(word);

  status = 0;
  return 0;
}

void defineWord(char *name) {
  DBG_LOGF("\n-dw-%s at %lx-", name, HERE);
  push(HERE);
  WCOMMA(LAST);             // link
  LAST = pop();
  push(HERE);
  WCOMMA(0);                // XT
  CCOMMA(0);                // FLAGS
  CCOMMA(strlen(name));     // LEN
  while (*name) {           // NAME
    CCOMMA(*(name++));
  }
  CCOMMA(0);
  push(HERE);
  swap();
  wStore();
}

void findWord(char *name) {
  CELL cur = LAST;
  int len = strlen(name);
  while (cur) {
    CELL t = cur + 6;
    int found = 0;
    if (dict[t-1] == len) {
      found = 1;
      for (int i = 0; i < len; i++) {
        if (name[i] != dict[t+i]) {
          found = 0;
          break;
        }
      }
    }
    if (found) {
      push(cur);
      push(1);
      return;
    } else {
      push(cur);
      wFetch();
      cur = pop();
    }
  }
  push(0);
}

void parseWord(char *word) {
  ulong val = is_number(word);
  if (status) {
    DBG_LOGF("-num:%ld-", val);
    if (STATE) {
      if (val <= 0xFF) {
        CCOMMA(CLIT);
        CCOMMA(val);
        return;
      }
      if (val > 0xFFFF) {
        CCOMMA(LIT);
        COMMA(val);
        return;
      }
      CCOMMA(WLIT);
      WCOMMA(val);
      return;
    } else {
      push(val);
    }
    return;
  }

  findWord(word);
  if (pop()) {
    CELL dp = pop();
    push(dp + 2); // Get the XT
    wFetch();
    // dict[dp+4] => flags
    if ((STATE == 1) && (dict[dp+4] == 0)) {
      CCOMMA(CALL);
      wComma();
    } else {
      runProgram(pop());
    }
    return;
  }
  
  char kw[12];
  for (int i = 0; i < 999; i++) {
    char *src = (char *)pgm_read_word(&(keyWords[i]));
    if (src == 0) break;
    strcpy_P(kw, src);
    if (strcmp(kw, word) == 0) {
      //sendOutput("-kw:"); sendOutput(i); sendOutput("-");
      if (STATE) {
        CCOMMA(i);
      } else {
        dict[HERE+10] = i;
        dict[HERE+11] = RET;
        runProgram(HERE+10);
      }
      return;
    }
  }

  if (strcmp(":", word) == 0) {
    int len = nextWord(word);
    if (len) {
      defineWord(word);
      STATE = 1;
    }
    return;
  }

  if (strcmp(";", word) == 0) {
    CCOMMA(RET);
    STATE = 0;
    return;
  }

  if (strcmp("if", word) == 0) {
    CCOMMA(JMPZ);
    push(HERE);
    WCOMMA(0);
    return;
  }

  if (strcmp("then", word) == 0) {
    push(HERE);
    swap();
    wStore();
    return;
  }

  if (strcmp("begin", word) == 0) {
    push(HERE);
    return;
  }

  if (strcmp("again", word) == 0) {
    CCOMMA(JMP);
    wComma();
    return;
  }

  if (strcmp("while", word) == 0) {
    CCOMMA(JMPNZ);
    wComma();
    return;
  }
  
  if (strcmp("until", word) == 0) {
    CCOMMA(JMPZ);
    wComma();
    return;
  }
  
  if (strcmp("variable", word) == 0) {
    int len = nextWord(word);
    if (len) {
      defineWord(word);
      CCOMMA(WLIT);
      WCOMMA(HERE+3);
      CCOMMA(RET);
      COMMA(0);
    }
    return;
  }
  
  STATE = 0;
  writePort_StringF("\n[%s]??", word);
}

void parseLine(char *line) {
  DBG_LOG("\n---"); DBG_LOG(line); DBG_LOG("---\n");
  char word[32];
  toIN = line;
  while (1) {
    if (nextWord(word) == 0) { return; }
    if (strcmp("\\", word) == 0) { return; }
    if (strcmp("//", word) == 0) { return; }
    parseWord(word);
  }
}
