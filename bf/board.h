#ifndef __BOARD_H__
#define __BOARD_H__

#ifndef _WIN32
	#define __DEV_BOARD__
	#define __SERIAL__
	#include <Arduino.h>
	#define theSerial SerialUSB
	// #define theSerial Serial
	// #define __ESP32__
	// #define __LITTLEFS__
	// #include <LittleFS.h>
	#define __ARDUINO__
	#define __GAMEPAD__
	#include <HID-Project.h>
	#define __NEEDS_ALIGN__
	#define DICT_SZ (1024*20)
	#define VARS_SZ (1024*4)
	#define STK_SZ 32
	#define TIB_SZ (128)
	#define MAX_FILE_SZ (1024*10)
	#define PIN_INPUT INPUT
	#define PIN_INPUT_PULLUP INPUT_PULLUP
	#define PIN_OUTPUT OUTPUT
#else
	#define __IS_PC__
	#include <windows.h>
	#define DICT_SZ (1024*128)
	#define VARS_SZ (1024*256)
	#define STK_SZ 32
	#define TIB_SZ (128)
	#define MAX_FILE_SZ 10*1024
	#define __FILES__
	#define __USER_WORDS__
	// #define __ARDUINO__
	// #define __ARDUINO_FAKE__
	// #define __GAMEPAD_FAKE__
	#define __COM_PORT__
	typedef unsigned int uint;
	typedef unsigned long ulong;
	#define PSTR(str) str
	#define PROGMEM
#endif

#endif
