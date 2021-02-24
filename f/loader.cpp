#include"defs.h"


/* -- NimbleText script:
$once
// vvvvv - NimbleText generated - vvvvv
void loadBaseSystem() {
$each
    parseLine_P(F("$row"));
$once
}
// ^^^^^ - NimbleText generated - ^^^^^
*/

// vvvvv - NimbleText generated - vvvvv
void loadBaseSystem() {
    loadSource(PSTR("// : ! ;"));
    loadSource(PSTR("// : * ;"));
    loadSource(PSTR(": CELL 4 ;           : ADDR 2 ;"));
    loadSource(PSTR(": (L) #20 ;          : LAST (L) @ ;"));
    loadSource(PSTR(": IMMEDIATE 1 LAST ADDR + C! ;"));
    loadSource(PSTR(": INLINE    2 LAST ADDR + C! ;"));
    loadSource(PSTR(": TIB    #8 @ ;      : >IN   #12 ;"));
    loadSource(PSTR(": (H)   #16 ;        : HERE (H) @ ;"));
    loadSource(PSTR(": BASE  #24 ;        : STATE #28 ;"));
    loadSource(PSTR(": SP0   #32 @ ;      : RP0   #36 @ ;"));
    loadSource(PSTR(": (DSP) #40 ;        : DSP (DSP) @ ;"));
    loadSource(PSTR(": (RSP) #44 ;        : RSP (RSP) @ ;"));
    loadSource(PSTR(": !SP 0 (DSP) ! ;    : !RSP 0 (RSP) ! ;"));
    loadSource(PSTR(": CELLS 4 * ;        : CELL+ 4 + ;"));
    loadSource(PSTR(": NIP SWAP DROP ;"));
    loadSource(PSTR(": TUCK SWAP OVER ;"));
    loadSource(PSTR(": 2DROP DROP DROP ;"));
    loadSource(PSTR(": 2DUP OVER OVER ;"));
    loadSource(PSTR(": / /MOD NIP ;"));
    loadSource(PSTR(": MOD /MOD DROP ;"));
    loadSource(PSTR(": */ >R * R> / ;"));
    loadSource(PSTR(": */MOD >R * R> /MOD ;"));
    loadSource(PSTR("// : + ;"));
    loadSource(PSTR(": +! TUCK @ + SWAP ! ;"));
    loadSource(PSTR("// : - ;"));
    loadSource(PSTR("// : /MOD ;"));
    loadSource(PSTR("// : < ;"));
    loadSource(PSTR("// : = ;"));
    loadSource(PSTR("// : > ;"));
    loadSource(PSTR("// : NOT ;"));
    loadSource(PSTR(": 0< 0 < ;"));
    loadSource(PSTR(": 0= 0 = ;"));
    loadSource(PSTR(": 0> 0 > ;"));
    loadSource(PSTR("// : 1+ ;"));
    loadSource(PSTR("// : 1- ;"));
    loadSource(PSTR(": 2+ 1+ 1+ ;"));
    loadSource(PSTR(": 2- 1- 1- ;"));
    loadSource(PSTR("// : 2/ ;"));
    loadSource(PSTR("// : >R ;"));
    loadSource(PSTR(": ?DUP IF- DUP THEN ;"));
    loadSource(PSTR("// : @ ;"));
    loadSource(PSTR(": NEGATE 0 SWAP - ;"));
    loadSource(PSTR(": ABS DUP 0< IF NEGATE THEN ;"));
    loadSource(PSTR("// : AND ;"));
    loadSource(PSTR("// : C! ;"));
    loadSource(PSTR("// : C@ ;"));
    loadSource(PSTR("// : CMOVE ;"));
    loadSource(PSTR("// : CMOVE> ;"));
    loadSource(PSTR(": COUNT DUP 1+ SWAP C@ ;"));
    loadSource(PSTR(": D+ ;"));
    loadSource(PSTR(": D< ;"));
    loadSource(PSTR(": DNEGATE ;"));
    loadSource(PSTR("// : DROP ;"));
    loadSource(PSTR("// : DUP ;"));
    loadSource(PSTR(": EXECUTE >R ;"));
    loadSource(PSTR("// : EXIT ;"));
    loadSource(PSTR("// : FILL ;"));
    loadSource(PSTR("// : I ;"));
    loadSource(PSTR("// : J ;"));
    loadSource(PSTR("// : ELSE ;"));
    loadSource(PSTR("// : OR ;"));
    loadSource(PSTR("// : OVER ;"));
    loadSource(PSTR(": DEPTH DSP 1- ;"));
    loadSource(PSTR(": PICK DEPTH SWAP - 1- CELLS SP0 + @ ;"));
    loadSource(PSTR("// : R> ;"));
    loadSource(PSTR("// : R@ ;"));
    loadSource(PSTR(": ROLL ;"));
    loadSource(PSTR(": ROT >R SWAP R> SWAP ;"));
    loadSource(PSTR(": -ROT SWAP >R SWAP R> ;"));
    loadSource(PSTR(": MIN 2DUP < IF DROP ELSE NIP THEN ;"));
    loadSource(PSTR(": MAX 2DUP > IF DROP ELSE NIP THEN ;"));
    loadSource(PSTR(": BETWEEN ROT DUP >R MIN MAX R> = ;"));
    loadSource(PSTR("// : SWAP ;"));
    loadSource(PSTR(": U< ;"));
    loadSource(PSTR(": UM* ;"));
    loadSource(PSTR(": UM/MOD ;"));
    loadSource(PSTR("// : XOR ;"));
    loadSource(PSTR(": BLOCK ;"));
    loadSource(PSTR(": BUFFER ;"));
    loadSource(PSTR(": CR #13 EMIT #10 EMIT ;"));
    loadSource(PSTR("// : EMIT ;"));
    loadSource(PSTR(": EXPECT ;"));
    loadSource(PSTR(": FLUSH ;"));
    loadSource(PSTR(": KEY ;"));
    loadSource(PSTR(": SAVE-BUFFERS ;"));
    loadSource(PSTR(": SPACE $20 EMIT ;"));
    loadSource(PSTR(": SPACES IF- BEGIN SPACE 1- WHILE- THEN DROP ;"));
    loadSource(PSTR(": TYPE IF- BEGIN >R DUP C@ EMIT 1+ R> 1- WHILE- THEN 2DROP ;"));
    loadSource(PSTR(": UPDATE ;"));
    loadSource(PSTR(": # ;"));
    loadSource(PSTR(": #> ;"));
    loadSource(PSTR(": #S ;"));
    loadSource(PSTR(": #TIB ;"));
    loadSource(PSTR(": ' ;"));
    loadSource(PSTR("// : ( ;"));
    loadSource(PSTR(": -TRAILING ;"));
    loadSource(PSTR("// : . ;"));
    loadSource(PSTR(": .( ;"));
    loadSource(PSTR(": <# ;"));
    loadSource(PSTR("// : >BODY ;"));
    loadSource(PSTR("// : >IN ;"));
    loadSource(PSTR(": ABORT ;"));
    loadSource(PSTR("// : BASE ;"));
    loadSource(PSTR(": BLK ;"));
    loadSource(PSTR(": CONVERT ;"));
    loadSource(PSTR(": HEX $10 BASE ! ;"));
    loadSource(PSTR(": DECIMAL #10 BASE ! ;"));
    loadSource(PSTR(": DEFINITIONS ;"));
    loadSource(PSTR("// : FIND ;"));
    loadSource(PSTR(": FORGET ;"));
    loadSource(PSTR(": FORTH ;"));
    loadSource(PSTR(": FORTH-83 ;"));
    loadSource(PSTR(": HOLD ;"));
    loadSource(PSTR(": LOAD ;"));
    loadSource(PSTR(": PAD ;"));
    loadSource(PSTR(": QUIT ;"));
    loadSource(PSTR(": SIGN ;"));
    loadSource(PSTR(": SPAN ;"));
    loadSource(PSTR("// : TIB ;"));
    loadSource(PSTR(": U. ;"));
    loadSource(PSTR("// : WORD ;"));
    loadSource(PSTR("// : +LOOP ;"));
    loadSource(PSTR("// : , ;"));
    loadSource(PSTR(": .\" ;"));
    loadSource(PSTR("// : : ;"));
    loadSource(PSTR("// : ; ;"));
    loadSource(PSTR(": ABORT\" ;"));
    loadSource(PSTR(": ALLOT HERE + (H) ! ;"));
    loadSource(PSTR("// : BEGIN ;"));
    loadSource(PSTR(": COMPILE ;"));
    loadSource(PSTR("// : CONSTANT ;"));
    loadSource(PSTR("// : CREATE ;"));
    loadSource(PSTR("// : DO ;"));
    loadSource(PSTR(": DOES> ;"));
    loadSource(PSTR("// : ELSE ;"));
    loadSource(PSTR("// : IF ;"));
    loadSource(PSTR("// : LEAVE ;"));
    loadSource(PSTR("// : LITERAL ;"));
    loadSource(PSTR("// : LOOP ;"));
    loadSource(PSTR("// : REPEAT ;"));
    loadSource(PSTR("// : STATE ;"));
    loadSource(PSTR("// : THEN ;"));
    loadSource(PSTR("// : UNTIL ;"));
    loadSource(PSTR("// : VARIABLE ;"));
    loadSource(PSTR(": VOCABULARY ;"));
    loadSource(PSTR("// : WHILE ;"));
    loadSource(PSTR(": [ 0 STATE ! ; IMMEDIATE"));
    loadSource(PSTR(": ] 1 STATE ! ;"));
    loadSource(PSTR(": ['] [ ' ] ;"));
    loadSource(PSTR(": [COMPILE] ; IMMEDIATE"));
    loadSource(PSTR(": .WORD ADDR + 1+ COUNT TYPE ;"));
    loadSource(PSTR(": WORDS LAST BEGIN DUP .WORD SPACE A@ WHILE- DROP ;"));
}
// ^^^^^ - NimbleText generated - ^^^^^
