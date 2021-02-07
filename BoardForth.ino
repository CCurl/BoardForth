
#include "vm.h"

// FLASH(ok) = ": ok cr 'o' emit 'k' emit cr ; ok";
// FLASH(test) = ": emit $20001 ! ; : cr '-' emit $ffffffff drop ;";

const char *const strings[] PROGMEM = {
  0
  };


void sendKeyboard(char *str) {
#ifdef HAS_KEYBOARD
  Keyboard.begin();
  Keyboard.print(str);
  Keyboard.end();
 #else
  sendOutput(F("[kb]"));
  sendOutput(str);
  sendOutput(F("[kb]"));
 #endif
}

void keyPress(char key, char modifier) {
#ifdef HAS_KEYBOARD
  Keyboard.begin();
  if ((modifier == KEY_LEFT_CTRL) || (modifier == KEY_RIGHT_CTRL)) {
    Keyboard.press(modifier);
  }
  if ((modifier == KEY_LEFT_SHIFT) || (modifier == KEY_RIGHT_SHIFT)) {
    Keyboard.press(modifier);
  }
  Keyboard.press(key);
  delay(1000);
  Keyboard.releaseAll();
  Keyboard.end();
#else
  sendOutput(F("[kb]"));
  sendOutput_Char(key);
  sendOutput(F("[kb]"));
#endif
}

void dumpDict() {
  char buf[16];
  int h = 0;
  int b = BASE;
  BASE = 16;
  while (h < HERE) {
      writePort_StringF("\n%04lx:", h);
    for (int i = 0; i < 8; i++) {
      writePort_StringF(" %02x", dict[h++]);
    }
    writePort_String(" ");
    for (int i = 0; i < 8; i++) {
      writePort_StringF(" %02x", dict[h++]);
    }
  }
  writePort_String("\n");
  BASE = b;
}

void setup() {
  SERIAL_begin(19200);
  // Default pin 13 (the LED) to OUTPUT and OFF (LOW)
  // pinMode(13, OUTPUT);
  // writePort(PORT_PINS+13, LOW);

  while (!SERIAL);
  while (SERIAL_available() > 0) SERIAL_read();
  sendOutput("\n");
  // sendKeyboard("hi there");

  vm_init();

  char *buf = (char *)vm_Alloc(256);
  for (int i = 0; i < 999; i++ ) {
    char *src;
    #ifdef MEM_ESP8266
     src = (char *)pgm_read_dword(&(strings[i]));
    #else
     src = (char *)pgm_read_word(&(strings[i]));
    #endif
    if (src == 0) {
      break;
    }
    strcpy_P(buf, src);
    parseLine(buf);
  }
  writePort_String("\nBoardForth v0.0.1 Written by Chris Curl (2021)");
  writePort_StringF("\nDictionary size is: %d (0x%lx) bytes.", DICT_SZ, (long)DICT_SZ);
  writePort_String("\nHello.");
}

void loop() {
  while (SERIAL_available() > 0) {
    char c = SERIAL_read();
    char_in(c);
  }
  // autoRun();
  vm_FreeAll();
}

void DoNothing(const char *str) {}

void DoNothingF(const char *fmt, ...) {}
