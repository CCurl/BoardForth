void blockOpen();
void fileRead();
void fileWrite();
void fileClose();

void comOpen();
void comRead();
void comWrite();
void comClose();

// ---------------------------------------------------------------------
void writePort_String(const char *str)
{
  sendOutput(str);
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
void write_portEmit() {
  char x[2];
  CELL val = pop();
  x[0] = val & 0xff;
  x[1] = 0;
  sendOutput(x);
}

// ---------------------------------------------------------------------
void write_portDot() {
  char x[24];
  CELL val = pop();
  if (BASE == 10) sprintf(x, " %ld", val);
  else if (BASE == 0x10) sprintf(x, " %lx", val);
  else sprintf(x, "(%d in BASE %d)", val, BASE);
  writePort_String(x);
}

// ---------------------------------------------------------------------
void write_comOpen() {
  #ifdef IS_PC
      CELL action = pop();
      action ? comOpen() : comClose();
  #else
      writePort_String("-comOpen: PC only-");
  #endif
}

// ---------------------------------------------------------------------
void write_comIO() {
  #ifdef IS_PC
    comWrite();
  #else
    write_portEmit();
  #endif
}

// ---------------------------------------------------------------------
void read_ComIO() {
  #ifdef IS_PC
    comRead();
  #else
    push(0);
    if (SERIAL_available()) {
        T = SERIAL_read();
    }
  #endif
}

// ---------------------------------------------------------------------
void write_blockOpen() {
  #ifdef IS_PC
      CELL action = pop();
      action ? blockOpen() : fileClose();
  #else
      writePort_String("-fileOpen: PC only-");
  #endif
}

// ---------------------------------------------------------------------
void write_fileIO() {
  #ifdef IS_PC
    fileWrite();
  #else
    write_portEmit();
  #endif
}

// ---------------------------------------------------------------------
void read_fileIO() {
  #ifdef IS_PC
    fileRead();
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
  DBG_LOGF("-readPort(0x%lx)-", portNumber);
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
    case PORT_HERE:
      push(HERE);
      break;
    
    case PORT_LAST:
      push(LAST);
      break;
    
    case PORT_STATE:
      push(STATE);
      break;
    
    case PORT_BASE:
      push(BASE);
      break;
    
    case PORT_DSP:
      push(DSP);
      break;
    
    case PORT_DICT_SZ:
      push(DICT_SZ);
      break;
    
    case PORT_COM_IO:
      read_ComIO();
      break;
    
    case PORT_FILE_IO:
      read_fileIO();
      break;
  }
}

void writePort(CELL portNumber) {
  DBG_LOGF("\n-writePort(%lx, %ld)-", portNumber, T);
  CELL val;
  if ((portNumber >= PORT_PINS) && (portNumber < (PORT_PINS+0x0100))) {
      int pinNumber = (portNumber & 0xFF);
      writePort_StringF("(pin port:%d<-%ld)", pinNumber, pop());
      return;
  }
  if ((portNumber >= PORT_APINS) && (portNumber < (PORT_APINS+0x100))) {
      int pinNumber = (portNumber & 0xFF);
      writePort_StringF("(apin port:%d<-%ld)", pinNumber, pop());
      return;
  }
  switch (portNumber) {
    case PORT_COM_IO:
      write_comIO();
      break;
      
    case PORT_COM_OPEN:
      write_comOpen();
      break;
      
    case PORT_BLOCK_OPEN:
      write_blockOpen();
      break;
      
    case PORT_FILE_IO:
      write_fileIO();
      break;
      
    case PORT_BASE:
      val = pop();
      BASE = val > 0 ? val : 10;
      break;
      
    case PORT_STATE:
      val = pop();
      STATE = pop();
      break;
      
    case PORT_HERE:
      val = pop();
      val = (val >= DICT_SZ) ? DICT_SZ-1 : val;
      val = (val < 1) ? 0 : val;
      HERE = val;
      break;
    
    case PORT_LAST:
      val = pop();
      val = (val >= DICT_SZ) ? DICT_SZ-1 : val;
      val = (val < 1) ? 0 : val;
      LAST = val;
      break;
    
    case PORT_DSP:
      val = pop();
      val = (val >= STK_SZ) ? STK_SZ-1 : val;
      val = (val < 1) ? 0 : val;
      DSP = val;
      break;
    
    case PORT_DOT:
      write_portDot();
      break;
    
    case PORT_EMIT:
      write_portEmit();
      break;
    
  }
}

#ifdef IS_PC
void blockOpen() {
    CELL blockNum = pop();
    char fn[24];

    sprintf(fn, "block-%03d.fs", blockNum);
    FILE *fp = fopen(fn, "rt");
    push((CELL)fp);
}

void fileRead() {
    FILE *fp = (FILE *)pop();
    push(0);
    if (fp && (!feof(fp))) {
      T = fgetc(fp);
      DBG_LOGF("-fileRead:%d-",(int)T);
    }
}

void fileWrite() {
    FILE *fp = (FILE *)pop();
    CELL v = pop();
    if (fp) {
      fputc(v%0xFF, fp);
    }
}

void fileClose() {
    FILE *fp = (FILE *)pop();
    if (fp) { fclose(fp); }
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

    hPort = CreateFile(port,          // for COM1—COM9 only
        GENERIC_READ | GENERIC_WRITE, //Read/Write
        0,               // No Sharing
        NULL,            // No Security
        OPEN_EXISTING,   // Open existing port only
        0,               // Non Overlapped I/O
        NULL);


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
}

void comRead() {
    HANDLE H = (HANDLE)pop();
    char buf[2];
    DWORD num = 0;
    int sts = ReadFile(H, buf, 1, &num, NULL);
    (num) ? push(buf[0]) : push(0);
}

void comWrite() {
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

void comClose() {
    HANDLE H = (HANDLE)pop();
    if (H) { CloseHandle(H); }
}
#endif
