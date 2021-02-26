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
    loadSource(PSTR(": +! tuck @ + swap ! ;"));
    loadSource(PSTR(": 0< 0 < ;"));
    loadSource(PSTR(": 0= 0 = ;"));
    loadSource(PSTR(": 0> 0 > ;"));
    loadSource(PSTR(": 2+ 1+ 1+ ;"));
    loadSource(PSTR(": 2- 1- 1- ;"));
    loadSource(PSTR(": ?dup if- dup then ;"));
    loadSource(PSTR(": negate 0 swap - ;"));
    loadSource(PSTR(": abs dup 0< if negate then ;"));
    loadSource(PSTR(": count dup 1+ swap c@ ;"));
    loadSource(PSTR(": execute >r ;"));
    loadSource(PSTR(": depth dsp 1- ;"));
    loadSource(PSTR(": pick depth swap - 1- cells sp0 + @ ;"));
    loadSource(PSTR(": rot >r swap r> swap ;"));
    loadSource(PSTR(": -rot swap >r swap r> ;"));
    loadSource(PSTR(": min 2dup < if drop else nip then ;"));
    loadSource(PSTR(": max 2dup > if drop else nip then ;"));
    loadSource(PSTR(": between rot dup >r min max r> = ;"));
    loadSource(PSTR(": cr #13 emit #10 emit ;"));
    loadSource(PSTR(": space $20 emit ;"));
    loadSource(PSTR(": spaces if- begin space 1- while- then drop ;"));
    loadSource(PSTR(": type if- begin >r dup c@ emit 1+ r> 1- while- then 2drop ;"));
    loadSource(PSTR(": .( ;"));
    loadSource(PSTR(": hex $10 base ! ;"));
    loadSource(PSTR(": decimal #10 base ! ;"));
    loadSource(PSTR(": .\" ;"));
    loadSource(PSTR(": allot here + (h) ! ;"));
    loadSource(PSTR(": does> ;"));
    loadSource(PSTR(": [ 0 state ! ; immediate"));
    loadSource(PSTR(": ] 1 state ! ;"));
    loadSource(PSTR(": .word addr + 1+ count type ;"));
    loadSource(PSTR(": words last begin dup .word space a@ while- drop ;"));
}
// ^^^^^ - NimbleText generated - ^^^^^
