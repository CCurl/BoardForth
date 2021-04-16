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
    #define WORDS_SZ 256
#else
    #include <windows.h>
    #define DICT_SZ (64*1024)
    #define STK_SZ 32
    #define TIB_SZ 0x0400
    #define WORDS_SZ 256
    #define F(str) (char *)str
    #define PSTR(str) (char *)str
#endif

#endif
