#ifndef __BOARD_H__
#define __BOARD_H__

// Boards ...
#define XIAO        0
#define PICO        1
#define TEENSY_4    2
#define TEENSY_LC   3
#define ESP32       4
#define ESP8266     5

#define __BOARD__ PICO

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
	#define __NEEDS_ALIGN__
	#define STK_SZ 32
	#define TIB_SZ (128)
	#define MAX_FILE_SZ (1024*10)
	#define PIN_INPUT INPUT
	#define PIN_INPUT_PULLUP INPUT_PULLUP
	#define PIN_OUTPUT OUTPUT
	#if __BOARD__ == XIAO
		#define DICT_SZ (1024*20)
		#define VARS_SZ (1024*4)
		#define __GAMEPAD__
		#include <HID-Project.h>
	#elif __BOARD__ == PICO
		#define DICT_SZ (1024*64)
		#define VARS_SZ (1024*96)
	#elif __BOARD__ == TEENSY_4
		#define DICT_SZ (1024*48)
		#define VARS_SZ (1024*64)
	#elif __BOARD__ == TEENSY_LC
		#define DICT_SZ (1024*16)
		#define VARS_SZ (1024*2)
	#endif
#else
	#define __IS_PC__
	#include <windows.h>
	#define DICT_SZ (1024*128)
	#define VARS_SZ (1024*256)
	#define STK_SZ 32
	#define TIB_SZ (128)
	#define MAX_FILE_SZ 10*1024
	#define __FILES__
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
