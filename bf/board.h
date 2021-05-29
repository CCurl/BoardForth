#ifndef __BOARD_H__
#define __BOARD_H__

#ifndef _WIN32
	#define __DEV_BOARD__
	#define __SERIAL__
	#include <Arduino.h>
	#define mySerial SerialUSB
	// #define mySerial Serial
	// #define __ESP32__
    // #include <LittleFS.h>
    // #define __LITTLEFS__
	#define __JOYSTICK__
	#define __NEEDS_ALIGN__
	#define DICT_SZ (4*1024)
	#define VARS_SZ (1*1024)
	#define STK_SZ 16
	#define TIB_SZ 0x0064
	#define MAX_FILE_SZ 10*1024
	#define PIN_INPUT INPUT
	#define PIN_INPUT_PULLUP INPUT_PULLUP
	#define PIN_OUTPUT OUTPUT
#else
	#define __IS_PC__
	#include <windows.h>
	#define DICT_SZ (512*1024)
	#define VARS_SZ (64*1024)
	#define STK_SZ 64
	#define TIB_SZ 0x0400
	#define MAX_FILE_SZ 10*1024
	#define __FILES__
	#define __COM_PORT__
	void loadSource(const char* source);
	typedef unsigned int uint;
	typedef unsigned long ulong;
	#define PIN_INPUT 1
	#define PIN_INPUT_PULLUP 2
	#define PIN_OUTPUT 3
	#define PSTR(str) str
#endif

// Optional wordsets
#define __ARDUINO__

#endif
