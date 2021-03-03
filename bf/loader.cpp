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
    loadSource(PSTR("// : tib    #8 @ ;      : >in   #12 ;"));
    loadSource(PSTR(": (h) #16 ;          : here (h) @ ;"));
    loadSource(PSTR(": (l) #20 ;          : last (l) @ ;"));
    loadSource(PSTR(": base  #24 ;        : state #28 ;"));
    loadSource(PSTR(": sp0   #32 @ ;      // : rp0   #36 @ ;"));
    loadSource(PSTR(": (dsp) #40 ;        : dsp (dsp) @ ;"));
    loadSource(PSTR("// : (rsp) #44 ;        : rsp (rsp) @ ;"));
    loadSource(PSTR(": !sp 0 (dsp) ! ;    // : !rsp 0 (rsp) ! ;"));
    loadSource(PSTR("// : cells 4 * ;        // : cell+ 4 + ;"));
    loadSource(PSTR(": inline    2 last addr + c! ;"));
    loadSource(PSTR("// : immediate 1 last addr + c! ;"));
    loadSource(PSTR(": nip swap drop   ; inline"));
    loadSource(PSTR(": tuck swap over  ; inline"));
    loadSource(PSTR(": / /mod nip      ; inline"));
    loadSource(PSTR(": mod /mod drop   ; inline"));
    loadSource(PSTR(": +! tuck @ + swap ! ;"));
    loadSource(PSTR(": <> = not ; inline"));
    loadSource(PSTR(": ?dup if- dup then ;"));
    loadSource(PSTR(": abs dup 0 < if 0 swap - then ;"));
    loadSource(PSTR(": execute >r ;"));
    loadSource(PSTR(": depth dsp 1- ;"));
    loadSource(PSTR("// : pick depth swap - 1- cells sp0 + @ ;"));
    loadSource(PSTR(": rot >r swap r> swap ;"));
    loadSource(PSTR(": min over over < if drop else nip then ;"));
    loadSource(PSTR(": max over over > if drop else nip then ;"));
    loadSource(PSTR(": between rot dup >r min max r> = ;"));
    loadSource(PSTR(": cr #13 emit #10 emit ;"));
    loadSource(PSTR(": space $20 emit ; inline"));
    loadSource(PSTR(": count dup 1+ swap c@ ;"));
    loadSource(PSTR(": . 0 num>str space type ;"));
    loadSource(PSTR(": .2 2 num>str type ; : .4 4 num>str type ;"));
    loadSource(PSTR(": hex $10 base ! ; : decimal #10 base ! ; : binary 2 base ! ;"));
    loadSource(PSTR(": allot here + (h) ! ;"));
    loadSource(PSTR("// : [ 0 state ! ; immediate"));
    loadSource(PSTR("// : ] 1 state ! ;"));
    loadSource(PSTR(": .wordl dup .4 space dup >body .4 addr + dup c@ . 1+ space count type cr ;"));
    loadSource(PSTR(": .word addr + 1+ count type space ;"));
    loadSource(PSTR(": wordsl last begin dup .wordl a@ while- drop ;"));
    loadSource(PSTR(": words  last begin dup .word  a@ while- drop ;"));
}
// ^^^^^ - NimbleText generated - ^^^^^
