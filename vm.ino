CELL PC;
byte IR;

char TIB[80];
byte tibIn = 0;

byte dict[DICT_SZ+1];
CELL HERE, LAST, BASE, STATE;

CELL dstk[STK_SZ+1];
CELL rstk[STK_SZ+1];
byte DSP, RSP;
CELL last_allocated = 0;

void vm_init() {
  for (int i = 0; i < DICT_SZ; i++) {
    dict[i] = 0;
  }
  DSP = RSP = 0;
  HERE = STATE = 0;
  BASE = 10;
  // 16-bits so that it can be replaced with an address for auto-run
  LAST = HERE;
  vm_FreeAll();
  addrCOMMA(0);
}

void push(CELL v){
    if (DSP < STK_SZ) {
        dstk[++DSP] = v;
    }
}

CELL pop(){
    if (DSP > 0) {
        return dstk[DSP--];
    }
    return 0;
}

void rpush(CELL v){
    if (RSP < STK_SZ) {
        rstk[++RSP] = v;
    }
}

CELL rpop(){
    if (RSP > 0) {
        return rstk[RSP--];
    }
    return 0;
}

void dotS() {
  writePort_String("(");
  for (int i = 1; i <= DSP; i++) {
    dot_port(dstk[i]);
  }
  writePort_String(" )");
}

CELL cStore() {
    CELL addr = pop();
    CELL v    = pop();
    if ((0 <= addr) && (addr < DICT_SZ)) {
        dict[addr] = (v & 0xff);
        // Dot(addr, 0, 0); sendOutput(" set to "); Dot(v,0,0);
        addr++;
    } else {
      // Dot(addr, 0, 0); sendOutput("is out of range");
    }
    return addr;
}

void cFetch() {
    CELL addr = T;
    CELL v    = 0;
    if ((0 <= addr) && (addr < DICT_SZ)) {
        v = dict[addr];
    }
    T = v;
}

void cComma() {
    push(HERE);
    HERE = cStore();
}

void CCOMMA(CELL val) {
  // DBG_LOGF("CCOMMA("); Dot(HERE, 0, 0); Dot(val, 0, 0); sendOutput(")");
  push(val);
  push(HERE);
  HERE = cStore();
}

CELL wStore() {
    CELL addr = pop();
    CELL v    = pop();
    if ((0 <= addr) && ((addr+2) < DICT_SZ)) {
        dict[addr++] = (v & 0xff);
        dict[addr++] = ((v >> 8) & 0xff);
    }
    return addr;
}

void wFetch() {
    CELL addr = T;
    CELL v    = 0;
    if ((addr+2) < DICT_SZ) {
        v = dict[addr++];
        v += ((CELL)dict[addr++] << 8);
    }
    T = v;
}

void wComma() {
    push(HERE);
    HERE = wStore();
}

void WCOMMA(CELL val) {
  push(val);
  push(HERE);
  HERE = wStore();
}

CELL Store() {
    CELL addr = pop();
    CELL v    = pop();

    if ((0 <= addr) && ((addr+4) < DICT_SZ)) {
        dict[addr++] = (v & 0xff);
        dict[addr++] = ((v >>  8) & 0xff);
        dict[addr++] = ((v >> 16) & 0xff);
        dict[addr++] = ((v >> 24) & 0xff);
    } else if (DICT_SZ < addr) {
//DBG_LOGF("-Store(%lx<-%lx)-", addr, v);
      writePort(addr, v);
    }
    return addr;
 }

void Fetch() {
    CELL addr = T;
//char x[64];
//sprintf(x, "-fetch(%lx)-", addr); sendOutput(x);
    if (DICT_SZ < addr) {
      readPort(pop());
    } else if ((1 < addr) && ((addr+4) < DICT_SZ)) {
        CELL v    = 0;
        v = dict[addr++];
        v += ((CELL)dict[addr++] <<  8);
        v += ((CELL)dict[addr++] << 16);
        v += ((CELL)dict[addr++] << 24);
        T = v;
    } else {
        T = 0;
    }
}

void Comma() {
    push(HERE);
    HERE = Store();
}

void COMMA(CELL val) {
  push(val);
  Comma();
}

CELL addrAt(CELL loc) {
//    DBG_LOGF("-addr(%lx,SZ=%d):[%02x", loc, ADDR_SZ, dict[loc]);
    CELL addr = dict[loc++];
//    DBG_LOGF(",%02x", dict[loc]);
    addr += ((CELL)dict[loc++] << 8);
    if (ADDR_SZ > 2) {
//      DBG_LOGF(",%02x", dict[loc]);
      addr += ((CELL)dict[loc++] << 16);
    }
    if (ADDR_SZ > 3) {
//      DBG_LOGF(",%02x", dict[loc]);
      addr += ((CELL)dict[loc++] << 24);
    }
//    DBG_LOGF("]:%lx-", addr);
    return addr;
}

void addrCOMMA(CELL loc) {
    if (ADDR_SZ == 4) {
        COMMA(loc); 
//        DBG_LOGF("-addrCOMMA(L):HERE now=%d-", HERE);
    } else {
        WCOMMA(loc);
//        DBG_LOGF("-addrCOMMA(W):HERE now=%d-", HERE);
    }
}

void addrStore() {
    if (ADDR_SZ == 4) {
        Store(); 
    } else {
        wStore();
    }
}

void swap() {
  CELL t = T;
  T = N;
  N = t;
}

void runProgram(CELL start) {
    int callDepth = 0;
    CELL t1, t2;
    PC = start;
    char buf[64];
    long cycles = MAX_CYCLES;

    while (1) {
      if ((MAX_CYCLES > 0) && ((--cycles) < 1)) { return; }
      IR = dict[PC++];
      DBG_LOGF("\n-PC:%lx,IR:%d-", PC-1, IR);
      #ifdef LOG_DEBUG
        dotS();
      #endif
      switch (IR) {
        case CALL: 
          ++callDepth;
          rpush(PC+ADDR_SZ);
          PC = addrAt(PC);
          DBG_LOGF("-call:%lx-", PC);
          break;

        case RET:
          if (--callDepth < 0) { return; }
          PC = rpop();
          break;
          
        case JMP:
          PC = addrAt(PC);
          break;
  
        case JMPZ:
          DBG_LOGF("-JMPZ:TOS=%d-", T);
          if (pop() == 0) {
            PC = addrAt(PC);
          } else {
            PC += ADDR_SZ;
          }
          break;
  
        case JMPNZ:
          DBG_LOGF("-JMPNZ:TOS=%d-", T);
          if (pop()) {
            PC = addrAt(PC);
          } else {
            PC += ADDR_SZ;
          }
          break;
  
        case ONEMINUS:
          T--;
          break;
  
        case ONEPLUS:
          T++;
          break;
  
        case DUP:
          push(T);
          break;
  
        case SWAP:
          swap();
          break;
  
        case DROP:
          pop();
          break;
  
        case OVER:
          push(N);
          break;
  
        case ADD:
          N = (N+T);
          pop();
          break;
  
        case SUB:
          N = (N-T);
          pop();
          break;
  
        case MULT:
          N = (N*T);
          pop();
          break;
  
        case SLASHMOD:
          if (T != 0) {
            t1 = N;
            t2 = T;
            N = t1 % t2;
            T = t1 / t2;
          } else {
            N = 0;
            T = 0;
          }
          break;
  
        case LSHIFT:
          N = (N<<T);
          pop();
          break;
  
        case RSHIFT:
          N = (N>>T);
          pop();
          break;
  
        case AND:
          N = (N&T);
          pop();
          break;
  
        case OR:
          N = (N|T);
          pop();
          break;
  
        case XOR:
          N = (N^T);
          pop();
          break;
  
        case NOT:
          T = (~T);
          break;
  
        case CFETCH:
          cFetch();
          break;
  
        case WFETCH:
          wFetch();
          break;
  
        case FETCH:
          Fetch();
          break;
  
        case CSTORE:
          cStore();
          break;
  
        case WSTORE:
          wStore();
          break;
  
        case STORE:
          Store();
          break;
  
        case CLIT:
          DBG_LOGF("-CLIT:PC=%lx,", PC);
          push(PC);
          cFetch();
          PC += 1;
          DBG_LOGF("T=%lx,PC=%lx-", T, PC);
          break;
  
        case WLIT:
          DBG_LOGF("-WLIT:PC=%lx,", PC);
          push(PC);
          wFetch();
          PC += 2;
          DBG_LOGF("T=%lx,PC=%lx-", T, PC);
          break;

        case LIT:
          DBG_LOGF("-LIT:PC=%lx,", PC); // DBG_LOGF
          push(PC);
          Fetch();
          PC += 4;
          DBG_LOGF("T=%lx,PC=%lx-", T, PC);
          break;

        case DTOR:
          rpush(pop());
          break;

        case RFETCH:
          push(R);
          break;

        case RTOD:
          push(rpop());
          break;

        case WDTFEED:
          #ifdef MEM_ESP8266
            ESP.wdtFeed();
          #endif
          break;

        case GTHAN:
            N = (N > T) ? -1 : 0;
            pop();
          break;

        case LTHAN:
            N = (N < T) ? -1 : 0;
            pop();
          break;

        case EQUAL:
            N = (N == T) ? -1 : 0;
            pop();
          break;
      }
    }
}

void autoRun() {
  CELL addr = addrAt(0);
  if ((0 < addr) && (addr < DICT_SZ)) {
    runProgram(addr);
  }
}

byte *vm_Alloc(CELL sz) {
  last_allocated -= sz;
  if (last_allocated < (HERE + 24)) {
    vm_FreeAll();
    return 0;
  }
  return (byte *)&dict[last_allocated];
}

void vm_FreeAll() {
  last_allocated = DICT_SZ-1;
}

void char_in(char c) {
  if (tibIn < sizeof(TIB)) {
    TIB[tibIn++] = c;
    TIB[tibIn] = 0;
  } else {
    writePort_String("\nInput buffer full!");
  }
  if (c == 13) {
    parseLine(TIB);
    tibIn = 0;
  }
}
