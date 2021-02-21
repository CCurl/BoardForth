// : ! ;
// : * ;
: TIB   8 @ ;     : >IN   12 ;
: (H)  16 ;       : (L)   20 ;
: BASE 24 ;       : STATE 28 ; 
: SP0  32 @ ;     : RP0   36 @ ; 
: (DSP) 40 ;      : (RSP) 44 ; 
: DSP (DSP) @ ;   : RSP (RSP) @ ;
: !SP 0 (DSP) ! ; : !RSP 0 (RSP) ! ;
: HERE (H) @ ;    : LAST (L) @ ;
: CELL 4 ;        : ADDR 2 ; 
: CELLS 4 * ;     : CELL+ 4 + ;
: nip swap drop ;
: tuck swap over ;
: 2DROP drop drop ;
: 2DUP over over ;
: / /mod nip ;
: mod /mod drop ;
: */ >r * r> / ;
: */MOD >r * r> /mod ;
// : + ;
: +! tuck @ + swap ! ;
// : - ;
// : /MOD ;
// : < ;
// : = ;
// : > ;
// : NOT ;
: 0< 0 < ;
: 0= 0 = ;
: 0> 0 > ;
// : 1+ ;
// : 1- ;
: 2+ 1+ 1+ ;
: 2- 1- 1- ;
// : 2/ ;
// : >R ;
: ?DUP if- dup then ;
// : @ ;
: NEGATE 0 swap - ;
: ABS dup 0< if NEGATE then ;
// : AND ;
// : C! ;
// : C@ ;
: CMOVE ;
: CMOVE> ;
: COUNT dup 1+ swap c@ ;
: D+ ;
: D< ;
: DEPTH ;
: DNEGATE ;
// : DROP ;
// : DUP ;
: EXECUTE ;
: EXIT ;
: FILL ;
: I ;
: J ;
: else ;
: MAX 2DUP > if drop else nip then ;
: MIN 2DUP < if drop else nip then ;
// : OR ;
// : OVER ;
: DEPTH DSP 1- ;
: PICK DEPTH swap - 1- CELLS SP0 + @ ;
// : R> ;
// : R@ ;
: ROLL ;
: ROT >r swap r> swap ;
: -ROT swap >r swap r> ;
// : SWAP ;
: U< ;
: UM* ;
: UM/MOD ;
// : XOR ;
: BLOCK ;
: BUFFER ;
: CR #13 emit #10 emit ;
// : EMIT ;
: EXPECT ;
: FLUSH ;
: KEY ;
: SAVE-BUFFERS ;
: SPACE $20 emit ;
: SPACES if- begin SPACE 1- while- then drop ;
: TYPE if- begin >r dup c@ emit 1+ r> 1- while- then 2DROP ;
: UPDATE ;
: # ;
: #> ;
: #S ;
: #TIB ;
: ' ;
// : ( ;
: -TRAILING ;
// : . ;
: .( ;
: <# ;
// : >BODY ;
// : >IN ;
: ABORT ;
// : BASE ;
: BLK ;
: CONVERT ;
: HEX $10 base ! ;
: DECIMAL #10 base ! ;
: DEFINITIONS ;
// : FIND ;
: FORGET ;
: FORTH ;
: FORTH-83 ;
: here (here) @ ;
: last (last) @ ;
: HOLD ;
: LOAD ;
: PAD ;
: QUIT ;
: SIGN ;
: SPAN ;
// : TIB ;
: U. ;
// : WORD ;
: +LOOP ;
// : , ;
: .\" ;
// : : ;
// : ; ;
: ABORT\" ;
: ALLOT here + (here) ! ;
// : BEGIN ;
: COMPILE ;
// : CONSTANT ;
// : CREATE ;
: DO ;
: DOES> ;
// : ELSE ;
// : IF ;
: IMMEDIATE ;
// : LEAVE ;
: LITERAL ;
: LOOP ;
: REPEAT ;
// : STATE ;
// : THEN ;
// : UNTIL ;
// : VARIABLE ;
: VOCABULARY ;
// : WHILE ;
: [ 0 state ! ;
: ] 1 state ! ;
: ['] [ ' ] ;
: [COMPILE] ;
: .word ADDR + 1+ COUNT TYPE ;
: words last begin dup .word SPACE a@ while- drop ;
