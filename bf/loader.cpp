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
    loadSource(PSTR(": cell 4 ;           : addr 2 ;"));
    loadSource(PSTR(": (l) #20 ;          : last (l) @ ;"));
    loadSource(PSTR(": immediate 1 last addr + c! ;"));
    loadSource(PSTR(": inline    2 last addr + c! ;"));
    loadSource(PSTR(": tib    #8 @ ;      : >in   #12 ;"));
    loadSource(PSTR(": (h)   #16 ;        : here (h) @ ;"));
    loadSource(PSTR(": base  #24 ;        : state #28 ;"));
    loadSource(PSTR(": sp0   #32 @ ;      : rp0   #36 @ ;"));
    loadSource(PSTR(": (dsp) #40 ;        : dsp (dsp) @ ;"));
    loadSource(PSTR(": (rsp) #44 ;        : rsp (rsp) @ ;"));
    loadSource(PSTR(": !sp 0 (dsp) ! ;    : !rsp 0 (rsp) ! ;"));
    loadSource(PSTR(": cells 4 * ;        : cell+ 4 + ;"));
    loadSource(PSTR(": nip swap drop ;"));
    loadSource(PSTR(": tuck swap over ;"));
    loadSource(PSTR(": 2drop drop drop ;"));
    loadSource(PSTR(": 2dup over over ;"));
    loadSource(PSTR(": / /mod nip ;"));
    loadSource(PSTR(": mod /mod drop ;"));
    loadSource(PSTR(": */ >r * r> / ;"));
    loadSource(PSTR(": */mod >r * r> /mod ;"));
    loadSource(PSTR("// : + ;"));
    loadSource(PSTR(": +! tuck @ + swap ! ;"));
    loadSource(PSTR("// : - ;"));
    loadSource(PSTR("// : /mod ;"));
    loadSource(PSTR("// : < ;"));
    loadSource(PSTR("// : = ;"));
    loadSource(PSTR("// : > ;"));
    loadSource(PSTR("// : not ;"));
    loadSource(PSTR(": 0< 0 < ;"));
    loadSource(PSTR(": 0= 0 = ;"));
    loadSource(PSTR(": 0> 0 > ;"));
    loadSource(PSTR("// : 1+ ;"));
    loadSource(PSTR("// : 1- ;"));
    loadSource(PSTR(": 2+ 1+ 1+ ;"));
    loadSource(PSTR(": 2- 1- 1- ;"));
    loadSource(PSTR("// : 2/ ;"));
    loadSource(PSTR("// : >r ;"));
    loadSource(PSTR(": ?dup if- dup then ;"));
    loadSource(PSTR("// : @ ;"));
    loadSource(PSTR(": negate 0 swap - ;"));
    loadSource(PSTR(": abs dup 0< if negate then ;"));
    loadSource(PSTR("// : and ;"));
    loadSource(PSTR("// : c! ;"));
    loadSource(PSTR("// : c@ ;"));
    loadSource(PSTR("// : cmove ;"));
    loadSource(PSTR("// : cmove> ;"));
    loadSource(PSTR(": count dup 1+ swap c@ ;"));
    loadSource(PSTR(": d+ ;"));
    loadSource(PSTR(": d< ;"));
    loadSource(PSTR(": dnegate ;"));
    loadSource(PSTR("// : drop ;"));
    loadSource(PSTR("// : dup ;"));
    loadSource(PSTR(": execute >r ;"));
    loadSource(PSTR("// : exit ;"));
    loadSource(PSTR("// : fill ;"));
    loadSource(PSTR("// : i ;"));
    loadSource(PSTR("// : j ;"));
    loadSource(PSTR("// : else ;"));
    loadSource(PSTR("// : or ;"));
    loadSource(PSTR("// : over ;"));
    loadSource(PSTR(": depth dsp 1- ;"));
    loadSource(PSTR(": pick depth swap - 1- cells sp0 + @ ;"));
    loadSource(PSTR("// : r> ;"));
    loadSource(PSTR("// : r@ ;"));
    loadSource(PSTR(": roll ;"));
    loadSource(PSTR(": rot >r swap r> swap ;"));
    loadSource(PSTR(": -rot swap >r swap r> ;"));
    loadSource(PSTR(": min 2dup < if drop else nip then ;"));
    loadSource(PSTR(": max 2dup > if drop else nip then ;"));
    loadSource(PSTR(": between rot dup >r min max r> = ;"));
    loadSource(PSTR("// : swap ;"));
    loadSource(PSTR(": u< ;"));
    loadSource(PSTR(": um* ;"));
    loadSource(PSTR(": um/mod ;"));
    loadSource(PSTR("// : xor ;"));
    loadSource(PSTR(": block ;"));
    loadSource(PSTR(": buffer ;"));
    loadSource(PSTR(": cr #13 emit #10 emit ;"));
    loadSource(PSTR("// : emit ;"));
    loadSource(PSTR(": expect ;"));
    loadSource(PSTR(": flush ;"));
    loadSource(PSTR(": key ;"));
    loadSource(PSTR(": save-buffers ;"));
    loadSource(PSTR(": space $20 emit ;"));
    loadSource(PSTR(": spaces if- begin space 1- while- then drop ;"));
    loadSource(PSTR(": type if- begin >r dup c@ emit 1+ r> 1- while- then 2drop ;"));
    loadSource(PSTR(": update ;"));
    loadSource(PSTR(": # ;"));
    loadSource(PSTR(": #> ;"));
    loadSource(PSTR(": #s ;"));
    loadSource(PSTR(": #tib ;"));
    loadSource(PSTR(": ' ;"));
    loadSource(PSTR("// : ( ;"));
    loadSource(PSTR(": -trailing ;"));
    loadSource(PSTR("// : . ;"));
    loadSource(PSTR(": .( ;"));
    loadSource(PSTR(": <# ;"));
    loadSource(PSTR("// : >body ;"));
    loadSource(PSTR("// : >in ;"));
    loadSource(PSTR(": abort ;"));
    loadSource(PSTR("// : base ;"));
    loadSource(PSTR(": blk ;"));
    loadSource(PSTR(": convert ;"));
    loadSource(PSTR(": hex $10 base ! ;"));
    loadSource(PSTR(": decimal #10 base ! ;"));
    loadSource(PSTR(": definitions ;"));
    loadSource(PSTR("// : find ;"));
    loadSource(PSTR(": forget ;"));
    loadSource(PSTR(": forth ;"));
    loadSource(PSTR(": forth-83 ;"));
    loadSource(PSTR(": hold ;"));
    loadSource(PSTR(": load ;"));
    loadSource(PSTR(": pad ;"));
    loadSource(PSTR(": quit ;"));
    loadSource(PSTR(": sign ;"));
    loadSource(PSTR(": span ;"));
    loadSource(PSTR("// : tib ;"));
    loadSource(PSTR(": u. ;"));
    loadSource(PSTR("// : word ;"));
    loadSource(PSTR("// : +loop ;"));
    loadSource(PSTR("// : , ;"));
    loadSource(PSTR(": .\" ;"));
    loadSource(PSTR("// : : ;"));
    loadSource(PSTR("// : ; ;"));
    loadSource(PSTR(": abort\" ;"));
    loadSource(PSTR(": allot here + (h) ! ;"));
    loadSource(PSTR("// : begin ;"));
    loadSource(PSTR(": compile ;"));
    loadSource(PSTR("// : constant ;"));
    loadSource(PSTR("// : create ;"));
    loadSource(PSTR("// : do ;"));
    loadSource(PSTR(": does> ;"));
    loadSource(PSTR("// : else ;"));
    loadSource(PSTR("// : if ;"));
    loadSource(PSTR("// : leave ;"));
    loadSource(PSTR("// : literal ;"));
    loadSource(PSTR("// : loop ;"));
    loadSource(PSTR("// : repeat ;"));
    loadSource(PSTR("// : state ;"));
    loadSource(PSTR("// : then ;"));
    loadSource(PSTR("// : until ;"));
    loadSource(PSTR("// : variable ;"));
    loadSource(PSTR(": vocabulary ;"));
    loadSource(PSTR("// : while ;"));
    loadSource(PSTR(": [ 0 state ! ; immediate"));
    loadSource(PSTR(": ] 1 state ! ;"));
    loadSource(PSTR(": ['] [ ' ] ;"));
    loadSource(PSTR(": [compile] ; immediate"));
    loadSource(PSTR(": .word addr + 1+ count type ;"));
    loadSource(PSTR(": words last begin dup .word space a@ while- drop ;"));
}
// ^^^^^ - NimbleText generated - ^^^^^
