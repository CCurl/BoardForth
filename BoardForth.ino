#include "vm.h"

FLASH(ok) = ": ok cr 'o' emit 'k' emit cr ; ok";
// FLASH(test) = ": emit $20001 ! ; : cr '-' emit $ffffffff drop ;";

const char *const strings[] PROGMEM = {
  string_ok, 0
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
    char *src = (char *)pgm_read_dword(&(strings[i]));
    if (src == 0) {
      break;
    }
    strcpy_P(buf, src);
    parseLine(buf);
  }
  writePort_StringF("\nDICT_SZ: %d (0x%04lx)", DICT_SZ, DICT_SZ);
  writePort_StringF(",HERE: %d (0x%04lx)", HERE, HERE);
  writePort_StringF(",LAST: %d (0x%04lx)", LAST, LAST);
  writePort_String("\nStack: "); dotS();
  writePort_String("\n");
  // dumpDict();
}

void loop() {
  while (SERIAL_available() > 0) {
    char c = SERIAL_read();
    char_in(c);
    // sendOutput_Char(c);
  }
  // autoRun();
  vm_FreeAll();
}

void DoNothing(const char *str) {}

void DoNothingF(const char *fmt, ...) {}
