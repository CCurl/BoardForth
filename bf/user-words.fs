forth definitions
: all 0 ;

variable seed
timer seed !

: random    ( -- x )
    seed @
    dup 13 lshift xor
    dup 17 rshift xor
    dup 5  lshift xor
    dup seed ! ;
 
: rand ( n1 -- n2 ) 
    random abs swap mod ;

: pc 64 ; 
pc definitions
variable (regs) #15 CELLS allot
: _t1 #15 and CELLS (regs) + ;
: reg@ _t1 @ ; : reg! _t1 ! ;
: reg+ _t1 dup @ swap over 1+ swap ! ;
: reg- _t1 dup @ swap over 1- swap ! ;

: csi 27 emit '[' emit ;
: gotoXY csi (.) ';' emit (.) 'H' emit ;
: cls csi ." 2J" 0 dup gotoXY ;

all definitions
marker
