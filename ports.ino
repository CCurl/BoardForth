
void readPort(CELL portNumber) {
//char x[64];
//sprintf(x, "-readPort(%lx)-", portNumber); sendOutput(x);
  if (portNumber == PORT_HERE   ) { push(HERE); }
  if (portNumber == PORT_LAST   ) { push(LAST); }
  if (portNumber == PORT_STATE  ) { push(STATE); }
  if (portNumber == PORT_BASE   ) { push(BASE); }
  if (portNumber == PORT_MEM_SZ ) { push(MEM_SZ); }
  if (portNumber == PORT_DICT_SZ) { push(DICT_SZ); }
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
  // writePort_StringF(x, "-port:%lx,num:%ld-", portNumber, val);
  if (portNumber == PORT_EMIT  ) { emit_port(val); }
  if (portNumber == PORT_DOT   ) { dot_port(val); }
  if (portNumber == PORT_HERE  ) { HERE = val; }
  if (portNumber == PORT_BASE  ) { BASE = val > 0 ? val : 10; }
  if (portNumber == PORT_STATE ) { STATE = val; }
  // if (portNumber == PORT_XXX ) { emit(val); }
  // if (portNumber == PORT_XXX ) { emit(val); }
}



// ---------------------------------------------------------------------
void writePort_String(const char *str)
{
  while (*str) {
    emit_port(*(str++));
  }
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
