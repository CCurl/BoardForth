#ifndef __BOARD_H__
#define __BOARD_H__

#ifndef _WIN32
#define __DEV_BOARD__
#else
#include <windows.h>
#define DICT_SZ (512*1024)
#define STK_SZ 64
#define TIB_SZ 0x0400
#define __FILES__
void loadSource(const char* source);
typedef unsigned int uint;
typedef unsigned long ulong;
#define PIN_INPUT 1
#define PIN_INPUT_PULLUP 2
#define PIN_OUTPUT 3
#endif

// Optional wordsets
#define __ARDUINO__

#ifdef __DEV_BOARD__
    #include <Arduino.h>
    #define mySerial SerialUSB
    // #define mySerial Serial
    // #define __ESP32__
    // #define __LITTLEFS__
    #define __JOYSTICK__
    void printSerial(const char *);
    void loadSource(const char *source);
    #define __NEEDS_ALIGN__
    #define DICT_SZ (96*1024)
    #define STK_SZ 32
    #define TIB_SZ 0x0400
    #define PIN_INPUT INPUT
    #define PIN_INPUT_PULLUP INPUT_PULLUP
    #define PIN_OUTPUT OUTPUT
#endif

#endif
