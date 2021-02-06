
// ---------------------------------------------------------------------
void writePort_String(const char *str)
{
  sendOutput(str);
  // while (*str) {
  //   emit_port(*(str++));
  // }
}

// ---------------------------------------------------------------------
void writePort_StringF(const char *fmt, ...)
{
    char buf[64];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    writePort_String(buf);
}

void readPort(CELL portNumber) {
//char x[64];
//sprintf(x, "-readPort(%lx)-", portNumber); sendOutput(x);
  if (portNumber == PORT_HERE   ) { push(HERE); }
  if (portNumber == PORT_LAST   ) { push(LAST); }
  if (portNumber == PORT_STATE  ) { push(STATE); }
  if (portNumber == PORT_BASE   ) { push(BASE); }
  if (portNumber == PORT_DSP    ) { push(DSP); }
  if (portNumber == PORT_DICT_SZ) { push(DICT_SZ); }
  if ((portNumber >= PORT_PINS) && (portNumber < (PORT_PINS+0x0100))) {
      int pinNumber = (portNumber & 0xFF);
      writePort_StringF("(read pin port:%d)", pinNumber);
  }
  if ((portNumber >= PORT_APINS) && (portNumber < (PORT_APINS+0x100))) {
      int pinNumber = (portNumber & 0xFF);
      writePort_StringF("(read apin port:%d)", pinNumber);
  }
}

void emit_port(CELL val) {
  char x[2];
  x[0] = val & 0xff;
  x[1] = 0;
  sendOutput(x);
}

void dot_port(CELL val) {
  char x[24];
  if (BASE == 10) sprintf(x, " %ld", val);
  else if (BASE == 0x10) sprintf(x, " %lx", val);
  else sprintf(x, "(%d in BASE %d)", val, BASE);
  writePort_String(x);
}

void writePort(CELL portNumber, CELL val) {
  DBG_LOGF("\n-writePort(%lx,%ld)-", portNumber, val);
  if (portNumber == PORT_EMIT  ) { emit_port(val); }
  if (portNumber == PORT_DOT   ) { dot_port(val); }
  if (portNumber == PORT_BASE  ) { BASE = val > 0 ? val : 10; }
  if (portNumber == PORT_STATE ) { STATE = val; }
  if (portNumber == PORT_HERE ) {
      val = (val >= DICT_SZ) ? DICT_SZ-1 : val;
      val = (val < 1) ? 0 : val;
      HERE = val;
  }
  if (portNumber == PORT_LAST ) {
      val = (val >= DICT_SZ) ? DICT_SZ-1 : val;
      val = (val < 1) ? 0 : val;
      LAST = val;
  }
  if (portNumber == PORT_DSP ) {
      val = (val >= STK_SZ) ? STK_SZ-1 : val;
      val = (val < 1) ? 0 : val;
      DSP = val;
  }
  if ((portNumber >= PORT_PINS) && (portNumber < (PORT_PINS+0x0100))) {
      int pinNumber = (portNumber & 0xFF);
      writePort_StringF("(pin port:%d<-%d)", pinNumber, val);
  }
  if ((portNumber >= PORT_APINS) && (portNumber < (PORT_APINS+0x100))) {
      int pinNumber = (portNumber & 0xFF);
      writePort_StringF("(apin port:%d<-%d)", pinNumber, val);
  }
}
