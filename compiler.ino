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
  if (*word == (char)0) return 0;
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
  if (*word == (char)0) return 0;
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
  }
  if (*word == (char)0) { return 0; }
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
  addrCOMMA(LAST);          // link
  LAST = pop();
  push(HERE);
  addrCOMMA(0);             // XT
  CCOMMA(0);                // FLAGS
  CCOMMA(strlen(name));     // LEN
  while (*name) {           // NAME
    CCOMMA(*(name++));
  }
  CCOMMA(0);
  push(HERE);
  swap();
  addrStore();
}

void findWord(char *name) {
  CELL cur = LAST;
  int len = strlen(name);
  while (cur) {
    CELL t = cur + (ADDR_SZ*2) + 2;
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
      cur = addrAt(cur);
    }
  }
  push(0);
}

void parseWord(char *word) {
  DBG_LOGF("[%s]", word);
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

  DBG_LOG("-word?-");
  findWord(word);
  if (pop()) {
    CELL dp = pop();
    CELL xt = addrAt(dp+ADDR_SZ);
    // dict[dp+(2*ADDR_SZ)] => flags
    if ((STATE == 1) && (dict[dp+(2*ADDR_SZ)] == 0)) {
      CCOMMA(CALL);
      addrCOMMA(xt);
    } else {
      runProgram(xt);
    }
    return;
  }
  
  DBG_LOG("-kw?-");
  char kw[12];
  for (int i = 0; i < 999; i++) {
    char *src = (char *)pgm_read_dword(&(keyWords[i]));
    if (src == 0) break;
    strcpy_P(kw, src);
    if (strcmp(kw, word) == 0) {
      DBG_LOGF("-kw:%d-", i);
      if (STATE) {
        CCOMMA(i);
      } else {
        CELL xt = HERE + 0x10;
        dict[xt] = i;
        dict[xt+1] = RET;
        runProgram(xt);
      }
      return;
    }
  }

  if (strcmp(":", word) == 0) {
    DBG_LOG("-is-colon-");
    int len = nextWord(word);
    DBG_LOGF("-word:%s-", word);
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
    addrCOMMA(0);
    return;
  }

  if (strcmp("then", word) == 0) {
    push(HERE);
    swap();
    addrStore();
    return;
  }

  if (strcmp("begin", word) == 0) {
    push(HERE);
    return;
  }

  if (strcmp("again", word) == 0) {
    CCOMMA(JMP);
    addrCOMMA(pop());
    return;
  }

  if (strcmp("while", word) == 0) {
    CCOMMA(JMPNZ);
    addrCOMMA(pop());
    return;
  }
  
  if (strcmp("until", word) == 0) {
    CCOMMA(JMPZ);
    addrCOMMA(pop());
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

void loadHereLast(char *line) {
    char word[24];
    STATE = 0;
    int len = nextWord(word);
    CELL hereVal = is_number(word);
    if (status == 0) {
      writePort_String("\nHERE val not a number.");
      return;
    }
    len = nextWord(word);
    CELL lastVal = is_number(word);
    if (status == 0) {
      writePort_String("\nLAST val not a number.");
      return;
    }
    if ((0 <= hereVal) && (hereVal < DICT_SZ) && 
        (0 <= lastVal) && (lastVal < DICT_SZ)) {
      HERE=  hereVal;
      LAST = lastVal;
      STATE = LOAD_LINE;
    } else {
      writePort_String("\nHERE or LAST val not in range.");
    }
}

void loadLine(char *line) {
    if (strcmp("(end)", line) == 0) {
        STATE = 0;
    } else {
        char word[32];
        int len = nextWord(word);
        writePort_StringF("[%s]", word);
        if ((len < 1) || (word[len-1] != ':')) {
            writePort_String("\nAddress must end with ':'");
            STATE = 0;
            return;
        }
        word[len-1] = (char)0;
        CELL addr = is_number(word);
        if (status == 0) {
            writePort_String("\nAddress must be a number");
            STATE = 0;
            return;
        }
        len = nextWord(word);
        while (len) {
            writePort_StringF("[%s]", word);
            CELL val = is_number(word);
            if ((status == 0) || (val > 0xFF) || (val < 0)) {
                writePort_String("\nInvalid byte value, must be 0-255");
                STATE = 0;
                return;
            }
            dict[addr++] = (val & 0xFF);
            len = nextWord(word);
        }
    }
}

void parseLine(char *line) {
  DBG_LOG("\n---"); DBG_LOG(line); DBG_LOG("---\n");
  char word[32];
  toIN = line;
  if (STATE == LOAD_HERE_LAST) {
      loadHereLast(line);
      return;
  }
  if (STATE == LOAD_LINE) {
      loadLine(line);
      return;
  }
  while (1) {
    if (nextWord(word) == 0) { return; }
    if (strcmp("\\", word) == 0) { return; }
    if (strcmp("//", word) == 0) { return; }
    parseWord(word);
  }
}
