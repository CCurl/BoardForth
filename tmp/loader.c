#include "defs.h"


/* -- NimbleText script:
// vvvvv - NimbleText generated - vvvvv
$once
void loadSystem() {
$each
    parseLine(F("$row"));
$once
}
// ^^^^^ - NimbleText generated - ^^^^^
*/

// vvvvv - NimbleText generated - vvvvv
void loadSystem() {
    parseLine(F("// : ! ;"));
    parseLine(F("// : * ;"));
    parseLine(F(": nip swap drop ;"));
    parseLine(F(": tuck swap over ;"));
    parseLine(F(": 2DROP drop drop ;"));
    parseLine(F(": 2DUP over over ;"));
    parseLine(F(": / /mod nip ;"));
    parseLine(F(": mod /mod drop ;"));
    parseLine(F(": */ >r * r> / ;"));
    parseLine(F(": */MOD >r * r> /mod ;"));
    parseLine(F("// : + ;"));
    parseLine(F(": +! tuck @ + swap ! ;"));
    parseLine(F("// : - ;"));
    parseLine(F("// : /MOD ;"));
    parseLine(F(": < ;"));
    parseLine(F(": = ;"));
    parseLine(F(": > ;"));
    parseLine(F(": NOT ;"));
    parseLine(F(": 0< 0 < ;"));
    parseLine(F(": 0= NOT ;"));
    parseLine(F(": 0> 0 > ;"));
    parseLine(F("// : 1+ ;"));
    parseLine(F("// : 1- ;"));
    parseLine(F(": 2+ 1+ 1+ ;"));
    parseLine(F(": 2- 1- 1- ;"));
    parseLine(F("// : 2/ ;"));
    parseLine(F("// : >R ;"));
    parseLine(F(": ?DUP if- dup then ;"));
    parseLine(F("// : @ ;"));
    parseLine(F(": NEGATE 0 swap - ;"));
    parseLine(F(": ABS dup 0< if NEGATE then ;"));
    parseLine(F("// : AND ;"));
    parseLine(F("// : C! ;"));
    parseLine(F("// : C@ ;"));
    parseLine(F(": CMOVE ;"));
    parseLine(F(": CMOVE> ;"));
    parseLine(F(": COUNT dup 1+ swap c@ ;"));
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
    parseLine(F(": else ;"));
    parseLine(F(": MAX over over > if drop else nip then ;"));
    parseLine(F(": MIN over over < if drop else nip then ;"));
    parseLine(F("// : OR ;"));
    parseLine(F("// : OVER ;"));
    parseLine(F(": PICK ;"));
    parseLine(F("// : R> ;"));
    parseLine(F("// : R@ ;"));
    parseLine(F(": ROLL ;"));
    parseLine(F(": ROT swap >r swap r> ;"));
    parseLine(F("// : SWAP ;"));
    parseLine(F(": U< ;"));
    parseLine(F(": UM* ;"));
    parseLine(F(": UM/MOD ;"));
    parseLine(F("// : XOR ;"));
    parseLine(F(": BLOCK ;"));
    parseLine(F(": BUFFER ;"));
    parseLine(F(": CR #13 emit #10 emit ;"));
    parseLine(F("// : EMIT ;"));
    parseLine(F(": EXPECT ;"));
    parseLine(F(": FLUSH ;"));
    parseLine(F(": KEY ;"));
    parseLine(F(": SAVE-BUFFERS ;"));
    parseLine(F(": SPACE $20 emit ;"));
    parseLine(F(": SPACES ;"));
    parseLine(F(": TYPE if- begin >r dup emit 1+ r> 1- while- then 2DROP ;"));
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
    parseLine(F(": >IN ;"));
    parseLine(F(": ABORT ;"));
    parseLine(F("// : BASE ;"));
    parseLine(F(": BLK ;"));
    parseLine(F(": CONVERT ;"));
    parseLine(F(": DECIMAL #10 base ! ;"));
    parseLine(F(": DEFINITIONS ;"));
    parseLine(F("// : FIND ;"));
    parseLine(F(": FORGET ;"));
    parseLine(F(": FORTH ;"));
    parseLine(F(": FORTH-83 ;"));
    parseLine(F(": here (here) @ ;"));
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
    parseLine(F(": , ;"));
    parseLine(F(": .\" ;"));
    parseLine(F("// : : ;"));
    parseLine(F("// : ; ;"));
    parseLine(F(": ABORT\" ;"));
    parseLine(F(": ALLOT here + (here) ! ;"));
    parseLine(F("// : BEGIN ;"));
    parseLine(F(": COMPILE ;"));
    parseLine(F(": CONSTANT ;"));
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
    parseLine(F(": VARIABLE ;"));
    parseLine(F(": VOCABULARY ;"));
    parseLine(F("// : WHILE ;"));
    parseLine(F(": ['] ;"));
    parseLine(F(": [COMPILE] ;"));
    parseLine(F(": ] ;"));

}
// ^^^^^ - NimbleText generated - ^^^^^
