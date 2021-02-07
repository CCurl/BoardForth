
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

// ---------------------------------------------------------------------
void emit_port(CELL val) {
  char x[2];
  x[0] = val & 0xff;
  x[1] = 0;
  sendOutput(x);
}

// ---------------------------------------------------------------------
void dot_port(CELL val) {
  char x[24];
  if (BASE == 10) sprintf(x, " %ld", val);
  else if (BASE == 0x10) sprintf(x, " %lx", val);
  else sprintf(x, "(%d in BASE %d)", val, BASE);
  writePort_String(x);
}

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void write_ComOpen() {
  #ifdef IS_PC
      comOpen();
  #else
    emit_port(pop());
  #endif
}

// ---------------------------------------------------------------------
void write_ComIO() {
  #ifdef IS_PC
    comOut();
  #else
    emit_port(pop());
  #endif
}

// ---------------------------------------------------------------------
void read_ComIO() {
  #ifdef IS_PC
    comIn();
  #else
    push(0);
    if (SERIAL_available()) {
        T = SERIAL_read();
    }
  #endif
}

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void readPort(CELL portNumber) {
  //char x[64];
  //sprintf(x, "-readPort(%lx)-", portNumber); sendOutput(x);
  if ((portNumber >= PORT_PINS) && (portNumber < (PORT_PINS+0x0100))) {
      int pinNumber = (portNumber & 0xFF);
      writePort_StringF("(read pin port:%d)", pinNumber);
      return;
  }
  if ((portNumber >= PORT_APINS) && (portNumber < (PORT_APINS+0x100))) {
      int pinNumber = (portNumber & 0xFF);
      writePort_StringF("(read apin port:%d)", pinNumber);
      return;
  }
  switch (portNumber) {
    case PORT_HERE:  push(HERE);
      break;
    case PORT_LAST: push(LAST);
      break;
    case PORT_STATE: push(STATE);
      break;
    case PORT_BASE: push(BASE);
      break;
    case PORT_DSP: push(DSP);
      break;
    case PORT_DICT_SZ: push(DICT_SZ);
      break;
    case PORT_COM_IO: read_ComIO();
      break;
  }
}

void writePort(CELL portNumber) {
  DBG_LOGF("\n-writePort(%lx, %ld)-", portNumber, T);
  if (portNumber == PORT_COM_IO )   { write_ComIO(); return; }
  if (portNumber == PORT_COM_OPEN ) { write_ComOpen(); return; }
  CELL val = pop();
  if (portNumber == PORT_EMIT ) { emit_port(val); }
  if (portNumber == PORT_DOT  ) { dot_port(val); }
  if (portNumber == PORT_BASE ) { BASE = val > 0 ? val : 10; }
  if (portNumber == PORT_STATE) { STATE = val; }
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


#ifdef IS_PC
void comClose() {
    HANDLE H = (HANDLE)pop();
    if (H) { CloseHandle(H); }
}

void comOpen() {
    CELL portNum = pop();
    HANDLE hPort;
    char port[24];

    if (portNum < 10) {
        sprintf(port, "COM%d", portNum);
    } else {
        sprintf(port, "\\\\.\\COM%d", portNum);
    }

    printf("trying to open com port [%s] ... ", port);
    hPort = CreateFile(port,          // for COM1—COM9 only
        GENERIC_READ | GENERIC_WRITE, //Read/Write
        0,               // No Sharing
        NULL,            // No Security
        OPEN_EXISTING,   // Open existing port only
        0,               // Non Overlapped I/O
        NULL);

    printf("handle: [%d] ... ", hPort);

    if (hPort == INVALID_HANDLE_VALUE) {
      printf("Could not open port.");
      push(0); 
      return;
    }

    // Serial port parameters ... e.g.: Baud rate, Byte size, Number of start/Stop bits etc.
    DCB dcbSerialParams;
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    GetCommState(hPort, &dcbSerialParams);
    dcbSerialParams.BaudRate = CBR_19200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.Parity   = NOPARITY;
    dcbSerialParams.StopBits = ONESTOPBIT;
    SetCommState(hPort, &dcbSerialParams);

    // Set timeouts ... they are in milliSeconds
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout         = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier  = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant    = 100;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant   = 50;
    SetCommTimeouts(hPort, &timeouts);

    COMSTAT comStat;
    DWORD errors;
    ClearCommError(hPort, &errors, &comStat);

    push((CELL)hPort);
    writePort_StringF(", T now=%ld", T);
}

void comOut() {
    HANDLE H = (HANDLE)pop();
    CELL val = pop();
    if (H) {
        DWORD dwNumWritten = 0;
        char buf[2];
        buf[0] = (val&0xFF);
        buf[1] = (char)0;
        WriteFile(H, buf, 1, &dwNumWritten, NULL);
        push(dwNumWritten);
    } else {
        // writePort_String("(com port not open)");
        push(0);
    }
}

void comIn() {
    HANDLE H = (HANDLE)pop();
    char buf[2];
    DWORD num = 0;
    int sts = ReadFile(H, buf, 1, &num, NULL);
    (num) ? push(buf[0]) : push(0);
}
#endif
