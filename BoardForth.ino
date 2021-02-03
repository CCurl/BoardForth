#include "vm.h"

FLASH(1)  = ": dict-start 0 ; : dict-end $30006 @ ; : here $30001 @ ;";
FLASH(2)  = ": last $30002 @ ; : base $30003 ; : state $30004 ; : . $20002 ! ;";
FLASH(3)  = ": emit $20001 ! ; : cr #13 emit #10 emit ; : space $20 emit ;";
FLASH(4)  = ": free dict-end here - ; : used here dict-start - ; : free? free . ; : used? used . ;";
FLASH(5)  = ": count dup 1+ swap c@ ;";
FLASH(6)  = ": type begin >r dup c@ emit 1+ r> 1- dup while drop drop ;";
FLASH(7)  = ": on 1 swap ! ; : off 0 swap ! ;";
FLASH(8)  = ": .word dup . dup 2 + w@ . space 5 + count type cr ;";
FLASH(9)  = ": words cr last begin dup .word w@ dup while drop ;";
FLASH(10) = ": auto-run dict-start 1+ ! ;";
FLASH(11) = ": auto-run-last last 1+ 1+ w@ auto-run ;";
FLASH(12) = ": bm begin wdtfeed 1- dup while drop ;";
FLASH(13) = ": pin! $10000 + ! ; : pin@ $10000 + @ ;";
FLASH(14) = ": led! 13 pin! ; : pin>led pin@ led! ;";
FLASH(15) = ": apin-base 54 ; : apin# apin-base + ;";
FLASH(16) = ": apin! apin# $10000 + ! ; : apin@ apin# $10000 + @ ; : apin? apin@ . ;";
FLASH(17) = "variable running : ?running running @ ;";
FLASH(18) = ": run running on ; : stop running off ;";
FLASH(19) = ": main ?running if 36 pin>led 0 apin? cr then $FFFF drop ;";
FLASH(20) = ": k 1000 * ; : mil k k ; : go cr 's' emit bm cr 'e' emit ;";
FLASH(21) = "// cr 1 2 3 4 + + + . cr $10 base ! dict-end . here . last . #10 base ! free? used?";
FLASH(ok) = ": ok cr 'o' emit 'k' emit cr ; ok";
// FLASH(test) = ": emit $20001 ! ; : cr '-' emit $ffffffff drop ;";

const char *const strings[] PROGMEM = {
    string_1 , string_2  , string_3  , string_4  , string_5
  , string_6  , string_7  , string_8  , string_9  , string_10
  , string_11  , string_12  , string_13  , string_14  , string_15
  , string_16  , string_17  , string_18  , string_19  , string_20, string_21
// string_test
  , string_ok
  , 0
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
  writePort_String("\nStack: ");
  dotS();
  writePort_String("\n");
  dumpDict();
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
