#ifndef __BOARD_H__
#define __BOARD_H__

#ifndef _WIN32
#define __DEV_BOARD__
#endif

#ifdef __DEV_BOARD__
    #include <Arduino.h>
    #define mySerial SerialUSB
    void printSerial(const char *);
    void loadSource(const char *source);
    #define __NEEDS_ALIGN__
    #define DICT_SZ (64*1024)
    #define STK_SZ 32
    #define TIB_SZ 0x0400
    #define ALLOC_SZ 16
#else
    #include <windows.h>
    #define DICT_SZ (512*1024)
    #define STK_SZ 64
    #define TIB_SZ 0x0400
    #define ALLOC_SZ 64
    #define F(str) (char *)str
    #define PSTR(str) (char *)str
    #define strcmp_PF(str1, str2) strcmp(str1, str2)
    void loadSource(const char *source);
    typedef unsigned int uint;
    typedef unsigned long ulong;
    #define PIN_INPUT 0
    #define PIN_INPUT_PULLUP 0
    #define PIN_INPUT_PULLDOWN 0
    #define PIN_OUTPUT 0
#endif

#endif
