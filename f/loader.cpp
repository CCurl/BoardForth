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
    parseLine_P(F("// : ! ;"));
    parseLine_P(F("// : * ;"));
    parseLine_P(F(": CELL 4 ;           : ADDR 2 ;"));
    parseLine_P(F(": (L) #20 ;          : LAST (L) @ ;"));
    parseLine_P(F(": IMMEDIATE 1 LAST ADDR + C! ;"));
    parseLine_P(F(": INLINE    2 LAST ADDR + C! ;"));
    parseLine_P(F(": TIB    #8 @ ;      : >IN  #12 ;"));
    parseLine_P(F(": (H)   #16 ;        : HERE (H) @ ;"));
    parseLine_P(F(": BASE  #24 ;        : STATE #28 ;"));
    parseLine_P(F(": SP0   #32 @ ;      : RP0   #36 @ ;"));
    parseLine_P(F(": (DSP) #40 ;        : DSP (DSP) @ ;"));
    parseLine_P(F(": (RSP) #44 ;        : RSP (RSP) @ ;"));
    parseLine_P(F(": !SP 0 (DSP) ! ;    : !RSP 0 (RSP) ! ;"));
    parseLine_P(F(": CELLS 4 * ;        : CELL+ 4 + ;"));
    parseLine_P(F(": NIP SWAP DROP ;"));
    parseLine_P(F(": TUCK SWAP OVER ;"));
    parseLine_P(F(": 2DROP DROP DROP ;"));
    parseLine_P(F(": 2DUP OVER OVER ;"));
    parseLine_P(F(": / /MOD NIP ;"));
    parseLine_P(F(": MOD /MOD DROP ;"));
    parseLine_P(F(": */ >R * R> / ;"));
    parseLine_P(F(": */MOD >R * R> /MOD ;"));
    parseLine_P(F("// : + ;"));
    parseLine_P(F(": +! TUCK @ + SWAP ! ;"));
    parseLine_P(F("// : - ;"));
    parseLine_P(F("// : /MOD ;"));
    parseLine_P(F("// : < ;"));
    parseLine_P(F("// : = ;"));
    parseLine_P(F("// : > ;"));
    parseLine_P(F("// : NOT ;"));
    parseLine_P(F(": 0< 0 < ;"));
    parseLine_P(F(": 0= 0 = ;"));
    parseLine_P(F(": 0> 0 > ;"));
    parseLine_P(F("// : 1+ ;"));
    parseLine_P(F("// : 1- ;"));
    parseLine_P(F(": 2+ 1+ 1+ ;"));
    parseLine_P(F(": 2- 1- 1- ;"));
    parseLine_P(F("// : 2/ ;"));
    parseLine_P(F("// : >R ;"));
    parseLine_P(F(": ?DUP IF- DUP THEN ;"));
    parseLine_P(F("// : @ ;"));
    parseLine_P(F(": NEGATE 0 SWAP - ;"));
    parseLine_P(F(": ABS DUP 0< IF NEGATE THEN ;"));
    parseLine_P(F("// : AND ;"));
    parseLine_P(F("// : C! ;"));
    parseLine_P(F("// : C@ ;"));
    parseLine_P(F("// : CMOVE ;"));
    parseLine_P(F("// : CMOVE> ;"));
    parseLine_P(F(": COUNT DUP 1+ SWAP C@ ;"));
    parseLine_P(F(": D+ ;"));
    parseLine_P(F(": D< ;"));
    parseLine_P(F(": DNEGATE ;"));
    parseLine_P(F("// : DROP ;"));
    parseLine_P(F("// : DUP ;"));
    parseLine_P(F(": EXECUTE ;"));
    parseLine_P(F("// : EXIT ; IMMEDIATE"));
    parseLine_P(F("// : FILL ;"));
    parseLine_P(F("// : I ;"));
    parseLine_P(F("// : J ;"));
    parseLine_P(F("// : ELSE ;"));
    parseLine_P(F("// : OR ;"));
    parseLine_P(F("// : OVER ;"));
    parseLine_P(F(": DEPTH DSP 1- ;"));
    parseLine_P(F(": PICK DEPTH SWAP - 1- CELLS SP0 + @ ;"));
    parseLine_P(F("// : R> ;"));
    parseLine_P(F("// : R@ ;"));
    parseLine_P(F(": ROLL ;"));
    parseLine_P(F(": ROT >R SWAP R> SWAP ;"));
    parseLine_P(F(": -ROT SWAP >R SWAP R> ;"));
    parseLine_P(F(": MIN 2DUP < IF DROP ELSE NIP THEN ;"));
    parseLine_P(F(": MAX 2DUP > IF DROP ELSE NIP THEN ;"));
    parseLine_P(F(": BETWEEN ROT DUP >R MIN MAX R> = ;"));
    parseLine_P(F("// : SWAP ;"));
    parseLine_P(F(": U< ;"));
    parseLine_P(F(": UM* ;"));
    parseLine_P(F(": UM/MOD ;"));
    parseLine_P(F("// : XOR ;"));
    parseLine_P(F(": BLOCK ;"));
    parseLine_P(F(": BUFFER ;"));
    parseLine_P(F(": CR #13 EMIT #10 EMIT ;"));
    parseLine_P(F("// : EMIT ;"));
    parseLine_P(F(": EXPECT ;"));
    parseLine_P(F(": FLUSH ;"));
    parseLine_P(F(": KEY ;"));
    parseLine_P(F(": SAVE-BUFFERS ;"));
    parseLine_P(F(": SPACE $20 EMIT ;"));
    parseLine_P(F(": SPACES IF- BEGIN SPACE 1- WHILE- THEN DROP ;"));
    parseLine_P(F(": TYPE IF- BEGIN >R DUP C@ EMIT 1+ R> 1- WHILE- THEN 2DROP ;"));
    parseLine_P(F(": UPDATE ;"));
    parseLine_P(F(": # ;"));
    parseLine_P(F(": #> ;"));
    parseLine_P(F(": #S ;"));
    parseLine_P(F(": #TIB ;"));
    parseLine_P(F(": ' ;"));
    parseLine_P(F("// : ( ;"));
    parseLine_P(F(": -TRAILING ;"));
    parseLine_P(F("// : . ;"));
    parseLine_P(F(": .( ;"));
    parseLine_P(F(": <# ;"));
    parseLine_P(F("// : >BODY ;"));
    parseLine_P(F("// : >IN ;"));
    parseLine_P(F(": ABORT ;"));
    parseLine_P(F("// : BASE ;"));
    parseLine_P(F(": BLK ;"));
    parseLine_P(F(": CONVERT ;"));
    parseLine_P(F(": HEX $10 BASE ! ;"));
    parseLine_P(F(": DECIMAL #10 BASE ! ;"));
    parseLine_P(F(": DEFINITIONS ;"));
    parseLine_P(F("// : FIND ;"));
    parseLine_P(F(": FORGET ;"));
    parseLine_P(F(": FORTH ;"));
    parseLine_P(F(": FORTH-83 ;"));
    parseLine_P(F(": HOLD ;"));
    parseLine_P(F(": LOAD ;"));
    parseLine_P(F(": PAD ;"));
    parseLine_P(F(": QUIT ;"));
    parseLine_P(F(": SIGN ;"));
    parseLine_P(F(": SPAN ;"));
    parseLine_P(F("// : TIB ;"));
    parseLine_P(F(": U. ;"));
    parseLine_P(F("// : WORD ;"));
    parseLine_P(F("// : +LOOP ;"));
    parseLine_P(F("// : , ;"));
    parseLine_P(F(": .\" ;"));
    parseLine_P(F("// : : ;"));
    parseLine_P(F("// : ; ;"));
    parseLine_P(F(": ABORT\" ;"));
    parseLine_P(F(": ALLOT HERE + (H) ! ;"));
    parseLine_P(F("// : BEGIN ;"));
    parseLine_P(F(": COMPILE ;"));
    parseLine_P(F("// : CONSTANT ;"));
    parseLine_P(F("// : CREATE ;"));
    parseLine_P(F("// : DO ;"));
    parseLine_P(F(": DOES> ;"));
    parseLine_P(F("// : ELSE ;"));
    parseLine_P(F("// : IF ;"));
    parseLine_P(F("// : LEAVE ;"));
    parseLine_P(F(": LITERAL ;"));
    parseLine_P(F("// : LOOP ;"));
    parseLine_P(F(": REPEAT ;"));
    parseLine_P(F("// : STATE ;"));
    parseLine_P(F("// : THEN ;"));
    parseLine_P(F("// : UNTIL ;"));
    parseLine_P(F("// : VARIABLE ;"));
    parseLine_P(F(": VOCABULARY ;"));
    parseLine_P(F("// : WHILE ;"));
    parseLine_P(F(": [ 0 STATE ! ;"));
    parseLine_P(F(": ] 1 STATE ! ;"));
    parseLine_P(F(": ['] [ ' ] ;"));
    parseLine_P(F(": [COMPILE] ; IMMEDIATE"));
    parseLine_P(F(": .WORD ADDR + 1+ COUNT TYPE ;"));
    parseLine_P(F(": WORDS LAST BEGIN DUP .WORD SPACE A@ WHILE- DROP ;"));
}
// ^^^^^ - NimbleText generated - ^^^^^
