CELL PC;
byte IR;

byte dict[DICT_SZ+1];
CELL HERE, LAST, BASE, STATE;

CELL dstk[STK_SZ+1];
CELL rstk[STK_SZ+1];
byte DSP, RSP;

void vm_init() {
  for (int i = 0; i < DICT_SZ; i++) {
    dict[i] = 0;
  }
  HERE = DSP = RSP = 0;
  STATE = 0;
  BASE = 10;
  // 16-bits so that it can be replaced with an address for auto-run
  WCOMMA(0);
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
    if (RSP > 1) {
        return rstk[RSP--];
    }
    return 0;
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
  // sendOutput("CCOMMA("); Dot(HERE, 0, 0); Dot(val, 0, 0); sendOutput(")");
  push(val);
  push(HERE);
  HERE = cStore();
}

CELL wStore() {
    CELL addr = pop();
    CELL v    = pop();
    if ((addr+2) < DICT_SZ) {
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

    if ((addr+4) < DICT_SZ) {
        dict[addr++] = (v & 0xff);
        dict[addr++] = ((v >>  8) & 0xff);
        dict[addr++] = ((v >> 16) & 0xff);
        dict[addr++] = ((v >> 24) & 0xff);
    }
    return addr;
 }

void Fetch() {
    CELL addr = T;
    CELL v    = 0;
    if ((addr+4) < DICT_SZ) {
        v = dict[addr++];
        v += ((CELL)dict[addr++] <<  8);
        v += ((CELL)dict[addr++] << 16);
        v += ((CELL)dict[addr++] << 24);
    }
    T = v;
}

void Comma() {
    push(HERE);
    HERE = lStore();
}

void COMMA(CELL val) {
  push(val);
  push(HERE);
  HERE = Store();
}

CELL addrAt(CELL loc) {
    CELL addr = dict[loc++];
    addr += ((CELL)dict[loc++] << 8);
    if (ADDR_SZ > 2) { addr += ((CELL)dict[loc++] << 16); }
    if (ADDR_SZ > 3) { addr += ((CELL)dict[loc++] << 24); }
    return addr;
}

void swap() {
  CELL t = T;
  T = N;
  N = t;
}

void dot() {
  CELL v = pop();
  Dot(v, BASE, 0);
}

void Dot(CELL num, int base, int width) {
  char buf[40];
  if (base == 16){
    sprintf(buf, " 0x%02lx", num);
  } else if (base == 10) {
    sprintf(buf, " %ld", num);
  } else {
    sprintf(buf, " (%lx in base %lx)", num, base);
  }
  sendOutput(buf);
}

void runProgram(CELL start) {
    int callDepth = 0;
    CELL t1, t2;
    PC = start;
    
    while (1) {
      IR = dict[PC++];
      switch (IR) {
        case CALL: 
          ++callDepth;
          rpush(PC+ADDR_SZ);
          PC = addrAt(PC);
          break;
  
        case RET: 
          if (--callDepth < 0) { return; }
          break;
          
        case JMP:
          PC = addrAt(PC);
          break;
  
        case JMPZ:
          if (pop() == 0) {
            PC = addrAt(PC);
          } else {
            PC += ADDR_SZ;
          }
          break;
  
        case JMPNZ:
          if (pop()) {
            PC = addrAt(PC);
          } else {
            PC += ADDR_SZ;
          }
          break;
  
        case ONEMINUS:
          T--;
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
          push(PC);
          cFetch();
          PC += 1;
          break;
  
        case WLIT:
          push(PC);
          wFetch();
          PC += 2;
          break;
  
        case LIT:
          push(PC);
          Fetch();
          PC += 4;
          break;

        case EMIT:
          {
            char *cp = (char *)&dict[HERE+24];
            sprintf(cp, "%c", pop());
            sendOutput(cp);
          }
          break;

        case DOT:
          Dot(pop(), BASE, 0);
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
