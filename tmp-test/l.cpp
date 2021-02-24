#include <Arduino.h>
#include <avr/pgmSpace.h>

extern void takeP(const PROGMEM char *str);

void loader() {
    takeP(PSTR("hi there 1"));
    takeP(PSTR("hi there 2"));
}
