#ifndef __DEFS_H__
#define __DEFS_H__

#define _CRT_SECURE_NO_WARNINGS

#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef void (*FP)();
typedef long  CELL;
typedef unsigned long  UCELL;
typedef unsigned short ADDR;
typedef unsigned short WORD;
typedef unsigned char  BYTE;

#define CELL_SZ (4)
#define WORD_SZ (2)
#define ADDR_SZ (2)
#define BLOCK_SZ (1024)

#define T dstk[sys->DSP]
#define N dstk[sys->DSP-1]
#define R rstk[sys->RSP]

typedef struct {
    ADDR XT;
    BYTE dictionaryId;
    BYTE flagsLen;
    BYTE name[20];
} DICT_T;

typedef struct {
    CELL autoRun;
    BYTE currentDictId;
    BYTE S4Mode;
    BYTE Reserved2;
    BYTE Reserved3;
    CELL TIB;
    CELL TOIN;
    CELL HERE;
    CELL LAST;
    CELL BASE;
    CELL STATE;
    CELL DSTACK;
    CELL RSTACK;
    CELL DSP;
    CELL RSP;
} SYSVARS_T;

#define ADDR_AUTORUN    (CELL_SZ*0)
#define ADDR_RES_1      (CELL_SZ*1)
#define ADDR_TIB        (CELL_SZ*2)
#define ADDR_TOIN       (CELL_SZ*3)
#define ADDR_HERE       (CELL_SZ*4)
#define ADDR_LAST       (CELL_SZ*5)
#define ADDR_BASE       (CELL_SZ*6)
#define ADDR_STATE      (CELL_SZ*7)
#define ADDR_DSTK       (CELL_SZ*8)
#define ADDR_RSTK       (CELL_SZ*9)
#define ADDR_DSP        (CELL_SZ*10)
#define ADDR_RSP        (CELL_SZ*11)

#define ADDR_HERE_BASE  (CELL_SZ*12)

extern BYTE dict[];
extern DICT_T words[];
extern SYSVARS_T* sys;

void dumpAll();
void dumpCode();
void dumpDict(int);
void dumpRegs();
void dumpStack(int);
void vmInit();
void push(CELL);
CELL pop();
void rpush(CELL);
CELL rpop();
void run(CELL, CELL);
BYTE nextChar();
void CCOMMA(BYTE v);
void WCOMMA(WORD v);
void COMMA(CELL v);
void parseLine(char*);
void loadBaseSystem();
void printString(const char* str);
void printStringF(const char* fmt, ...);
void loadUserWords();
void ok();
void autoRun();
CELL cellAt(CELL);
CELL wordAt(CELL);
CELL addrAt(CELL);
int inAddrSpace(CELL);
void wordStore(CELL addr, CELL val);
void cellStore(CELL addr, CELL val);
void addrStore(CELL addr, CELL val);
void doBlockRead();
void doBlockLoad();
CELL doNumber(CELL);
int isNumber(char *);
void doNumOut(CELL, int);
CELL s4Number(CELL, int);
CELL s4NumberAt(CELL);
CELL s4addrAt(CELL);

void fPARSEWORD();       // OP_PARSEWORD
void fPARSELINE();       // OP_PARSELINE
void fCREATE();          // OP_CREATE
void fFIND();            // OP_FIND
void fNEXTWORD();        // OP_NEXTWORD
void fISNUMBER();        // OP_ISNUMBER
void fNUM2STR();         // OP_NUM2STR
void fDUMPCODE();

#endif // __DEFS_H__
