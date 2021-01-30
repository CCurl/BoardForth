#include "vm.h"

// --- MAX 70 chars ---           "123456789 123456789 123456789 123456789 123456789 123456789 123456789 ";
const char string_001[] PROGMEM = ": dict-start 0 ; : dict-end $30006 @ ; : here $30001 @ ;";
const char string_002[] PROGMEM = ": last $30002 @ ; : base $30003 ; : state $30004 ; : . $20002 ! ;";
const char string_003[] PROGMEM = ": emit $20001 ! ; : cr #13 emit #10 emit ; : bl $20 emit ;";
const char string_004[] PROGMEM = ": free dict-end here - ; : used here dict-start - ; : free? free . ; : used? used . ;";
const char string_005[] PROGMEM = ": count dup 1+ swap c@ ;";
const char string_006[] PROGMEM = ": type begin >r dup c@ emit 1+ r> 1- dup while drop drop ;";
const char string_007[] PROGMEM = ": on 1 swap ! ; : off 0 swap ! ;";
const char string_008[] PROGMEM = ": .word dup . 1+ 1+ dup w@ . 1+ 1+ count $0f and type cr ;";
const char string_009[] PROGMEM = ": words cr last begin dup .word w@ dup while drop ;";
const char string_010[] PROGMEM = ": auto-run dict-start 1+ w! ;";
const char string_011[] PROGMEM = ": auto-run-last last 1+ 1+ w@ auto-run ;";
const char string_012[] PROGMEM = "// test 123";
const char string_013[] PROGMEM = ": pin! $10000 + ! ; : pin@ $10000 + @ ;";
const char string_014[] PROGMEM = ": led! 13 pin! ; : pin>led pin@ led! ;";
const char string_015[] PROGMEM = ": apin-base 54 ; : apin# apin-base + ;";
const char string_016[] PROGMEM = ": apin! apin# $10000 + ! ; : apin@ apin# $10000 + @ ; : apin? apin@ . ;";
const char string_017[] PROGMEM = "variable running : ?running running @ ;";
const char string_018[] PROGMEM = ": run running on ; : stop running off ;";
const char string_ok[]  PROGMEM = "cr 'o' emit 'k' emit '.' emit";
// --- MAX 70 chars ---           "123456789 123456789 123456789 123456789 123456789 123456789 123456789 ";

//           "123456789 123456789 123456789 123456789 123456789 123456789 123456789 ";
FLASH(19)  = ": main ?running if 36 pin>led 0 apin? cr then $FFFF drop ;";
FLASH(20)  = ": xxx stop auto-run-last ;";
FLASH(21)  = "cr 1 2 3 4 + + + . cr $10 base ! dict-end . here . last . #10 base ! free? used?";
FLASH(999) = "123456789 123456789 123456789 123456789 123456789 123456789 123456789 ";

const char *const forthSource[] PROGMEM = {
    string_001  , string_002  , string_003  , string_004  , string_005
  , string_006  , string_007  , string_008  , string_009  , string_010
  , string_011  , string_012  , string_013  , string_014  , string_015
  , string_016  , string_017  , string_018  , string_19  , string_20, string_21
  // string_003
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
  sendOutput(key);
  sendOutput(F("[kb]"));
#endif
}

void dumpDict() {
  int h = 0;
  while (h < HERE) {
    Dot(h, 16, 4);
    sendOutput(": ");
    for (int i = 0; i < 8; i++) {
      byte v = dict[h++];
      Dot(v, 16, 2);
    }
    sendOutput("\n");
  }
}

void xxx(int addr) {
  sendOutput("\nval at "); Dot(addr, 10, 0);
  sendOutput(" is "); Dot(dict[addr], 10, 0);
  sendOutput(".");
}

void setup() {
  SERIAL.begin(19200);
  // Default pin 13 (the LED) to OUTPUT and OFF (LOW)
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  while (!SERIAL);
  while (SERIAL.available() > 0) SERIAL.read();
  sendOutput(F("\n"));
  sendKeyboard("hi there");

  vm_init();

  char *srcBuf = (char *)vm_Alloc(256);
  for (int i = 0; i < 999; i++) {
    char *src = (char *)pgm_read_word(&(forthSource[i]));
    if (src == 0) break;
    strcpy_P(srcBuf, src);
    parseLine(srcBuf);
  }
  writePort_StringF("\nDICT_SZ: %d (0x%04lx)", DICT_SZ, DICT_SZ);
  writePort_StringF(",HERE: %d (0x%04lx)", HERE, HERE);
  writePort_StringF(",LAST: %d (0x%04lx)", LAST, LAST);
  writePort_String("\nStack: ");
  dumpDSTK();
  writePort_String("\n");
  dumpDict();
}

void loop() {
  while (SERIAL.available() > 0) {
    char c = SERIAL.read();
    //char_in(c);
    sendOutput(c);
  }
  // autoRun();
  vm_FreeAll();
}

void DoNothing(const char *str) {}

void DoNothingF(const char *fmt, ...) {}
