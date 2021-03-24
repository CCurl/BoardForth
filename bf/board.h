#ifndef __BOARD_H__
#define __BOARD_H__

// #define __DEV_BOARD__

#ifdef __DEV_BOARD__
    #include <Arduino.h>
    #define mySerial SerialUSB
    void printSerial(const char *);
    void loadSource(const char *source);
    #define DICT_SZ (48*1024)
    #define STK_SZ 16
    #define TIB_SZ 0x0064
    #define ALLOC_SZ 16
#else
    #define DICT_SZ (64*1024)
    #define STK_SZ 32
    #define TIB_SZ 0x0064
    #define ALLOC_SZ 32
    #define F(str) (char *)str
    #define PSTR(str) (char *)str
    #define strcmp_PF(str1, str2) strcmp(str1, str2)
    void loadSource(const char *source);
    typedef unsigned int uint;
    typedef unsigned long ulong;
#endif

#endif
