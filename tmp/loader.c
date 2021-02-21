#include "defs.h"


/* -- NimbleText script:
$once
// vvvvv - NimbleText generated - vvvvv
void loadBaseSystem() {
$each
    parseLine(F("$row"));
$once
}
// ^^^^^ - NimbleText generated - ^^^^^
*/

// vvvvv - NimbleText generated - vvvvv
void loadBaseSystem() {
    parseLine(F("// : ! ;"));
    parseLine(F("// : * ;"));
    parseLine(F(": TIB   8 @ ;     : >IN   12 ;"));
    parseLine(F(": (H)  16 ;       : (L)   20 ;"));
    parseLine(F(": BASE 24 ;       : STATE 28 ; "));
    parseLine(F(": SP0  32 @ ;     : RP0   36 @ ; "));
    parseLine(F(": (DSP) 40 ;      : (RSP) 44 ; "));
    parseLine(F(": DSP (DSP) @ ;   : RSP (RSP) @ ;"));
    parseLine(F(": !SP 0 (DSP) ! ; : !RSP 0 (RSP) ! ;"));
    parseLine(F(": HERE (H) @ ;    : LAST (L) @ ;"));
    parseLine(F(": CELL 4 ;        : ADDR 2 ; "));
    parseLine(F(": CELLS 4 * ;     : CELL+ 4 + ;"));
    parseLine(F(": NIP SWAP DROP ;"));
    parseLine(F(": TUCK SWAP OVER ;"));
    parseLine(F(": 2DROP DROP DROP ;"));
    parseLine(F(": 2DUP OVER OVER ;"));
    parseLine(F(": / /MOD NIP ;"));
    parseLine(F(": MOD /MOD DROP ;"));
    parseLine(F(": */ >R * R> / ;"));
    parseLine(F(": */MOD >R * R> /MOD ;"));
    parseLine(F("// : + ;"));
    parseLine(F(": +! TUCK @ + SWAP ! ;"));
    parseLine(F("// : - ;"));
    parseLine(F("// : /MOD ;"));
    parseLine(F("// : < ;"));
    parseLine(F("// : = ;"));
    parseLine(F("// : > ;"));
    parseLine(F("// : NOT ;"));
    parseLine(F(": 0< 0 < ;"));
    parseLine(F(": 0= 0 = ;"));
    parseLine(F(": 0> 0 > ;"));
    parseLine(F("// : 1+ ;"));
    parseLine(F("// : 1- ;"));
    parseLine(F(": 2+ 1+ 1+ ;"));
    parseLine(F(": 2- 1- 1- ;"));
    parseLine(F("// : 2/ ;"));
    parseLine(F("// : >R ;"));
    parseLine(F(": ?DUP IF- DUP THEN ;"));
    parseLine(F("// : @ ;"));
    parseLine(F(": NEGATE 0 SWAP - ;"));
    parseLine(F(": ABS DUP 0< IF NEGATE THEN ;"));
    parseLine(F("// : AND ;"));
    parseLine(F("// : C! ;"));
    parseLine(F("// : C@ ;"));
    parseLine(F(": CMOVE ;"));
    parseLine(F(": CMOVE> ;"));
    parseLine(F(": COUNT DUP 1+ SWAP C@ ;"));
    parseLine(F(": D+ ;"));
    parseLine(F(": D< ;"));
    parseLine(F(": DEPTH ;"));
    parseLine(F(": DNEGATE ;"));
    parseLine(F("// : DROP ;"));
    parseLine(F("// : DUP ;"));
    parseLine(F(": EXECUTE ;"));
    parseLine(F(": EXIT ;"));
    parseLine(F(": FILL ;"));
    parseLine(F(": I ;"));
    parseLine(F(": J ;"));
    parseLine(F("// : ELSE ;"));
    parseLine(F("// : OR ;"));
    parseLine(F("// : OVER ;"));
    parseLine(F(": DEPTH DSP 1- ;"));
    parseLine(F(": PICK DEPTH SWAP - 1- CELLS SP0 + @ ;"));
    parseLine(F("// : R> ;"));
    parseLine(F("// : R@ ;"));
    parseLine(F(": ROLL ;"));
    parseLine(F(": ROT >R SWAP R> SWAP ;"));
    parseLine(F(": -ROT SWAP >R SWAP R> ;"));
    parseLine(F(": MIN 2DUP < IF DROP ELSE NIP THEN ;"));
    parseLine(F(": MAX 2DUP > IF DROP ELSE NIP THEN ;"));
    parseLine(F(": BETWEEN ROT DUP >R MIN MAX R> = ;"));
    parseLine(F("// : SWAP ;"));
    parseLine(F(": U< ;"));
    parseLine(F(": UM* ;"));
    parseLine(F(": UM/MOD ;"));
    parseLine(F("// : XOR ;"));
    parseLine(F(": BLOCK ;"));
    parseLine(F(": BUFFER ;"));
    parseLine(F(": CR #13 EMIT #10 EMIT ;"));
    parseLine(F("// : EMIT ;"));
    parseLine(F(": EXPECT ;"));
    parseLine(F(": FLUSH ;"));
    parseLine(F(": KEY ;"));
    parseLine(F(": SAVE-BUFFERS ;"));
    parseLine(F(": SPACE $20 EMIT ;"));
    parseLine(F(": SPACES IF- BEGIN SPACE 1- WHILE- THEN DROP ;"));
    parseLine(F(": TYPE IF- BEGIN >R DUP C@ EMIT 1+ R> 1- WHILE- THEN 2DROP ;"));
    parseLine(F(": UPDATE ;"));
    parseLine(F(": # ;"));
    parseLine(F(": #> ;"));
    parseLine(F(": #S ;"));
    parseLine(F(": #TIB ;"));
    parseLine(F(": ' ;"));
    parseLine(F("// : ( ;"));
    parseLine(F(": -TRAILING ;"));
    parseLine(F("// : . ;"));
    parseLine(F(": .( ;"));
    parseLine(F(": <# ;"));
    parseLine(F("// : >BODY ;"));
    parseLine(F("// : >IN ;"));
    parseLine(F(": ABORT ;"));
    parseLine(F("// : BASE ;"));
    parseLine(F(": BLK ;"));
    parseLine(F(": CONVERT ;"));
    parseLine(F(": HEX $10 BASE ! ;"));
    parseLine(F(": DECIMAL #10 BASE ! ;"));
    parseLine(F(": DEFINITIONS ;"));
    parseLine(F("// : FIND ;"));
    parseLine(F(": FORGET ;"));
    parseLine(F(": FORTH ;"));
    parseLine(F(": FORTH-83 ;"));
    parseLine(F(": HOLD ;"));
    parseLine(F(": LOAD ;"));
    parseLine(F(": PAD ;"));
    parseLine(F(": QUIT ;"));
    parseLine(F(": SIGN ;"));
    parseLine(F(": SPAN ;"));
    parseLine(F("// : TIB ;"));
    parseLine(F(": U. ;"));
    parseLine(F("// : WORD ;"));
    parseLine(F(": +LOOP ;"));
    parseLine(F("// : , ;"));
    parseLine(F(": .\" ;"));
    parseLine(F("// : : ;"));
    parseLine(F("// : ; ;"));
    parseLine(F(": ABORT\" ;"));
    parseLine(F(": ALLOT HERE + (H) ! ;"));
    parseLine(F("// : BEGIN ;"));
    parseLine(F(": COMPILE ;"));
    parseLine(F("// : CONSTANT ;"));
    parseLine(F("// : CREATE ;"));
    parseLine(F(": DO ;"));
    parseLine(F(": DOES> ;"));
    parseLine(F("// : ELSE ;"));
    parseLine(F("// : IF ;"));
    parseLine(F(": IMMEDIATE ;"));
    parseLine(F("// : LEAVE ;"));
    parseLine(F(": LITERAL ;"));
    parseLine(F(": LOOP ;"));
    parseLine(F(": REPEAT ;"));
    parseLine(F("// : STATE ;"));
    parseLine(F("// : THEN ;"));
    parseLine(F("// : UNTIL ;"));
    parseLine(F("// : VARIABLE ;"));
    parseLine(F(": VOCABULARY ;"));
    parseLine(F("// : WHILE ;"));
    parseLine(F(": [ 0 STATE ! ;"));
    parseLine(F(": ] 1 STATE ! ;"));
    parseLine(F(": ['] [ ' ] ;"));
    parseLine(F(": [COMPILE] ;"));
    parseLine(F(": .WORD ADDR + 1+ COUNT TYPE ;"));
    parseLine(F(": WORDS LAST BEGIN DUP .WORD SPACE A@ WHILE- DROP ;"));
}
// ^^^^^ - NimbleText generated - ^^^^^