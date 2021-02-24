#include <Arduino.h>
#include <avr/pgmSpace.h>

void loader();
void takeP(const PROGMEM char *str);
void zzz(char *);

void setup() {
    Serial.begin(19200);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    zzz("regular1");
    zzz("regular2");
    zzz("regular3");
    loader();
}

void loop() {

}

void takeP(const PROGMEM char *str) {
    char buf[128];
    strcpy_PF(buf, str);
    zzz(buf);
}

void zzz(char *str) {
    Serial.println(str);
}
